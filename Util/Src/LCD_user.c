/*
 * LCD_user.c
 *
 *  Created on: Dec 13, 2021
 *      Author: User
 */

/* Includes ------------------------------------------------------------------*/

#include "stm32l1xx_hal.h"
#include "stm32l1xx_hal_lcd.h"
#include "LCD_user.h"
#include "touchsensing.h"

/* Private variables ---------------------------------------------------------*/

/* LCD BAR status: We don't write directly in LCD RAM for save the bar setting */
uint8_t t_bar[2]={0x0,0X0};
extern TSL_LinRot_T MyLinRots[];

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

/* Constant table for cap characters 'A' --> 'Z' */
const uint16_t CapLetterMap[26]=
    {
        /* A      B      C      D      E      F      G      H      I  */
        0xFE00,0x6714,0x1d00,0x4714,0x9d00,0x9c00,0x3f00,0xfa00,0x0014,
        /* J      K      L      M      N      O      P      Q      R  */
        0x5300,0x9841,0x1900,0x5a48,0x5a09,0x5f00,0xFC00,0x5F01,0xFC01,
        /* S      T      U      V      W      X      Y      Z  */
        0xAF00,0x0414,0x5b00,0x18c0,0x5a81,0x00c9,0x0058,0x05c0
    };

/* Constant table for number '0' --> '9' */
const uint16_t NumberMap[10]=
    {
        /* 0      1      2      3      4      5      6      7      8      9  */
        0x5F00,0x4200,0xF500,0x6700,0xEa00,0xAF00,0xBF00,0x04600,0xFF00,0xEF00
    };

/* Private function prototypes -----------------------------------------------*/

static void LCD_Conv_Char_Seg(uint8_t* c,uint8_t point,uint8_t column,uint8_t* digit);

/* External functions --------------------------------------------------------*/

/**
  * @brief  Converts an ascii char to the a LCD digit.
  * @param  c: a char to display.
  * @param  point: a point to add in front of char
  *         This parameter can be: POINT_OFF or POINT_ON
  * @param  column : flag indicating if a column has to be add in front
  *         of displayed character.
  *         This parameter can be: COLUMN_OFF or COLUMN_ON.
	* @param 	digit array with segment
  * @retval None
  */
static void LCD_Conv_Char_Seg(uint8_t* c, uint8_t point, uint8_t column, uint8_t* digit)
{
  uint16_t ch = 0 ;
  uint8_t i,j;

  switch (*c)
    {
    case ' ' :
      ch = 0x00;
      break;

    case '*':
      ch = star;
      break;

    case 'µ' :
      ch = C_UMAP;
      break;

    case 'm' :
      ch = C_mMap;
      break;

    case 'n' :
      ch = C_nMap;
      break;

    case '-' :
      ch = C_minus;
      break;

    case '/' :
      ch = C_slatch;
      break;

    case '°' :
      ch = C_percent_1;
      break;
    case '%' :
      ch = C_percent_2;
      break;
    case 255 :
      ch = C_full;
      break ;

    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      ch = NumberMap[*c-0x30];
      break;

    default:
      /* The character c is one letter in upper case*/
      if ( (*c < 0x5b) && (*c > 0x40) )
      {
        ch = CapLetterMap[*c-'A'];
      }
      /* The character c is one letter in lower case*/
      if ( (*c <0x7b) && ( *c> 0x60) )
      {
        ch = CapLetterMap[*c-'a'];
      }
      break;
  }

  /* Set the digital point can be displayed if the point is on */
  if (point)
  {
    ch |= 0x0002;
  }

  /* Set the "COL" segment in the character that can be displayed if the column is on */
  if (column)
  {
    ch |= 0x0020;
  }

  for (i = 12,j=0 ;j<4; i-=4,j++)
  {
    digit[j] = (ch >> i) & 0x0f; //To isolate the less signifiant dibit
  }
}

/**
  * @brief  Setting bar on LCD, writes bar value in LCD frame buffer
  * @param  None
  * @retval None
  */
void LCD_bar()
{

  LCD->RAM[LCD_RAM_REGISTER4] &= 0xffff5fff;
  LCD->RAM[LCD_RAM_REGISTER6] &= 0xffff5fff;
/* bar1 bar3 */
  LCD->RAM[LCD_RAM_REGISTER4] |= (uint32_t)(t_bar[0]<<12);

/*bar0 bar2 */
  LCD->RAM[LCD_RAM_REGISTER6] |= (uint32_t)(t_bar[1]<<12);

}

/**
  * @brief  This function writes a char in the LCD frame buffer.
  * @param  ch: the character to display.
  * @param  point: a point to add in front of char
  *         This parameter can be: POINT_OFF or POINT_ON
  * @param  column: flag indicating if a column has to be add in front
  *         of displayed character.
  *         This parameter can be: COLUMN_OFF or COLUMN_ON.
  * @param  position: position in the LCD of the caracter to write [0:7]
  * @retval None
  * @par    Required preconditions: The LCD should be cleared before to start the
  *         write operation.
  */
void LCD_GLASS_WriteChar(uint8_t* ch, uint8_t point, uint8_t column, uint8_t position)
{
  uint8_t digit[4];     /* Digit frame buffer */

/* To convert displayed character in segment in array digit */
  LCD_Conv_Char_Seg(ch,point,column,digit);


  switch (position)
  {
    /* Position 1 on LCD (Digit1)*/
    case 1:
      LCD->RAM[LCD_RAM_REGISTER0] &= 0xcffffffc;
      LCD->RAM[LCD_RAM_REGISTER2] &= 0xcffffffc;
      LCD->RAM[LCD_RAM_REGISTER4] &= 0xcffffffc;
      LCD->RAM[LCD_RAM_REGISTER6] &= 0xcffffffc;

      LCD->RAM[LCD_RAM_REGISTER0] |= ((digit[0]& 0x0c) << 26 ) | (digit[0]& 0x03) ; // 1G 1B 1M 1E
      LCD->RAM[LCD_RAM_REGISTER2] |= ((digit[1]& 0x0c) << 26 ) | (digit[1]& 0x03) ; // 1F 1A 1C 1D
      LCD->RAM[LCD_RAM_REGISTER4] |= ((digit[2]& 0x0c) << 26 ) | (digit[2]& 0x03) ; // 1Q 1K 1Col 1P
      LCD->RAM[LCD_RAM_REGISTER6] |= ((digit[3]& 0x0c) << 26 ) | (digit[3]& 0x03) ; // 1H 1J 1DP 1N

      break;

    /* Position 2 on LCD (Digit2)*/
    case 2:
      LCD->RAM[LCD_RAM_REGISTER0] &= 0xf3ffff03;
      LCD->RAM[LCD_RAM_REGISTER2] &= 0xf3ffff03;
      LCD->RAM[LCD_RAM_REGISTER4] &= 0xf3ffff03;
      LCD->RAM[LCD_RAM_REGISTER6] &= 0xf3ffff03;

      LCD->RAM[LCD_RAM_REGISTER0] |= ((digit[0]& 0x0c) << 24 )|((digit[0]& 0x02) << 6 )|((digit[0]& 0x01) << 2 ) ; // 2G 2B 2M 2E
      LCD->RAM[LCD_RAM_REGISTER2] |= ((digit[1]& 0x0c) << 24 )|((digit[1]& 0x02) << 6 )|((digit[1]& 0x01) << 2 ) ; // 2F 2A 2C 2D
      LCD->RAM[LCD_RAM_REGISTER4] |= ((digit[2]& 0x0c) << 24 )|((digit[2]& 0x02) << 6 )|((digit[2]& 0x01) << 2 ) ; // 2Q 2K 2Col 2P
      LCD->RAM[LCD_RAM_REGISTER6] |= ((digit[3]& 0x0c) << 24 )|((digit[3]& 0x02) << 6 )|((digit[3]& 0x01) << 2 ) ; // 2H 2J 2DP 2N

      break;

    /* Position 3 on LCD (Digit3)*/
    case 3:
      LCD->RAM[LCD_RAM_REGISTER0] &= 0xfcfffcff;
      LCD->RAM[LCD_RAM_REGISTER2] &= 0xfcfffcff;
      LCD->RAM[LCD_RAM_REGISTER4] &= 0xfcfffcff;
      LCD->RAM[LCD_RAM_REGISTER6] &= 0xfcfffcff;

      LCD->RAM[LCD_RAM_REGISTER0] |= ((digit[0]& 0x0c) << 22 ) | ((digit[0]& 0x03) << 8 ) ; // 3G 3B 3M 3E
      LCD->RAM[LCD_RAM_REGISTER2] |= ((digit[1]& 0x0c) << 22 ) | ((digit[1]& 0x03) << 8 ) ; // 3F 3A 3C 3D
      LCD->RAM[LCD_RAM_REGISTER4] |= ((digit[2]& 0x0c) << 22 ) | ((digit[2]& 0x03) << 8 ) ; // 3Q 3K 3Col 3P
      LCD->RAM[LCD_RAM_REGISTER6] |= ((digit[3]& 0x0c) << 22 ) | ((digit[3]& 0x03) << 8 ) ; // 3H 3J 3DP 3N

      break;

    /* Position 4 on LCD (Digit4)*/
    case 4:
      LCD->RAM[LCD_RAM_REGISTER0] &= 0xffcff3ff;
      LCD->RAM[LCD_RAM_REGISTER2] &= 0xffcff3ff;
      LCD->RAM[LCD_RAM_REGISTER4] &= 0xffcff3ff;
      LCD->RAM[LCD_RAM_REGISTER6] &= 0xffcff3ff;

      LCD->RAM[LCD_RAM_REGISTER0] |= ((digit[0]& 0x0c) << 18 ) | ((digit[0]& 0x03) << 10 ) ; // 4G 4B 4M 4E
      LCD->RAM[LCD_RAM_REGISTER2] |= ((digit[1]& 0x0c) << 18 ) | ((digit[1]& 0x03) << 10 ) ; // 4F 4A 4C 4D
      LCD->RAM[LCD_RAM_REGISTER4] |= ((digit[2]& 0x0c) << 18 ) | ((digit[2]& 0x03) << 10 ) ; // 4Q 4K 4Col 4P
      LCD->RAM[LCD_RAM_REGISTER6] |= ((digit[3]& 0x0c) << 18 ) | ((digit[3]& 0x03) << 10 ) ; // 4H 4J 4DP 4N

      break;

    /* Position 5 on LCD (Digit5)*/
    case 5:
      LCD->RAM[LCD_RAM_REGISTER0] &= 0xfff3cfff;
      LCD->RAM[LCD_RAM_REGISTER2] &= 0xfff3cfff;
      LCD->RAM[LCD_RAM_REGISTER4] &= 0xfff3efff;
      LCD->RAM[LCD_RAM_REGISTER6] &= 0xfff3efff;

      LCD->RAM[LCD_RAM_REGISTER0] |= ((digit[0]& 0x0c) << 16 ) | ((digit[0]& 0x03) << 12 ) ; // 5G 5B 5M 5E
      LCD->RAM[LCD_RAM_REGISTER2] |= ((digit[1]& 0x0c) << 16 ) | ((digit[1]& 0x03) << 12 ) ; // 5F 5A 5C 5D
      LCD->RAM[LCD_RAM_REGISTER4] |= ((digit[2]& 0x0c) << 16 ) | ((digit[2]& 0x01) << 12 ) ; // 5Q 5K   5P
      LCD->RAM[LCD_RAM_REGISTER6] |= ((digit[3]& 0x0c) << 16 ) | ((digit[3]& 0x01) << 12 ) ; // 5H 5J   5N

      break;

    /* Position 6 on LCD (Digit6)*/
    case 6:
      LCD->RAM[LCD_RAM_REGISTER0] &= 0xfffc3fff;
      LCD->RAM[LCD_RAM_REGISTER2] &= 0xfffc3fff;
      LCD->RAM[LCD_RAM_REGISTER4] &= 0xfffc3fff;
      LCD->RAM[LCD_RAM_REGISTER6] &= 0xfffc3fff;

      LCD->RAM[LCD_RAM_REGISTER0] |= ((digit[0]& 0x04) << 15 ) | ((digit[0]& 0x08) << 13 ) | ((digit[0]& 0x03) << 14 ) ; // 6B 6G 6M 6E
      LCD->RAM[LCD_RAM_REGISTER2] |= ((digit[1]& 0x04) << 15 ) | ((digit[1]& 0x08) << 13 ) | ((digit[1]& 0x03) << 14 ) ; // 6A 6F 6C 6D
      LCD->RAM[LCD_RAM_REGISTER4] |= ((digit[2]& 0x04) << 15 ) | ((digit[2]& 0x08) << 13 ) | ((digit[2]& 0x01) << 14 ) ; // 6K 6Q    6P
      LCD->RAM[LCD_RAM_REGISTER6] |= ((digit[3]& 0x04) << 15 ) | ((digit[3]& 0x08) << 13 ) | ((digit[3]& 0x01) << 14 ) ; // 6J 6H   6N

      break;

     default:
      break;
  }

/* Refresh LCD  bar */
  LCD_bar();

}

/**
  * @brief  This function writes a char in the LCD RAM.
  * @param  hlcd: Instance of the LCD
  * @param  ptr: Pointer to string to display on the LCD Glass.
  * @retval None
  */
void LCD_GLASS_DisplayString(LCD_HandleTypeDef *hlcd, uint8_t* ptr)
{
  uint8_t i = 0x01;

	/* wait for LCD Ready */
  while( __HAL_LCD_GET_FLAG(hlcd, LCD_FLAG_UDR) != RESET) ;

  /* Send the string character by character on lCD */
  while ((*ptr != 0) & (i < 8))
  {
    /* Display one character on LCD */
    LCD_GLASS_WriteChar(ptr, 0, 0, i);

    /* Point on the next character */
    ptr++;

    /* Increment the character counter */
    i++;
  }

	/* Update the LCD display */
  HAL_LCD_UpdateDisplayRequest(hlcd);
}

/**
  * @brief  This function writes a char in the LCD RAM.
  * @param  ptr: Pointer to string to display on the LCD Glass.
  * @retval None
  * @par    Required preconditions: Char is ASCCI value "Ored" with decimal point or Column flag
  */
void LCD_GLASS_DisplayStrDeci(LCD_HandleTypeDef *hlcd, uint16_t* ptr)
{
  uint8_t i = 0x01;
  uint8_t char_tmp;

	/* TO wait LCD Ready */
  while(__HAL_LCD_GET_FLAG(hlcd, LCD_FLAG_UDR) != RESET) ;

  /* Send the string character by character on lCD */
  while ((*ptr != 0) & (i < 8))
  {
    char_tmp = (*ptr) & 0x00ff;

    switch ((*ptr) & 0xf000)
    {
      case DOT:
          /* Display one character on LCD with decimal point */
          LCD_GLASS_WriteChar(&char_tmp, POINT_ON, COLUMN_OFF, i);
          break;
      case DOUBLE_DOT:
          /* Display one character on LCD with decimal point */
          LCD_GLASS_WriteChar(&char_tmp, POINT_OFF, COLUMN_ON, i);
          break;
      default:
          LCD_GLASS_WriteChar(&char_tmp, POINT_OFF, COLUMN_OFF, i);
          break;
    }/* Point on the next character */
    ptr++;

    /* Increment the character counter */
    i++;
  }
	/* Update the LCD display */
  HAL_LCD_UpdateDisplayRequest(hlcd);
}

/**
  * @brief  This function Clear the whole LCD RAM.
  * @param  None
  * @retval None
  */
void LCD_GLASS_Clear(LCD_HandleTypeDef *hlcd)
{
  uint32_t counter = 0;

  /* TO wait LCD Ready */
  while( __HAL_LCD_GET_FLAG(hlcd,LCD_FLAG_UDR) != RESET) ;

  for (counter = LCD_RAM_REGISTER0; counter <= LCD_RAM_REGISTER15; counter++)
  {
    LCD->RAM[counter] = 0;
  }

  /* Update the LCD display */
  HAL_LCD_UpdateDisplayRequest(hlcd);

}

/**
  * @brief converts a 32bit unsined int into ASCII
  * @caller several callers for display values
  * @param Number digit to displays
  *  p_tab values in array in ASCII
  * @retval None
  */
static void convert_into_char(uint32_t number, uint16_t *p_tab)
{
  uint16_t units=0, tens=0, hundreds=0, thousands=0, misc=0;

  units = (((number%10000)%1000)%100)%10;
  tens = ((((number-units)/10)%1000)%100)%10;
  hundreds = (((number-tens-units)/100))%100%10;
  thousands = ((number-hundreds-tens-units)/1000)%10;
  misc = ((number-thousands-hundreds-tens-units)/10000);

  *(p_tab+4) = units + 0x30;
  *(p_tab+3) = tens + 0x30;
  *(p_tab+2) = hundreds + 0x30;
  *(p_tab+1) = thousands + 0x30;
  *(p_tab) = misc + 0x30;

}

/**
  * @brief  Manage the activity on sensors when touched/released (example)
  * @param  hlcd: Currently used hlcd device
  * @retval None
  */
void ProcessSensors(LCD_HandleTypeDef *hlcd)
{
  uint16_t Max_Value = 256/(9-TSLPRM_LINROT_RESOLUTION);

  uint16_t Message[6];
  uint32_t percent_value;
  Message[0] = ' ';
  Message[1] = ' ';
  Message[2] = ' ';
  /*Add "%" in message*/
  Message[3] = '°' ;
  Message[4] = '/' ;
  Message[5] = '%' ;

  /* get Slider position and convert it in percent*/
  percent_value = MyLinRots[0].p_Data->Position ;
  percent_value *= 10000;
  percent_value /= Max_Value;
  /*Convert percent value in char and store it in message*/
  convert_into_char(percent_value,Message);
  /*Add "%" in message*/
  Message[3] = '°' ;
  Message[4] = '/' ;
  Message[5] = '%' ;

  /*Display message*/
  LCD_GLASS_DisplayStrDeci(hlcd, Message);
}

/**
  * @brief  Manage the activity on sensors when touched/released (example)
  * @param  hlcd: Currently used hlcd device
  * @retval None
  */
void ProcessSensorsButtons(LCD_HandleTypeDef *hlcd)
{
  uint8_t Message[7];

    /* Prepare Message to display*/
    Message[0] = '0';
    Message[1] = '0';
    Message[2] = '0';
    Message[3] = '0';
    Message[4] = '0';
    Message[5] = '0';

	#ifdef STM32L1XX_MDP // for medium density plus device
		if( MyLinRots[0].p_Data->Position > 118 )
				Message[4] = 255;
			else if( MyLinRots[0].p_Data->Position > 80 )
				Message[3] = 255;
			else if( MyLinRots[0].p_Data->Position > 5 )
				Message[2] = 255;
			else
				Message[1] = 255;
	#else // for medium density device
		if( MyLinRots[0].p_Data->Position < 127 )
		{
			if( MyLinRots[0].p_Data->Position > 110 )
				Message[5] = 255;
			else if( MyLinRots[0].p_Data->Position > 75 )
				Message[4] = 255;
			else if( MyLinRots[0].p_Data->Position > 50 )
				Message[3] = 255;
			else if( MyLinRots[0].p_Data->Position > 25 )
				Message[2] = 255;
			else if( MyLinRots[0].p_Data->Position > 10 )
				Message[1] = 255;
			else
				Message[0] = 255;
		}
	#endif

    LCD_GLASS_DisplayString(hlcd, Message);

}
