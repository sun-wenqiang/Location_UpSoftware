function [x, y, z] = ENU2ECEF(East, North, Up, lon0_rad, lat0_rad, h0)
    % ENU2ECEF: 将东北天坐标系 (ENU) 转换为ECEF坐标系
    % 输入参数：
    %   East, North, Up - 目标点相对于参考点的 ENU 坐标（单位：米）
    %   lon0_rad     - 参考点的经度（单位：弧度）
    %   lat0_rad     - 参考点的纬度（单位：弧度）
    %   h0           - 参考点的海拔高度（单位：米）
    %
    % 输出参数：
    %   x, y, z      - 目标点在地心地固坐标系下的三维坐标（单位：米）
        
    % 计算参考点的地心地固坐标
    [x0, y0, z0] = WGS842ECEF(lon0_rad, lat0_rad, h0);

    % 计算旋转矩阵
    sin_lat0 = sin(lat0_rad);
    cos_lat0 = cos(lat0_rad);
    sin_lon0 = sin(lon0_rad);
    cos_lon0 = cos(lon0_rad);

    M = [-sin_lon0,         cos_lon0,           0;
        -cos_lon0*sin_lat0, -sin_lon0*sin_lat0, cos_lat0;
        cos_lat0*cos_lon0   cos_lat0*sin_lon0   sin_lat0];
    xyzd = M\[East, North, Up]';

    % 计算差值向量
    x = xyzd(1) + x0;
    y = xyzd(2) + y0;
    z = xyzd(3) + z0;
end