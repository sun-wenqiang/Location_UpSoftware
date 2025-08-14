1.BLH2ENU.m、ECEF2ENU.m、ECEF2WGS84.m、ENU2ECEF.m、ENU2WGS84.m和WGS842ECEF.m均为坐标变换的函数。
2.BLH2ENU.m：以输入矩阵 BLH 的第一行为参考点，将一组 WGS84 坐标（经度、纬度、海拔）转换为以该参考点为原点的东北天 (ENU) 坐标系。其中会调用WGS842ECEF.m和ECEF2ENU.m。
3.ECEF2ENU.m：将地心地固坐标系 (ECEF) 下的三维坐标转换为以给定参考点为原点的东北天 (ENU) 坐标系。
4.ECEF2WGS84.m：将地心地固坐标系 (ECEF) 下的三维坐标 (x, y, z) 转换为 WGS84 坐标系中的经度、纬度和高度。
5.ENU2ECEF.m：将东北天坐标系（ENU）转化为地心地固坐标系（ECEF）。
6.ENU2WGS84.m： 将东北天 (ENU) 坐标转换为 WGS84 坐标（经度、纬度、高度）。其中会掉用ENU2ECEF.m和ECEF2WGS84.m。
7.WGS842ECEF.m：将 WGS84 坐标（经度、纬度、高度）转换为 ECEF 坐标（x, y, z）。
