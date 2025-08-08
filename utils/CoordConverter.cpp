#include "CoordConverter.h"


double deg2rad(double deg)
{
    return deg / 180 * M_PI;
}

double rad2deg(double rad)
{
    return rad / M_PI * 180;
}


/**
 * @brief 将地理坐标(经纬高)转换为地心地固坐标(ECEF)
 * @param geo 输入地理坐标(纬度/经度为弧度单位)
 * @return 对应的ECEF坐标
 * 
 * @warning 输入经纬度必须为弧度值，角度需先转换为弧度
 * 
 * @copyright Copyright (c) 2025, Sun Wenqiang. All rights reserved.
 */
EcefCoord Geo2Ecef(GeoCoord geo)
{
    EcefCoord ans;
    double N = WGS84_A / sqrt(1 - WGS84_E_SQ * std::pow(sin(geo.latitude), 2));
    ans.x = (N + geo.altitude) * cos(geo.latitude) * cos(geo.longitude);
    ans.y = (N + geo.altitude) * cos(geo.latitude) * sin(geo.longitude);
    ans.z = (N * (1-WGS84_E_SQ) + geo.altitude) * sin(geo.longitude);
    return ans;
}


/**
 * @brief 将地心地固坐标(ECEF)转换为东北天坐标(ENU)
 * @param ecef 输入ECEF坐标
 * @param ref 参考点地理坐标(纬度/经度为弧度单位)
 * @return 对应的ENU坐标
 * 
 * @warning 
 *   - 输入参考点经纬度必须为弧度值
 *   - 参考点高度(ref.altitude)会影响ENU坐标原点
 * 
 * @copyright Copyright (c) 2025, Sun Wenqiang. All rights reserved.
 */
EnuCoord Ecef2Enu(EcefCoord ecef, GeoCoord ref)
{
    EnuCoord ans;
    EcefCoord temp = Geo2Ecef(ref);

    double dx = ecef.x - temp.x;
    double dy = ecef.y - temp.y;
    double dz = ecef.z - temp.z;

    double sin_lat0 = sin(ref.latitude);
    double cos_lat0 = cos(ref.latitude);
    double sin_lon0 = sin(ref.longitude);
    double cos_lon0 = cos(ref.longitude);

    double t = -cos_lat0 * dx - sin_lon0 * dy;

    ans.x = -sin_lon0 * dx + cos_lon0 * dy;
    ans.y = t * sin_lat0 + cos_lat0 * dz;
    ans.z = cos_lat0 * cos_lon0 * dx + cos_lat0 * sin_lon0 * dy + sin_lat0 * dz;

    return ans;
}

/**
 * @brief 将地理坐标(经纬高)转换为东北天坐标(ENU)
 * @param geo 待转换的地理坐标
 * @param ref ENU坐标系参考点(纬度/经度为弧度单位)
 * @return 对应的ENU坐标
 * @copyright Copyright (c) 2025, Sun Wenqiang. All rights reserved.
 */
EnuCoord Geo2Enu(GeoCoord geo, GeoCoord ref)
{
    EcefCoord temp = Geo2Ecef(geo);
    EnuCoord ans = Ecef2Enu(temp, ref);

    return ans;
}

/**
 * @brief 将东北天坐标(ENU)转换为地心地固坐标(ECEF)
 * @param enu 输入的ENU坐标
 * @param ref 参考点地理坐标(纬度/经度为弧度单位)
 * @return 对应的ECEF坐标
 * @copyright Copyright (c) 2025, Sun Wenqiang. All rights reserved.
 */
EcefCoord Enu2Ecef(EnuCoord enu, GeoCoord ref)
{
    EcefCoord ans;
    EcefCoord temp = Geo2Ecef(ref);

    double sin_lat0 = sin(ref.latitude);
    double cos_lat0 = cos(ref.latitude);
    double sin_lon0 = sin(ref.longitude);
    double cos_lon0 = cos(ref.longitude);

    // M 转置
    double t00 = -sin_lon0;
    double t01 = -cos_lon0 * sin_lat0;
    double t02 =  cos_lat0 * cos_lon0;

    double t10 =  cos_lon0;
    double t11 = -sin_lon0 * sin_lat0;
    double t12 =  cos_lat0 * sin_lon0;

    double t20 = 0.0;
    double t21 =  cos_lat0;
    double t22 =  sin_lat0;

    ans.x = t00 * enu.x + t01 * enu.y + t02 * enu.z;
    ans.y = t10 * enu.x + t11 * enu.y + t12 * enu.z;
    ans.z = t20 * enu.x + t21 * enu.y + t22 * enu.z;

    return ans;
}

/**
 * @brief 将地心地固坐标(ECEF)转换为地理坐标(经纬高)
 * @param ecef 输入的ECEF坐标
 * @return 对应的地理坐标(纬度/经度为弧度单位)
 * @copyright Copyright (c) 2025, Sun Wenqiang. All rights reserved.
 */
GeoCoord Ecef2Geo(EcefCoord ecef)
{
    GeoCoord ans;
    double lon_rad = atan2(ecef.y, ecef.x);   // 计算经度
    double p = sqrt(std::pow(ecef.x, 2) + std::pow(ecef.y, 2));  // 计算投影半径

    // 迭代求解纬度
    double lat_rad = atan2(ecef.z, p);   // 初始值
    double h = 0;   // 初始海拔高度

    double tol = 1e-9;
    int max_iter = 10;   // 最大迭代轮数，一半迭代7轮即可达到精度

    double N;
    double h_new;
    double lat_new;
    for (int i = 0; i < max_iter; i++)
    {
        N = WGS84_A / sqrt(1 - WGS84_E_SQ * std::pow(sin(lat_rad), 2));
        h_new = p / cos(lat_rad) - N;
        lat_new = atan2(ecef.z, p * (1 - WGS84_E_SQ * N / (N + h_new)));

        if (abs(lat_new - lat_rad) < tol && abs(h_new - h) < tol)
        {
            break;
        }

        lat_rad = lat_new;
        h = h_new;
    }

    ans.latitude = lat_rad;
    ans.longitude = lon_rad;
    ans.altitude = h;
    return ans;
}

/**
 * @brief 将东北天坐标(ENU)转换为地理坐标(经纬高)
 * @param enu 输入的ENU坐标
 * @param ref 参考点地理坐标(纬度/经度为弧度单位)
 * @return 对应的地理坐标(纬度/经度为弧度单位)
 * @copyright Copyright (c) 2025, Sun Wenqiang. All rights reserved.
 */
GeoCoord Enu2Geo(EnuCoord enu, GeoCoord ref)
{
    GeoCoord ans;
    EcefCoord temp = Enu2Ecef(enu, ref);
    ans = Ecef2Geo(temp);
    return ans;
}