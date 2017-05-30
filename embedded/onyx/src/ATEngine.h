#ifndef _ESP12E_H_
#define _ESP12E_H_

#include "circularBuffer.h"
#include "uart_usb.h"

#include <stdint.h>

#define AT_STD_CMD_SIZE   64
#define AT_STD_RESP_SIZE  512

#define MAX_READ_GAP_MS   100
#define DEFAULT_TIMEOUT  2000

//#define DEBUG_AT_ENGINE


struct ATCommand_s {
	const char *cmd;
	const char *resp;

	int (*callBack)(uint8_t cmdNdx, char *resp);

	int timeout;
};

enum ATState_e {
	AT_IDLE=0, AT_SEND, AT_RESPONSE, AT_UNSOLICITED, AT_CLEAN_UP, AT_ERROR, AT_STOP
};

enum errcode_e {
	NO_ERR=0, RESP_ERR, SEND_ERR, TIMEOUT_ERR
};

class ATEngine_c {

private:
	//SerialPort 
	SerialPort _port;

	//Circular Buffer
	circularBuffer<uint8_t> *_inBuf;
	
	//AT Command List and Parameters
	ATCommand_s *_ATCmdList;
	uint8_t _ATListLen;

	//State varibles
	ATState_e _state;

	//MS variables
	unsigned long _last_saved_ms;
	unsigned long _last_read_ms;
	

	struct {
		uint8_t _dataOrCmd;
		uint8_t _cmdNdx;

		char *_dataPtr;
		uint16_t _dataLen;

		char _respBuf[AT_STD_RESP_SIZE];
		char *_respPtr;
		uint16_t _respLen;
	};

	errcode_e _errcode;

	bool _unsolicited_accept;

	int (*unsolicited_handler)(char *data);
	int (*data_resp_call_back)(char *resp);


public:
	ATEngine_c(void) { _ATListLen = 0; _ATCmdList = NULL; _inBuf = NULL; _state = AT_STOP; }
	ATEngine_c(char *p, circularBuffer<uint8_t> *inBuf, ATCommand_s *list, uint8_t ATListLen);

	int init(char *p, circularBuffer<uint8_t> *inBuf, ATCommand_s *list, uint8_t ATListLen);
	void reset_engine(void);

	bool isStable() { return (_inBuf->isEmpty() && (_state == AT_IDLE) && (!hasErr())); }

	/* Unsolicited Response Handler */
	void bindUnsolicitedHandler(int (*func)(char *)) { unsolicited_handler = func; }
	void unsolicitedAccept(bool yesOrNo) { _unsolicited_accept = yesOrNo; }

	void bindDataHandler(int (*func)(char *)) { data_resp_call_back = func; }

	int updateEngine(unsigned long tick);

	int queue_cmd(uint8_t cmdNdx, ...);
	int queue_data(char *data, size_t count);

	int getErrCode() { return _errcode; }
	void clearErr() { _errcode = NO_ERR; }
	bool hasErr() { return (_errcode != NO_ERR); }

	~ATEngine_c() { _port.close(); } /*Destructor */

private:

	int send(void);

	int send_cmd(void);
	int send_data(void);

	bool isAvailable(void) { return (_respLen > 0); }	

	int process_response(void);

	void clear_buf();
	void clear_response_buf(void);
	void clear_data_buf(void);
	void clear_cmd_buf(void);

	void changeStateTo(ATState_e state) { _state = state; }

	void setErrCode(errcode_e code) { if (_errcode == NO_ERR) _errcode = code; }

	int fill_resp_buf(unsigned long ms);

};


#endif 