# STM32L152C-DISCO_Swipe
This application creates a "swipe" movement using STM32L152C-DISCO microcontroller's linear slider. In this program, "swipe" movement is considered as a finger movement on a linear slider from left to right (or from right to left). When a correct movement is done, microcontroller indicates it by turning ON the green internal LED LD3 for 3 seconds.   
This application has 3 different time configurations for "swipe" movement: **0.2 sec, 0.5 sec, 1.0 sec**. Every time configuration represents the time limit of every single "swipe" movement. For example, if time configuration was 0.5 sec and swipe duration was 0.4 sec, then this swipe is considered to be legal. But if time configuration was 0.5 sec and swipe duration was 0.6 sec, then this swipe will be abondoned.  
To change time configuration B1 blue button can be used. When B1 is pressed, microcontroller indicates new time configurations.  
LD3 has PWM 70% (200Hz).
## Created functions, enums, structs in swipe project
### List of all public functions
Full description of every function you can see directly in main.c/swipe.c file.
1. `void Swipe_Init(SwipeObj* Swipe);`
2. `void Swipe_Reset(SwipeObj* Swipe);`
3. `void Swipe_State_Machine(SwipeObj* Swipe);`
4. `void Swipe_Switch_Config(SwipeObj* Swipe);`
5. `void Procces_Button (SwipeObj* Swipe);`
6. `void LED_Handle(SwipeObj* Swipe);`
7. `void Display_Mode(LCD_HandleTypeDef *hlcd, SwipeObj *Swipe);`
### List of all private functions
Full description of every function you can see directly in swipe.c file. All of this function is needed to execute corresponding states.
1. `static void Swipe_Ready_Exec(SwipeObj* Swipe);`
2. `static void Swipe_Start_Exec(SwipeObj* Swipe);`
3. `static void Swipe_On_Exec(SwipeObj* Swipe);`
4. `static void Swipe_Success_Exec(SwipeObj* Swipe);`
### List of all enums
Button state enum:
```cpp
typedef enum {
	BUTTON_READY = 0,
	BUTTON_PRESSED = 1
} swipe_button_state_t;
```
Swipe time configuration enum:
```cpp
typedef enum {
	NO_CONFIG = 0,
	MS200 = MS200_TIME_FRAME,
	MS500 = MS500_TIME_FRAME,
	MS1000 = MS1000_TIME_FRAME
} swipe_config_t;
```
Swipe state machine enum:
```cpp
typedef enum {
	SWIPE_OFF = 0,
	SWIPE_READY = 1,
	SWIPE_START = 2,
	SWIPE_ON = 3,
	SWIPE_SUCCESS = 4
} swipe_state_t;
```
Swipe right/left destination enum:
```cpp
typedef enum {
	DESTINATION_OFF = 0,
	DESTINATION_RIGHT = 1,
	DESTINATION_LEFT = 2
} swipe_destination_t;
```
### List of all structs
Structure with all the data, that is captured during every swipe movement:
```cpp
typedef struct {
	swipe_destination_t Destination;
	uint8_t Position;
	uint16_t Position_count;
	uint16_t Swipe_time;
} current_swipe_t;
```
Main structure:
```cpp
typedef struct {
	swipe_state_t State;
	swipe_config_t TimeConf;
	current_swipe_t Data;
	swipe_button_state_t Button;
	uint32_t LED_Timer;
} SwipeObj;
```
## Implementation
Swipe is done by using state machine. All the possible states can be seen in the `swipe_state_t` enum. One iteration of this state machine can be done by calling `void Swipe_State_Machine(SwipeObj* Swipe)`. 
From a code point of view, swipe is implemented by using created `SwipeObj` structure. **To use this structure properly, special initialisation of this struct should be done by calling `void Swipe_Init(SwipeObj* Swipe);` function with passing newly create `SwipeObj` structure.**
To change time configuration `void Procces_Button (SwipeObj* Swipe);` function can be used.
To manipulate with LED `void LED_Handle(SwipeObj* Swipe);` function can be called. This function implements 1/10 of PWM signal and should be called 10 times to create full PWM signal.
## Example
In this project `SwipeObj` was created in main.c file as a global variable.  
For running everything simultaneously it was decided to use Timer 6 (TIM6) and Timer 7 (TIM7) with the interrupts. TIM6 interrupts (period 65535 ticks) is responsable for running Swipe state machine. TIM7 (period 16000 ticks) is responsable for creating PWM 70% 200Hz signal for the LD3. SYSCLK frequency is considered to be 32MHz.  
While loop in main function has 3 important tasks.
1. Calling `tsl_user_Exec();` to collect new data from linear slider
2. Calling `Procces_Button(&mySwipe);` to check, if button was pressed
3. Display information about slider current position and if time configuration was changed, display new time limit configuration.
## Optional defines
There is a number of usefull defines in swipe.h, that can be adjusted to manipulate with swipe accuracy.  
- FINAL_POS - Final slider position to initiate SWIPE_SUCCESS state. Can be any number from (half of the maximum sensor value) to (maximum sensor value). Closer this value to (maximum sensor value), harder it for the program to recognize swipe movement.
- SWIPE_RIGHT_MIN_VAL - Minimum linear slider position needed to initiate right swipe movement. Can be any number from 0 to (half of maximum sensor value).
- SWIPE_LEFT_MIN_VAL - Minimum linear slider position needed to initiate left swipe movement. Can be any number from (half of the maximum sensor value) to (maximum sensor value).
- MINIMUM_POSITIONS_NUMBER - Minimum number of different positions needed to be saved by Swipe state machine to recognize swipe movement. The higher the value, the more accurate the detected swipe movement will be.
## Time defines
This defines can be rewrited, if is used different IRQ_period (for TIM6, TIM7) and SYSCLK frequency. Basic TIM6 period is 65535, TIM7 period is 16000, SYSCLK 32 MHz.

- LED_3SEC_TIME_FRAME - Number of TIM7 interrupt cycles needed to create 200Hz 70% PWM signal for LED. Can be calculated by using this expression: 3000/(IRQ_period / SYSCLK * 1000 * 10)
- MS200_TIME_FRAME  - Number of TIM6 interrupt cycles needed to count 200 ms. Can be calculated by using this expression: 200/(IRQ_period / SYSCLK * 1000)
- MS500_TIME_FRAME  - Number of TIM6 interrupt cycles needed to count 500 ms. Can be calculated by using this expression: 500/(IRQ_period / SYSCLK * 1000)
- MS1000_TIME_FRAME - Number of TIM6 interrupt cycles needed to count 1000 ms. Can be calculated by using this expression:1000/(IRQ_period / SYSCLK * 1000)