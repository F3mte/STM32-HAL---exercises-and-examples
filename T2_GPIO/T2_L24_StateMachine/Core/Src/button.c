/*
 * button.c
 *
 *  Created on: Oct 21, 2020
 *      Author: Dominik
 */

#include "main.h"
#include "button.h"

TBUTTON BlueKey;

void ButtonInitKey(TBUTTON* Key, GPIO_TypeDef* GpioPort, uint16_t GpioPin, uint32_t TimerIdle,
					uint32_t TimerDebounce, uint32_t TimerPressed, uint32_t TimerRepeat)
{
	Key->GpioPort = GpioPort;
	Key->GpioPin = GpioPin;
	Key->State = IDLE;
	Key->Timer = 0;
	Key->TimerIdle = TimerIdle;
	Key->TimerDebounce = TimerDebounce;
	Key->TimerPressed = TimerPressed;
	Key->TimerRepeat = TimerRepeat;
}

void ButtonIdleRoutine(TBUTTON* Key)
{
	if( (HAL_GetTick() - Key->LastTick) > Key->TimerIdle)
	{
		if( GPIO_PIN_RESET == HAL_GPIO_ReadPin(Key -> GpioPort, Key -> GpioPin) )
		{
			Key -> State = DEBOUNCE;
			Key -> Timer = Key -> TimerDebounce;
		}
		else
		{
			Key -> State = IDLE;
			Key -> Timer = Key -> TimerIdle;
		}

		Key -> LastTick = HAL_GetTick();
	}
}

void ButtonRegisterPressCallback(TBUTTON* Key, void* Callback)
{
	Key->ButtonPress = Callback;
}

void ButtonRegisterLongPressCallback(TBUTTON* Key, void* Callback)
{
	Key->ButtonLongPress = Callback;
}

void ButtonRegisterRepeatPressCallback(TBUTTON* Key, void* Callback)
{
	Key->ButtonRepeatPress = Callback;
}

void ButtonRegisterReleaseCallback(TBUTTON* Key, void* Callback)
{
	Key->ButtonReleasePress = Callback;
}

void ButtonDebounceRoutine(TBUTTON* Key)
{
	if( GPIO_PIN_RESET == HAL_GPIO_ReadPin(Key->GpioPort, Key->GpioPin) )
	{
		if( HAL_GetTick() - Key->LastTick > Key->Timer)
		{
			if( Key->ButtonPress != NULL)
			{
				Key->ButtonPress();
			}
			Key->State = PRESSED;
			Key->Timer = Key->TimerPressed;
			Key->LastTick = HAL_GetTick();
		}
	}
	else
	{
		Key -> State = IDLE;
		Key -> Timer = Key -> TimerIdle;
	}
}

void ButtonPressedRoutine(TBUTTON* Key)
{
	if( GPIO_PIN_RESET == HAL_GPIO_ReadPin(Key->GpioPort, Key->GpioPin) )
	{
		if( HAL_GetTick() - Key->LastTick > Key->Timer)
		{
			if( Key->ButtonLongPress != NULL)
			{
				Key->ButtonLongPress();
			}
			Key->State = REPEAT;
			Key->Timer = Key->TimerRepeat;
			Key->LastTick = HAL_GetTick();
		}
	}
	else
	{
		Key -> State = RELEASE;
	}
}

void ButtonRepeatRoutine(TBUTTON* Key)
{
	if( GPIO_PIN_RESET == HAL_GPIO_ReadPin(Key->GpioPort, Key->GpioPin) )
	{
		if( HAL_GetTick() - Key->LastTick > Key->Timer)
		{
			if( Key->ButtonRepeatPress != NULL)
			{
				Key->ButtonRepeatPress();
			}

			Key->LastTick = HAL_GetTick();
		}
	}
	else
	{
		Key -> State = RELEASE;
	}
}

void ButtonReleaseRoutine(TBUTTON* Key)
{
	if( Key->ButtonReleasePress != NULL)
		{
			Key->ButtonReleasePress();
		}
	Key->State = IDLE;
	Key->Timer = Key->TimerIdle;
	Key->LastTick = HAL_GetTick();
}

void ButtonTask(TBUTTON* Key)
{
	switch (Key -> State)
	{
		case IDLE:		ButtonIdleRoutine(Key);		break;
		case DEBOUNCE:	ButtonDebounceRoutine(Key);	break;
		case PRESSED:	ButtonPressedRoutine(Key);	break;
		case REPEAT:	ButtonRepeatRoutine(Key);	break;
		case RELEASE:	ButtonReleaseRoutine(Key);	break;
		default:									break;
	}

}

