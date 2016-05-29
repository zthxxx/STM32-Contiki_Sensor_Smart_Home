#include "Filters.h"


double Moving_Average_Filter(double newValue, double* filteValue, double* movingWindow, uint16_t windowLength, uint16_t* currentCount)
{
    *filteValue += (newValue - movingWindow[*currentCount]) / (float)windowLength;
    movingWindow[(*currentCount)++] = newValue;
    *currentCount = *currentCount < windowLength ? *currentCount : 0;
    return *filteValue;
}

double Weight_Moving_Average_Filter(double newValue, double* filteValue, double valueTrust, double* movingWindow, uint16_t windowLength, uint16_t* currentCount)
{
    newValue = newValue * valueTrust + movingWindow[((*currentCount) ? (*currentCount) : windowLength)-1] * (1 - valueTrust);
    *filteValue += (newValue - movingWindow[*currentCount]) / (float)windowLength;
    movingWindow[(*currentCount)++] = newValue;
    *currentCount = *currentCount < windowLength ? *currentCount : 0;
    return *filteValue;
}

// 求线性回归方程：Y = ax + b
// dada[rows*2]数组：X, Y；rows：数据行数；a, b：返回回归系数
// SquarePoor[4]：返回方差分析指标: 回归平方和，剩余平方和，回归平方差，剩余平方差
// 返回值：1求解成功，0错误
int LinearRegression(double *data, int rows, double *a, double *b, double *SquarePoor)
{
    int m;
    double *p, Lxx = 0.0, Lxy = 0.0, xa = 0.0, ya = 0.0;
    if (data == 0 || a == 0 || b == 0 || rows < 1)
        return 0;
    for (p = data, m = 0; m < rows; m ++)
    {
        xa += *p ++;
        ya += *p ++;
    }
    xa /= rows;                                     // X平均值
    ya /= rows;                                     // Y平均值
    for (p = data, m = 0; m < rows; m ++, p += 2)
    {
        Lxx += ((*p - xa) * (*p - xa));             // Lxx = Sum((X - Xa)平方)
        Lxy += ((*p - xa) * (*(p + 1) - ya));       // Lxy = Sum((X - Xa)(Y - Ya))
    }
    *a = Lxy / Lxx;                                 // b = Lxy / Lxx
    *b = ya - *a * xa;                              // a = Ya - b*Xa
    if (SquarePoor == 0)
        return 1;
    // 方差分析
    SquarePoor[0] = SquarePoor[1] = 0.0;
    for (p = data, m = 0; m < rows; m ++, p ++)
    {
        Lxy = *b + *a * *p ++;
        SquarePoor[0] += ((Lxy - ya) * (Lxy - ya)); // U(回归平方和)
        SquarePoor[1] += ((*p - Lxy) * (*p - Lxy)); // Q(剩余平方和)
    }
    SquarePoor[2] = SquarePoor[0];                  // 回归方差
    SquarePoor[3] = SquarePoor[1] / (rows - 2);     // 剩余方差
    return 1;
}

