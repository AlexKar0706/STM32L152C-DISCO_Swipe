/*
 * swipe.c
 *
 *  Created on: Dec 15, 2021
 *      Author: User
 */

/* Includes ------------------------------------------------------------------*/

#include "swipe.h"
#include "touchsensing.h"

/* Private variables ---------------------------------------------------------*/

extern TSL_LinRot_T MyLinRots[];  //Global array with data from sensor slider

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Execute SWIPE_READY state
 * @param  Swipe: Current Swipe object
 * @retval None
 */
static void Swipe_Ready_Exec(SwipeObj* Swipe) {
	/* Check, if new action was detected on slider. Else clear sensor value */
	if (SENSOR_AREA_ACTIVE) Swipe->State = SWIPE_START;
	else SENSOR_AREA_POSITION = 0;
}


/**
 * @brief  Execute SWIPE_START state
 * @param  Swipe: Current Swipe object
 * @retval None
 */
static void Swipe_Start_Exec(SwipeObj* Swipe) {

	/* Check, if sensor is not currently active */
	if (!SENSOR_AREA_ACTIVE) {
		Swipe_Reset(Swipe);
		return;
	}

	if (SENSOR_AREA_POSITION < SWIPE_RIGHT_MIN_VAL) { /* If sensor position < SWIPE_RIGHT_MIN_VAL, then it is swipe from right to left */
		Swipe->Data.Destination = DESTINATION_RIGHT;
		Swipe->Data.Position = SENSOR_AREA_POSITION;
		Swipe->State = SWIPE_ON;
	} else if (SENSOR_AREA_POSITION > SWIPE_LEFT_MIN_VAL) { /* If sensor position > SWIPE_LEFT_MIN_VAL, then it is swipe from left to right */
		Swipe->Data.Destination = DESTINATION_LEFT;
		Swipe->Data.Position = MAX_SENSOR_VALUE - SENSOR_AREA_POSITION;
		Swipe->State = SWIPE_ON;
	} else { /* Else movement is recognized as illegal movement for swipe */
		Swipe_Reset(Swipe);
	}

}

/**
 * @brief  Execute SWIPE_ON state
 * @param  Swipe: Current Swipe object
 * @retval None
 */
static void Swipe_On_Exec(SwipeObj* Swipe) {
	uint8_t sensorPosition = 0;

	/* Check, if sensor is not currently active */
	if (!SENSOR_AREA_ACTIVE) {
		Swipe_Reset(Swipe);
		return;
	}

	/* Add one time frame to total number of time frames during swipe */
	Swipe->Data.Swipe_time += TIME_FRAME_SWIPE;

	/* Check that the movement does not go beyond time configuration limits */
	if (Swipe->Data.Swipe_time > Swipe->TimeConf) {
		Swipe_Reset(Swipe);
		return;
	}

	/* Configure sensor position accordingly to swipe destination */
	if (Swipe->Data.Destination == DESTINATION_LEFT)
		sensorPosition = MAX_SENSOR_VALUE - SENSOR_AREA_POSITION;
	else
		sensorPosition = SENSOR_AREA_POSITION;

	/* Compare previous and current swipe position */
	if (sensorPosition == Swipe->Data.Position) {

		/* Do nothing, if position didn't change */

	} else if (sensorPosition > Swipe->Data.Position) {

		/* Record new position, if position number increased */
		Swipe->Data.Position = sensorPosition;
		Swipe->Data.Position_count++;

	} else {

		/* Restart system, if position number decreased */
		Swipe_Reset(Swipe);
		return;

	}

	/* Check, if position is recognized as swipe movement */
	if (Swipe->Data.Position >= FINAL_POS && Swipe->Data.Position_count >= MINIMUM_POSITIONS_NUMBER)
		Swipe->State = SWIPE_SUCCESS;
}


/**
 * @brief  Execute SWIPE_SUCCESS state
 * @param  Swipe: Current Swipe object
 * @retval None
 */
static void Swipe_Success_Exec(SwipeObj* Swipe) {
	/* Activate LED and reset swipe object */
	Swipe->LED_Timer = LED_3SEC_TIME_FRAME;
	Swipe_Reset(Swipe);
}

/* Public functions ---------------------------------------------------------*/

/**
 * @brief  Initialisation function for Swipe object
 * @param  Swipe: Currently created swipe object
 * @retval None
 */
void Swipe_Init(SwipeObj* Swipe) {
	Swipe->Data.Destination = DESTINATION_OFF;
	Swipe->Data.Position = 0;
	Swipe->Data.Position_count = 0;
	Swipe->Data.Swipe_time = 0;
	Swipe->Button = BUTTON_READY;
	Swipe->TimeConf = MS200;
	Swipe->LED_Timer = 0;
	Swipe->State = SWIPE_READY;
}


/**
 * @brief  Reset position data in Swipe object
 * @param  Swipe: object, that needed to be reset
 * @retval None
 */
void Swipe_Reset(SwipeObj* Swipe) {
	Swipe->Data.Destination = DESTINATION_OFF;
	Swipe->Data.Position = 0;
	Swipe->Data.Position_count = 0;
	Swipe->Data.Swipe_time = 0;
	Swipe->State = SWIPE_READY;
}


/**
 * @brief  Main function, that can implement rotation of Swipe states
 * @param  Swipe: Currently used Swipe object
 * @retval None
 */
void Swipe_State_Machine(SwipeObj* Swipe) {

	/* Main swipe state machine consists of 5 different states */
	switch (Swipe->State) {

	/* State SWIPE_OFF: Data in Swipe object is not ready */
	case SWIPE_OFF:
		Swipe_Reset(Swipe);
		break;

	/* State SWIPE_READE: Swipe object is initialised and waiting for slider information */
	case SWIPE_READY:
		Swipe_Ready_Exec(Swipe);
		break;

	/* State SWIPE_START: New action was detected on slider, state machine trying to understand action */
	case SWIPE_START:
		Swipe_Start_Exec(Swipe);
		break;

	/* State SWIPE_ON: Action was successfully initialised, state machine gathering data to recognize swipe */
	case SWIPE_ON:
		Swipe_On_Exec(Swipe);
		break;

	/* State SWIPE_SUCCESS: Action was successfully recognized as swipe movement */
	case SWIPE_SUCCESS:
		Swipe_Success_Exec(Swipe);
		break;

	default: break;

	}
}

/**
 * @brief  Rotate Swipe object time configurations
 * @param  Swipe:  currently used Swipe object
 * @retval None
 */
void Swipe_Switch_Config(SwipeObj* Swipe) {
	/* Switch time configuration MS200->MS500->MS1000 */
	switch (Swipe->TimeConf) {
	case MS200:
		Swipe->TimeConf = MS500;
		break;
	case MS500:
		Swipe->TimeConf = MS1000;
		break;
	case MS1000:
		Swipe->TimeConf = MS200;
		break;
	default: break;
	}
}


/**
 * @brief  Switch Swipe time configuration by using button
 * @param  Swipe: currently used Switch object
 * @retval None
 * NB! for this project internal button B1 is used
 */
void Procces_Button (SwipeObj* Swipe) {

	if (READ_B1_PIN == GPIO_PIN_SET && Swipe->Button == BUTTON_PRESSED) return;

	if (READ_B1_PIN == GPIO_PIN_RESET) {
		Swipe->Button = BUTTON_READY;
		return;
	}

	DEBOUNCE_CHECK(DEBOUNCE_TIME_MS);

	Swipe_Switch_Config(Swipe);

	Swipe->Button = BUTTON_PRESSED;
}

/**
 * @brief  Execute 1/10 200Hz PWM cycle of LED
 * @param  Swipe: currently used Switch object
 * @retval None
 * NB! for this project internal LED LD4 is used
 */
void LED_Handle(SwipeObj* Swipe) {
	static uint16_t count = 0; //static value keeping track of 70% PWM signal

	/* Check if LED should be turned ON */
	if (Swipe->LED_Timer == 0) return;

	/* Create PWM 70%. If count = 0...6, then LD4 is turned ON, if count = 7...9 then LD4 is turned OFF */
	if (count >= 7) HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
	else HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);

	/* restart PWM 70% cycle */
	if (count == 9) {
		count = 0;
		Swipe->LED_Timer--;
	}

	count++;  // 1/10 of PWM cycle is passed
}
