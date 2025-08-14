function [lon_rad, lat_rad, h] = ENU2WGS84(East, North, Up, lon0_rad, lat0_rad, h0)
% ENU2ECEF: 将东北天坐标系 (ENU) 转换为WGS84坐标系
% 输入参数：
%   East, North, Up - 目标点相对于参考点的 ENU 坐标（单位：米）
%   lon0_rad     - 参考点的经度（单位：弧度）
%   lat0_rad     - 参考点的纬度（单位：弧度）
%   h0           - 参考点的海拔高度（单位：米）
%
% 输出参数：
%   lon_rad, lat_rad, h  - 目标点的经纬度和海拔(单位：弧度/米)
[x, y, z] = ENU2ECEF(East, North, Up, lon0_rad, lat0_rad, h0);   % 先将东北天转化为ECEF坐标
[lon_rad, lat_rad, h] = ECEF2WGS84(x, y, z);   % 将ECEF坐标转化为WGS84坐标

