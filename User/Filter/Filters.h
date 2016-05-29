#ifndef __FILTERS_H
#define __FILTERS_H
#include "stm32f10x.h"
#include "stdbool.h"
#include <math.h>
#include "stdint.h"
#include "stdio.h"


double Moving_Average_Filter(double newValue, double* filteValue, double* movingWindow, uint16_t windowLength, uint16_t* currentCount);
double Weight_Moving_Average_Filter(double newValue, double* filteValue, double valueTrust, double* movingWindow, uint16_t windowLength, uint16_t* currentCount);
int LinearRegression(double *data, int rows, double *a, double *b, double *SquarePoor);
#endif
