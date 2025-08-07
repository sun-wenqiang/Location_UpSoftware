#ifndef COORDCONVERTER_H
#define COORDCONVERTER_H
#include <iostream>
#include <cmath>

#define WGS84_A         6378137.0       // 长半轴（赤道半径，单位：米）
#define WGS84_B         6356752.3142    // 短半轴（极半径，单位：米）
#define WGS84_F         ((WGS84_A - WGS84_B) / WGS84_A)   // 扁率
#define WGS84_E_SQ      (WGS84_F * (2 - WGS84_F))         // 第一偏心率平方


class CoordConverter
{
public:
    CoordConverter(void);
    ~CoordConverter(void);

    double deg2rad(double deg) const;   // const表示这个函数不会修改类成员
    double rad2deg(double rad) const;





};

#endif