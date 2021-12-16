/*
 * LCD_user.h
 *
 *  Created on: Dec 13, 2021
 *      Author: User
 */

#ifndef INC_LCD_USER_H_
#define INC_LCD_USER_H_

#include "stm32l1xx_hal_lcd.h"

/* Define for scrolling sentences*/
#define SCROLL_SPEED  	75
#define SCROLL_SPEED_L  600
#define SCROLL_NUM    	1

/* Define for character '.' */
#define  POINT_OFF 0
#define  POINT_ON 1

/* Define for caracter ":" */
#define  COLUMN_OFF 0
#define  COLUMN_ON 1

#define DOT 0x8000 /* for add decimal point in string */
#define DOUBLE_DOT 0x4000 /* for add decimal point in string */


/*  =========================================================================
                                 LCD MAPPING
    =========================================================================
	    A
     _  ----------
COL |_| |\   |J  /|
       F| H  |  K |B
     _  |  \ | /  |
COL |_| --G-- --M--
        |   /| \  |
       E|  Q |  N |C
     _  | /  |P  \|
DP  |_| -----------
	    D

 An LCD character coding is based on the following matrix:
      { E , D , P , N   }
      { M , C , COL , DP}
      { B , A , K , J   }
      { G , F , Q , H   }

 The character 'A' for example is:
  -------------------------------
LSB   { 1 , 0 , 0 , 0   }
      { 1 , 1 , 0 , 0   }
      { 1 , 1 , 0 , 0   }
MSB   { 1 , 1 , 0 , 0   }
      -------------------
  'A' =  F    E   0   0 hexa

*/
/* Macros used for set/reset bar LCD bar */
#define BAR0_ON  t_bar[1] |= 8
#define BAR0_OFF t_bar[1] &= ~8
#define BAR1_ON  t_bar[0] |= 8
#define BAR1_OFF t_bar[0] &= ~8
#define BAR2_ON  t_bar[1] |= 2
#define BAR2_OFF t_bar[1] &= ~2
#define BAR3_ON t_bar[0]  |= 2
#define BAR3_OFF t_bar[0] &= ~2

/* code for 'µ' character */
#define C_UMAP 0x6084

/* code for 'm' character */
#define C_mMap 0xb210

/* code for 'n' character */
#define C_nMap 0x2210

/* constant code for '*' character */
#define star 0xA0DD

/* constant code for '-' character */
#define C_minus 0xA000

/* constant code for '/' */
#define C_slatch  0x00c0

/* constant code for ° */
#define C_percent_1 0xec00

/* constant code  for small o */
#define C_percent_2 0xb300

#define C_full 0xffdd

void LCD_bar(void);
void LCD_GLASS_WriteChar(uint8_t* ch, uint8_t point, uint8_t column,uint8_t position);
void LCD_GLASS_DisplayString(LCD_HandleTypeDef *hlcd, uint8_t* ptr);
void LCD_GLASS_DisplayStrDeci(LCD_HandleTypeDef *hlcd, uint16_t* ptr);
void LCD_GLASS_Clear(LCD_HandleTypeDef *hlcd);
void ProcessSensors(LCD_HandleTypeDef *hlcd);
void ProcessSensorsButtons(LCD_HandleTypeDef *hlcd);

#endif /* INC_LCD_USER_H_ */
