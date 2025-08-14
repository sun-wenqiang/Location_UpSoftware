function [x, y, z] = WGS842ECEF(lon_rad, lat_rad, h)  % 经度、纬度（弧度制）、海拔
    % WGS842ECEF: 将 WGS84 坐标系下的经纬度和海拔高度转换为地心地固坐标系 (ECEF)
    % 输入参数：
    %   lon_rad - 经度，单位为弧度
    %   lat_rad - 纬度，单位为弧度
    %   h       - 海拔高度，单位为米
    %
    % 输出参数：
    %   x, y, z - 在地心地固坐标系 (ECEF) 下的三维坐标，单位为米
    
    % WGS84椭球体参数
    a = 6378137; % 长半轴 (米)
    f = 1 / 298.257223563; % 扁率
    e_sq = f * (2 - f); % 偏心率平方

    % 计算卯酉圈曲率半径 N
    N = a / sqrt(1 - e_sq * sin(lat_rad)^2);

    % 计算地心地固坐标
    x = (N + h) * cos(lat_rad) * cos(lon_rad);
    y = (N + h) * cos(lat_rad) * sin(lon_rad);
    z = (N * (1 - e_sq) + h) * sin(lat_rad);
end