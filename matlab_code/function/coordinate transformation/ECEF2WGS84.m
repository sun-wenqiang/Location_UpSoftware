function [lon_rad, lat_rad, h] = ECEF2WGS84(x, y, z)
% ENU2ECEF: 将 ECEF (x, y, z) 转换为 WGS84 (经度，纬度，高度)
% 输入参数：
%   x, y, z      - 目标点的ECEF坐标（单位：米）
% 输出参数：
%   lon_rad, lat_rad, h  - 目标点的经纬度和海拔(单位：弧度/米)

a = 6378137;  % 长半轴 (m)
f = 1 / 298.257223563; % 扁率
e_sq = f * (2 - f);   % 第一偏心率平方

% 计算经度
lon_rad = atan2(y, x);

% 计算投影半径
p = sqrt(x^2 + y^2);

% 迭代求解纬度
lat_rad = atan2(z, p);  % 初始值
h = 0;  % 初始海拔高度

% 迭代收敛误差阈值
tol = 1e-9;
max_iter = 10;  % 最大迭代轮数，一半迭代7轮即可达到精度
for i = 1:max_iter
    N = a / sqrt(1 - e_sq * sin(lat_rad)^2);
    h_new = p / cos(lat_rad) - N;
    lat_new = atan2(z, p * (1 - e_sq * N / (N + h_new)));

    % 检查是否收敛
    if abs(lat_new - lat_rad) < tol && abs(h_new - h) < tol
        break;
    end

    lat_rad = lat_new;
    h = h_new;
end
end
