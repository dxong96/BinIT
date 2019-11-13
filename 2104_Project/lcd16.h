/*
 * lcd16.h
 *
 *  Created on: 13 Sep 2019
 *      Author: markpang
 */

#ifndef MSP_EXP432P401R_LCD16X2_LCD16_H_
#define MSP_EXP432P401R_LCD16X2_LCD16_H_



//P4.5
#define  RS BIT5
//P6.0
//#define  RW BIT0
//P4.3
#define  EN BIT3

void waitlcd(unsigned int x);

void lcdinit(void);
void integerToLcd(unsigned int integer );
void lcdData(unsigned char l);
void prints(char *s);
void gotoXy(unsigned char  x,unsigned char y);

#endif /* MSP_EXP430F5529LP_LCD16X2_LCD16_H_ */
