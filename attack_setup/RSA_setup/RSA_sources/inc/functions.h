/*
 * fonctions.h



 *
 *  Created on: 23 mars 2018
 *      Author: gemalto
 */
#include <stdint.h>
//#include "xgpio.h"
#ifndef SHAREFILE_INCLUDED
#define SHAREFILE_INCLUDED

#ifdef  MAIN_FILE
int v_base_addr;
#else
extern int v_base_addr;
#endif
#endif

void Write_Register(int adress,int data);
int Read_Register(int adress);
void Modify_Register(int adress,int newdata, int cleardata);
uint32_t convertData32(int startIndex, uint8_t *data);

uint32_t hex2int(char *hex);
uint32_t int2int(char *intchar);
uint32_t float2float(char *floatchar);
void removeChar(char *str, char garbage);
int hammingDistance(int x, int y);
int hammingWeight(int n);
int GetInt(void);
int power(int x, unsigned int y);
char* GetString(void);
//void CheckGPIOStatus(XGpio input, XGpio output);
//void InitializeGPIOStatus(XGpio input, XGpio output);
