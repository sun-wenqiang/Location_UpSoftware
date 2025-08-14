function X = LS(mic_loc, delay, c)
%输入：
%     mic_loc：n×2的矩阵，n为节点数量，第一列为节点横坐标，第二列为节点纵坐标
%     delay：n维行向量，第delay(i)为节点i相对于参考节点的时延
%     c：水下声速
% 输出：
%     X为行向量，分别为横纵坐标
if length(delay) == 3   % 三个节点，新增部分
    compute_Kx = @(index) sum(mic_loc(index, :).^2);
    compute_Rx1 = @(i) delay(i) * c;
    compute_Xx1 = @(i) mic_loc(i,1) - mic_loc(1,1);
    compute_Yx1 = @(i) mic_loc(i,2) - mic_loc(1,2);
    R21 = compute_Rx1(2);
    R31 = compute_Rx1(3);
    Y31 = compute_Yx1(3);
    Y21 = compute_Yx1(2);
    X31 = compute_Xx1(3);
    X21 = compute_Xx1(2);
    K1 = compute_Kx(1);
    K2 = compute_Kx(2);
    K3 = compute_Kx(3);
    X1 = mic_loc(1,1);
    Y1 = mic_loc(1,2);
    q1= (R21*Y31-R31*Y21)/(X31*Y21-X21*Y31);
    p1 = ((K1-K2)*Y31+(K3-K1)*Y21+R21^2*Y31-R31^2*Y21)/(2*(X31*Y21-X21*Y31));
    q2 = (R21*X31-R31*X21)/(Y31*X21-Y21*X31);
    p2 = ((K1-K2)*X31+(K3-K1)*X21+R21^2*X31-R31^2*X21)/(2*(Y31*X21-Y21*X31));
    % 解方程组求R1
    a = q1^2+q2^2-1;
    b = 2*(q1*p1+q2*p2-X1*q1-Y1*q2);
    cc = p1^2+p2^2+K1^2-2*X1*p1-2*Y1*p2;
    R1 = (-b-sqrt(b^2-4*a*cc))/(2*a);
    x = q1*R1+p1;
    y = q2*R1+p2;
    X = [x y];
elseif length(delay) < 3  % 三节点以下，无法解算
    X = [];
else   % 四节点及以上，这部分代码跟之前一样
    A = -(mic_loc(3:end, 1) - mic_loc(1, 1)) * 2 / c ./ delay(1, 3:end)' + (mic_loc(2, 1) - mic_loc(1, 1)) * 2 / c / delay(1, 2);
    B = -(mic_loc(3:end, 2) - mic_loc(1, 2)) * 2 / c ./ delay(1, 3:end)' + (mic_loc(2, 2) - mic_loc(1, 2)) * 2 / c / delay(1, 2);

    D1 = c * (delay(1, 2) - delay(1, 3:end))';
    D2 = mic_loc(1, 1)^2 + mic_loc(1, 2)^2 - mic_loc(3:end, 1).^2 - mic_loc(3:end, 2).^2;
    D3 = mic_loc(1, 1)^2 + mic_loc(1, 2)^2 - mic_loc(2, 1)^2 - mic_loc(2, 2)^2;

    D = -D2 / c ./ delay(1, 3:end)' + D3 / c / delay(1, 2) + D1;

    AA = [A, B];
    hasInf = any(isinf(AA(:)));
    if hasInf
        delay = delay+eps;
        A = -(mic_loc(3:end, 1) - mic_loc(1, 1)) * 2 / c ./ delay(1, 3:end)' + (mic_loc(2, 1) - mic_loc(1, 1)) * 2 / c / delay(1, 2);
        B = -(mic_loc(3:end, 2) - mic_loc(1, 2)) * 2 / c ./ delay(1, 3:end)' + (mic_loc(2, 2) - mic_loc(1, 2)) * 2 / c / delay(1, 2);

        D1 = c * (delay(1, 2) - delay(1, 3:end))';
        D2 = mic_loc(1, 1)^2 + mic_loc(1, 2)^2 - mic_loc(3:end, 1).^2 - mic_loc(3:end, 2).^2;
        D3 = mic_loc(1, 1)^2 + mic_loc(1, 2)^2 - mic_loc(2, 1)^2 - mic_loc(2, 2)^2;

        D = -D2 / c ./ delay(1, 3:end)' + D3 / c / delay(1, 2) + D1;

        AA = [A, B];
    end

    % Solve the least squares problem
    X = (AA'*AA)\(AA')*(-D);
    X = X';  % 转为行向量
end
