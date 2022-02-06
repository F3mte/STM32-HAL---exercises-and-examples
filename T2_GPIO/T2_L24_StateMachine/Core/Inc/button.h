/*
 * button.h
 *
 *  Created on: Oct 21, 2020
 *      Author: Dominik
 */

#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

typedef enum
{
	IDLE = 0,
	DEBOUNCE,
	PRESSED,
	REPEAT,
	RELEASE
} BUTTON_STATE;

typedef struct button_struct
{
	BUTTON_STATE	State;
	GPIO_TypeDef*	GpioPort;
	uint16_t		GpioPin;
	uint32_t		LastTick;
	uint32_t		Timer;
	uint32_t		TimerIdle;
	uint32_t		TimerDebounce;
	uint32_t		TimerPressed;
	uint32_t		TimerRepeat;
	void(*ButtonPress)(void);
	void(*ButtonLongPress)(void);
	void(*ButtonRepeatPress)(void);
	void(*ButtonReleasePress)(void);
} TBUTTON;

void ButtonInitKey(TBUTTON* Key, GPIO_TypeDef* GpioPort, uint16_t GpioPin, uint32_t TimerIdle, uint32_t TimerDebounce,
					uint32_t TimerPressed, uint32_t TimerRepeat);
void ButtonRegisterPressCallback(TBUTTON* Key, void* Callback);
void ButtonRegisterLongPressCallback(TBUTTON* Key, void* Callback);
void ButtonRegisterRepeatPressCallback(TBUTTON* Key, void* Callback);
void ButtonRegisterReleaseCallback(TBUTTON* Key, void* Callback);
void ButtonTask(TBUTTON* Key);

#endif /* INC_BUTTON_H_ */
