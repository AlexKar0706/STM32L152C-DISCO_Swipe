/*
 * swipe.h
 *
 *  Created on: Dec 15, 2021
 *      Author: User
 */

#ifndef INC_SWIPE_H_
#define INC_SWIPE_H_

/* Includes -----------------------------------------------------------*/

#include "main.h"
#include "touchsensing.h"

/* Defines ------------------------------------------------------------*/

#define MAX_SENSOR_VALUE (uint8_t)256/(9-TSLPRM_LINROT_RESOLUTION)  /** < Maximum value of linear slider > */

/* Optional defines ---------------------------------------------------*/
#define FINAL_POS 110  /** < Final slider position to initiate SWIPE_SUCCESS state > */
#define SWIPE_RIGHT_MIN_VAL 10 /** < Minimum linear slider position needed to initiate right swipe movement > */
#define SWIPE_LEFT_MIN_VAL 90  /** < Minimum linear slider position needed to initiate left swipe movement > */
#define MINIMUM_POSITIONS_NUMBER 5 /** < Minimum number of different positions saved by Swipe state machine > */

/* Time frame defines -------------------------------------------------*/

#define TIME_FRAME_SWIPE 1  /** < One time frame value > */
#define LED_3SEC_TIME_FRAME 600 /**  < 3000/(IRQ_period(TIM7) / SYSCLK * 1000 * 10) > */
#define MS200_TIME_FRAME 100    /**  < 200/(IRQ_period(TIM6) / SYSCLK * 1000)  > */
#define MS500_TIME_FRAME 250    /**  < 500/(IRQ_period(TIM6) / SYSCLK * 1000)  > */
#define MS1000_TIME_FRAME 500   /**  < 1000/(IRQ_period(TIM6) / SYSCLK * 1000) > */

/* Macros ------------------------------------------------------------*/

#define SENSOR_AREA_ACTIVE (MyLinRots[0].p_Data->StateId == TSL_STATEID_DETECT || \
							MyLinRots[0].p_Data->StateId == TSL_STATEID_TOUCH) /** < Check if linear sensor area is active > */

#define SENSOR_AREA_POSITION MyLinRots[0].p_Data->Position /** < Current linear sensor position > */

#define READ_B1_PIN HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) /** < Read value of B1 blue button > */
#define DEBOUNCE_TIME_MS 20 /** < Debounce time for B1 blue button > */

#define DEBOUNCE_CHECK(ms) HAL_Delay(ms);\
	if (READ_B1_PIN == GPIO_PIN_RESET) return /** < Check for debouncing of B1 blue button > */

/**
  * @brief  Swipe button state enum definition
  */
typedef enum {
	BUTTON_READY = 0,  /** < Button currently is not pressed > */
	BUTTON_PRESSED = 1  /** < Button currently is pressed > */
} swipe_button_state_t;

/**
  * @brief  Swipe time config state enum definition
  */
typedef enum {
	NO_CONFIG = 0,  /** < The value, that should not be loaded into Switch object, but can be used by user > */
	MS200 = MS200_TIME_FRAME,   /** < Needed amount of time frames to count 200ms > */
	MS500 = MS500_TIME_FRAME,   /** < Needed amount of time frames to count 500ms > */
	MS1000 = MS1000_TIME_FRAME   /** < Needed amount of time frames to count 1000ms > */
} swipe_config_t;

/**
  * @brief  Swipe object state enum definition
  */
typedef enum {
	SWIPE_OFF = 0,      /** < Swipe is OFF > */
	SWIPE_READY = 1,    /** < Swipe object is ready to initiate swipe procedure > */
	SWIPE_START = 2,    /** < Swipe object is started swipe procedure > */
	SWIPE_ON = 3,       /** < Swipe object is reading slider to get swipe movement > */
	SWIPE_SUCCESS = 4   /** < Swipe object is recognized swipe movement > */
} swipe_state_t;

/**
 * @brief Swipe object destination enum definition
 */
typedef enum {
	DESTINATION_OFF = 0,    /** < No swipe is detected > */
	DESTINATION_RIGHT = 1,  /** < Swipe destination is from right to left > */
	DESTINATION_LEFT = 2    /** < Swipe destination is from left to right > */
} swipe_destination_t;

/**
  * @brief  Swipe current data structure definition
  */
typedef struct {
	swipe_destination_t Destination;    /** < Current swipe destination > */
	uint8_t Position;                   /** < Current position of slider > */
	uint16_t Position_count;            /** < Number of different position encountered during swipe procedure > */
	uint16_t Swipe_time;                /** < Number of time frames passed during SWIPE_ON procedure > */
} current_swipe_t;

/**
  * @brief  Swipe object structure definition
  */
typedef struct {
	swipe_state_t State;          /** < Current state of Swipe state machine > */
	swipe_config_t TimeConf;      /** < Current time configuration of swipe > */
	current_swipe_t Data;         /** < Current data of swipe > */
	swipe_button_state_t Button;  /** < Current button state of B1 button > */
	uint32_t LED_Timer;           /** < Amount of time frames for LED, when swipe movement is recognized > */
} SwipeObj;

/* Function prototypes ------------------------------------------------------------*/

void Swipe_Init(SwipeObj* Swipe);
void Swipe_Reset(SwipeObj* Swipe);
void Swipe_State_Machine(SwipeObj* Swipe);
void Swipe_Switch_Config(SwipeObj* Swipe);
void Procces_Button (SwipeObj* Swipe);
void LED_Handle(SwipeObj* Swipe);

#endif /* INC_SWIPE_H_ */
