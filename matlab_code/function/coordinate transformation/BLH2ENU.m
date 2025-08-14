function ENU = BLH2ENU(BLH)
    % 将WGS84坐标(经度、纬度、海拔)转换为东北天坐标(East, North, Up，也就是NEZ坐标系，但是NE的顺序调换了)
    % 将第一个坐标作为参考节点，返回以这个点为原点的ENU坐标
    % 输入参数:
    %          BLH，n×3的矩阵，一行为一个目标点的经度、纬度（弧度制）和海拔，其中经纬度必须已经转化为弧度制，第一行的经纬度将作为参考节点
    %               |L1     B1     H1|
    %               |L2     B2     H2|
    %          BLH= |................|
    %               |Ln-1  Bn-1  Hn-1|
    %               |Ln     Bn     Hn|
    % 输出参数：
    %          ENU，n×3的矩阵，一行为一个目标点相对于参考点的东、北、天坐标（米），X轴指向正东，Y轴指向正北，Z轴向上
    %               |E1     N1     U1| -> [0 0 0]
    %               |E2     N2     U2|
    %          ENU= |................|
    %               |En-1  Nn-1  Un-1|
    %               |En     Nn     Un|
    ENU = zeros(size(BLH));
    lon0 = BLH(1,1);
    lat0 = BLH(1,2); 
    h0 = BLH(1,3); 
    for i=1:size(BLH,1)
        [x, y, z] = WGS842ECEF(BLH(i,1), BLH(i,2), BLH(i,3));% 第一步: 将目标点(WGS84)转换为地心地固坐标系(ECEF)
        [ENU(i,1), ENU(i,2), ENU(i,3)] = ECEF2ENU(x, y, z, lon0, lat0, h0);% 第二步: 将地心地固坐标系(ECEF)转换为东北天坐标系(ENU)
    end
end

