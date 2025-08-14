function [East, North, Up] = ECEF2ENU(x, y, z, lon0_rad, lat0_rad, h0)
    % ecef2enu: 将地心地固坐标系 (ECEF) 转换为东北天坐标系 (ENU)
    % 输入参数：
    %   x, y, z      - 目标点在地心地固坐标系下的三维坐标（单位：米）
    %   lon0_rad     - 参考点的经度（单位：弧度）
    %   lat0_rad     - 参考点的纬度（单位：弧度）
    %   h0           - 参考点的海拔高度（单位：米）
    %
    % 输出参数：
    %   East, North, Up - 目标点相对于参考点的 ENU 坐标（单位：米）
    
    % 计算参考点的地心地固坐标
    [x0, y0, z0] = WGS842ECEF(lon0_rad, lat0_rad, h0);

    % 计算差值向量
    xd = x - x0;
    yd = y - y0;
    zd = z - z0;

    % 计算旋转矩阵
    sin_lat0 = sin(lat0_rad);
    cos_lat0 = cos(lat0_rad);
    sin_lon0 = sin(lon0_rad);
    cos_lon0 = cos(lon0_rad);

    % 东北天坐标的转换
    t = -cos_lon0 * xd - sin_lon0 * yd;
    East = -sin_lon0 * xd + cos_lon0 * yd;
    North = t * sin_lat0 + cos_lat0 * zd;
    Up = cos_lat0 * cos_lon0 * xd + cos_lat0 * sin_lon0 * yd + sin_lat0 * zd;
end