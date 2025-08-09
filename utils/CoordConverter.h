#ifndef COORDCONVERTER_H
#define COORDCONVERTER_H
#include <iostream>
#include <cmath>

/* WGS84椭球体参数 */
const constexpr  double WGS84_A = 6378137.0;                       // 长半轴（赤道半径，单位：米）
const constexpr  double WGS84_F = 1 / 298.257223563;               // 扁率
const constexpr  double WGS84_E_SQ = (WGS84_F * (2 - WGS84_F));    // 第一偏心率平方

/**
 * @struct EcefCoord
 * @brief 地心地固坐标系(ECEF)结构体
 */
struct GeoCoord {
    double latitude;   // 纬度
    double longitude;  // 经度
    double altitude;   // 海拔
};

/**
 * @struct GeoCoord
 * @brief 地理坐标系结构体
 */
struct EcefCoord
{
    double x;
    double y;
    double z;
};

/**
 * @struct EnuCoord
 * @brief 局部东北天坐标系结构体
 */
struct EnuCoord
{
    double x;
    double y; 
    double z;
};


double deg2rad(double deg);
double rad2deg(double rad);

EnuCoord Geo2Enu(GeoCoord geo, GeoCoord ref);  // WGS84转化为局部东北天坐标，需要以ref为参考
EcefCoord Geo2Ecef(GeoCoord geo);   // WGS84转化为地心地固坐标系
EnuCoord Ecef2Enu(EcefCoord ecef, GeoCoord ref);  // ECEF转化为局部的直角坐标系，需要以ref节点为参考

GeoCoord Enu2Geo(EnuCoord enu, GeoCoord ref);  // 将局部东北天坐标转化为WGS84，需要以ref为参考
EcefCoord Enu2Ecef(EnuCoord enu, GeoCoord ref);  // 将局部的东北天坐标转化为ECEF坐标，需要以ref为参考
GeoCoord Ecef2Geo(EcefCoord ecef);    // 将ECEF坐标转化为经纬度

#endif