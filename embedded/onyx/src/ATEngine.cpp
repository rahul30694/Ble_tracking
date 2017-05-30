#include "uart_usb.h"
#include "circularBuffer.h"
#include "ATEngine.h"
#include "time.h"
#include "serial.h"
#include "cstring.h"

#include <iostream>

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>


ATEngine_c::ATEngine_c(char *p, circularBuffer<uint8_t> *inBuf, ATCommand_s *list, uint8_t ATListLen)
{
	init(p, inBuf, list, ATListLen);
}

int ATEngine_c::init(char *p, circularBuffer<uint8_t> *inBuf, ATCommand_s *list, uint8_t ATListLen)
{
	_port.open(p); //Open the serial port to communicate
		 
	_inBuf=inBuf; 

	_ATCmdList=list; 
	_ATListLen=ATListLen;

	clearErr();
	reset_engine();

	return 0;
}

void ATEngine_c::reset_engine()
{
	_inBuf->reset();

	_state=AT_IDLE;

	_last_saved_ms=0;
	_last_read_ms=0;

	_cmdNdx=-1;

	clear_data_buf();
	clear_response_buf();
	clear_cmd_buf();	
}


int ATEngine_c::updateEngine(unsigned long ms)
{
	fill_resp_buf(ms);

	switch (_state) {
	case AT_IDLE:
	{
		if (isAvailable()) {
#ifdef DEBUG_AT_ENGINE
			serialPrintln("Unsolicited Response");
#endif
			changeStateTo(AT_UNSOLICITED);
			break;
		}
		if (_inBuf->isEmpty())
			break;
		changeStateTo(AT_SEND);
	}
	//Fall Though
	case AT_SEND:
	{
		if (send()) {
			changeStateTo(AT_ERROR);
			setErrCode(SEND_ERR);
			break;
		}
		_last_saved_ms = ms;
		changeStateTo(AT_RESPONSE);
		break;
	}
	case AT_RESPONSE:
	{
		unsigned timeout = (_dataOrCmd == 'c') ? _ATCmdList[_cmdNdx].timeout : DEFAULT_TIMEOUT;
		if ((ms - _last_saved_ms) >= timeout) {
			changeStateTo(AT_ERROR);
			setErrCode(TIMEOUT_ERR);
#ifdef DEBUG_AT_ENGINE
			serialPrintln("Timeout");
#endif
			break;
		}

		int ret = 0;
		if (_dataOrCmd == 'd') {
			if (data_resp_call_back)
				ret = data_resp_call_back(_respBuf);
		} else {
			if (_ATCmdList[_cmdNdx].callBack)
				ret = _ATCmdList[_cmdNdx].callBack(_cmdNdx, _respBuf);
			else
				ret = process_response();
		}

		if (ret == 0) {
			changeStateTo(AT_CLEAN_UP);
		} else if (ret != -1) {
			changeStateTo(AT_ERROR);
			setErrCode(RESP_ERR);
		}

		break;
	}
	case AT_UNSOLICITED:
	{
		if (ms - _last_read_ms < MAX_READ_GAP_MS)
			break;
		if (_unsolicited_accept && unsolicited_handler)
			unsolicited_handler(_respBuf);
		changeStateTo(AT_CLEAN_UP);
		break;
	}
	case AT_CLEAN_UP: 
	{
#ifdef DEBUG_AT_ENGINE
		serialPrintln("Response - %s", _respBuf);
#endif
		clear_buf();
		changeStateTo(AT_IDLE);
		break;
	}
	case AT_ERROR:
	{
		reset_engine();
		changeStateTo(AT_CLEAN_UP);
		break;
	}
	default: 
	{
		break;
	}
	}

	if (hasErr())	
		return -1;
	return 0;
}

int ATEngine_c::send_data()
{
#ifdef DEBUG_AT_ENGINE
	serialPrintln("Data - %s", _dataPtr);
#endif
	_port.write(_dataPtr, _dataLen);
	return 0;
}

int ATEngine_c::send_cmd()
{
	_inBuf->deque(&_cmdNdx);

	if (_cmdNdx >= _ATListLen)
		return -1;

	uint8_t len;
	if (_inBuf->deque(&len))
		return -1;

	int i;
#ifdef DEBUG_AT_ENGINE
	serialPrint("Cmd - ");
#endif
	for (i=0; i<len; i++) {
		uint8_t ptr;
		if (_inBuf->deque(&ptr)) 
			break;
#ifdef DEBUG_AT_ENGINE
		serialPrint("%c", (char )ptr);
#endif
		_port.putchar((char *)&ptr);
	}
#ifdef DEBUG_AT_ENGINE
	serialPrintln("");
#endif
	return i >= len ? 0 : -1;
}


int ATEngine_c::send()
{
	_inBuf->deque(&_dataOrCmd);

	if (_dataOrCmd == 'd')
		return send_data();
		
	if (_dataOrCmd == 'c')
		return send_cmd(); 
	return -1;
}

int ATEngine_c::queue_cmd(uint8_t cmdNdx, ...)
{
	if (cmdNdx >= _ATListLen)
		return -1;

	va_list arg;
	va_start(arg, cmdNdx);
	char cmd[AT_STD_CMD_SIZE];

	vsnprintf(cmd, AT_STD_CMD_SIZE, _ATCmdList[cmdNdx].cmd, arg);

	uint8_t len = strnlen(cmd, AT_STD_CMD_SIZE);

	if (len >= AT_STD_CMD_SIZE)
		return -1;

	uint8_t cmd_type = 'c';
	_inBuf->enque(&cmd_type);
	_inBuf->enque(&cmdNdx);
	_inBuf->enque(&len);
	for (char *c = cmd; *c; c++)
		_inBuf->enque((uint8_t *)c);

	va_end(arg);
	return 0;
}


int ATEngine_c::queue_data(char *data, size_t count)
{
	if (_dataPtr)
		return -1;

	uint8_t data_type = 'd';
	_inBuf->enque(&data_type);
	_dataPtr = data;
	_dataLen = count;
	return 0;
}
	

int ATEngine_c::process_response()
{
	size_t str2Len = strnlen((char *)_ATCmdList[_cmdNdx].resp, AT_STD_RESP_SIZE);
	if (strstr(_respPtr, _ATCmdList[_cmdNdx].resp))
		return 0;

	int len = _respLen - str2Len;
	if (len > 0)
		_respPtr = _respBuf + len;
	return -1;
}
	

int ATEngine_c::fill_resp_buf(unsigned long ms)
{
	int bytes_read = _port.read((_respBuf + _respLen), AT_STD_RESP_SIZE - _respLen);

	if (bytes_read <= 0)
		return -1;

	_respLen += bytes_read;
	_respBuf[_respLen] = '\0'; //Null Terminated Response Buffer

	_last_read_ms = ms;
	return 0;
}

void ATEngine_c::clear_buf()
{
	clear_response_buf();
	if (_dataOrCmd == 'd')
		clear_data_buf();
	else
		clear_cmd_buf();
}

void ATEngine_c::clear_data_buf(void)
{
	_dataPtr = NULL;
	_dataLen = 0;
}

void ATEngine_c::clear_cmd_buf(void)
{
	_cmdNdx = -1;
}

void ATEngine_c::clear_response_buf()
{
	*_respBuf = '\0';
	_respPtr = _respBuf;
	_respLen = 0;
}