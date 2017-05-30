#include "gpio.h"
#include "Arduino.h"

#include <stdbool.h>


#define KEY_DSTATE						0x01  // 0=> LOW and 1=> HIGH
#define HOLD_STATE						0x02  // 0=> NOT HOLD and 1=> BEING HOLD
#define ONE_CLICK						0x04  // 0=> NOT PRESSED MORE THAN ONCE and 1=> PRESSED MORE THAN ONCE
#define TWO_CLICKS						0x08
#define THREE_CLICKS					0x10
#define RISING_EDGE						0x20  // 1=> DETECTED RISING EDGE
#define FALLING_EDGE					0x40  // 1=> DETECTED FALLING EDGE

#define PULLED_DOWN						0x01
#define PULLED_UP						  0x00

#define getKeyDState(k)					((key[k].events & KEY_DSTATE) != 0)
#define setKeyDState(k,s)				(s ? (key[k].events |= KEY_DSTATE) : (key[k].events &= ~KEY_DSTATE))
#define isPressed(k)        			(isPulledDown(k) ? getKeyDState(k) : !getKeyDState(k))
#define isPressedState(k,s)				(isPulledDown(k) ? s : !s)
#define raiseEdgeEvent(k,newDState)		(newDState ? (key[k].events |= RISING_EDGE) : (key[k].events |= FALLING_EDGE))
#define raiseEvent(k,e)					(key[k].events |= e)
#define isEventRaised(k,e)				((key[k].events & e) != 0)

#define clearEvent(k,e)					(key[k].events &= ~e)
#define clearAllEvents(k)				(key[k].events &= 0x01)

#define isPulledUp(k)					((key[k].flags & 0x01) == PULLED_UP)
#define isPulledDown(k)					((key[k].flags & 0x01) == PULLED_DOWN)


#define MAX_COUNT         				5
#define MAX_HOLD          				150
#define MAX_WAIT          				100

enum {
	CONFIG_KEY,
	LOCK_STATE_KEY_1,
	LOCK_STATE_KEY_2,
	KEY_SENTINEL
};

struct key_s {
	uint8_t pin;
	uint8_t flags;
	uint8_t events;

	uint8_t state;
	uint8_t offTTicks;
	uint8_t onTTricks;
};

void gpio_init(void)
{
	pinMode(CONFIG_BUTTON, INPUT);
	pinMode(WIFI_ADAPTER_RESET, OUTPUT);
	digitalWrite(WIFI_ADAPTER_RESET, HIGH);
	pinMode(WIFI_ADAPTER_ENABLE, OUTPUT);
	digitalWrite(WIFI_ADAPTER_ENABLE, HIGH);

	pinMode(GREEN_LED, OUTPUT);
	digitalWrite(GREEN_LED, LOW);
	pinMode(LOCK_ACTUATE, OUTPUT);
	digitalWrite(LOCK_ACTUATE, LOW);

	pinMode(LOCK_STATE_SWITCH_2, INPUT);
	digitalWrite(LOCK_STATE_SWITCH_2, HIGH);
	pinMode(LOCK_STATE_SWITCH_1, INPUT);
	digitalWrite(LOCK_STATE_SWITCH_1, HIGH);

	return;
}


struct key_s key[KEY_SENTINEL] = {
	[CONFIG_KEY]       =  { CONFIG_BUTTON,    		PULLED_UP, 	0x00, 	0x00, 	0x00, 	0x00},
	[LOCK_STATE_KEY_1] =  { LOCK_STATE_SWITCH_2,  	PULLED_UP, 	0x00, 	0x00, 	0x00, 	0x00},
	[LOCK_STATE_KEY_2] =  { LOCK_STATE_SWITCH_2,  	PULLED_UP, 	0x00,	0x00, 	0x00, 	0x00}

};

enum {
	KEY_IDLE,
	KEY_DEBOUNCE,
	KEY_PRESSED,
	KEY_CLICK_CHECK
};


void key_init()
{
  int i;
  for (i=0; i<KEY_SENTINEL; i++) {
    key[i].state = KEY_IDLE;
    if (isPulledDown(i))
      setKeyDState(i,false);
    else
      setKeyDState(i,true);
    key[i].offTTicks=0x00;
  }
}


void key_event_update()
{
	int k;
	for (k=0; k<KEY_SENTINEL; k++) {
		bool raw = digitalRead(key[k].pin);
		if (key[k].offTTicks < MAX_WAIT)
			key[k].offTTicks++;
		switch (key[k].state) {
		case KEY_IDLE:
			if (getKeyDState(k) == raw) 
        		break;
      		key[k].onTTricks = 0;
      		key[k].state = KEY_DEBOUNCE;
      		break;
    	case KEY_DEBOUNCE:
      		key[k].onTTricks++;
      		if (getKeyDState(k) == raw)
        		key[k].onTTricks = 0;
      		if (key[k].onTTricks < MAX_COUNT)
        		break;
       		raiseEdgeEvent(k,raw);
      		setKeyDState(k,raw);
      		key[k].state = isPressed(k) ? KEY_PRESSED : KEY_CLEANUP;
      		key[k].onTTricks = 0;
      		break;
    	case KEY_PRESSED:
    	  	if (raw != getKeyDState(k)) {
        		key[k].state = KEY_DEBOUNCE;
        		key[k].onTTricks = 0;
      		}
      		if (key[k].onTTricks > MAX_HOLD)
        		key[k].events |= HOLD_STATE;
        	else
        		key[k].onTTricks++;
        	break;
    	case KEY_CLICK_CHECK:
    		raiseEvent(k, ONE_CLICK);
      		if (key[k].offTTicks < MAX_WAIT)
      			raiseEvent(k, TWO_CLICKS);
     		key[k].offTTicks = 0x00;
      		key[k].state = KEY_IDLE;
      		break;
    	}
  	}
}

#define MAX_KEYPAD_ROWS   10
#define MAX_KEYPAD_COLUMS 10

struct key_s keypad_rows[MAX_KEYPAD_ROWS];
struct key_s keypad_colums[MAX_KEYPAD_COLUMS];

struct keypad_s {
  int num_rows;
  int num_colums;
};

int keypad_init(struct keypad_s *pad, int *rows, int num_rows, int *colums, int num_colums)
{
  pad->num_rows = num_rows;
  pad->num_colums = num_colums;

  int i;
  for (i=0; i<num_rows; i++) {
    keypad_rows.pin = rows[i];
    keypad_rows.flags = PULLED_UP;
  }
  for (i=0; i<num_colums; i++) {
    keypad_colums.pin = colums[i];
    keypad_colums.flags = PULLED_UP;
  }

  return 0;
}

int keypad_onevent(struct keypad_s *pad, int *key_pressed)
{
  int i, j;
  for (i=0; i<pad->num_colums; i++) {
    digitalWrite(keypad_colums.pin, HIGH);
    for (j=0; j<pad->num_rows; j++) {
      if (digitalRead(keypad_rows.pin) == HIGH)
        goto DETECTED;
    }
  }

DETECTED:
  if (j >= pad->num_rows)
    return -1;
  *key_pressed = j;
  for (i=0; i<pad->num_colums; i++) {
    digitalWrite(keypad_colums.pin, LOW);
  }
}