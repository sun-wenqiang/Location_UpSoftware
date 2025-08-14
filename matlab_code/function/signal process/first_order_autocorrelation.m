function [arival_point]= first_order_autocorrelation(data)
%输入：
%     data:输入信号
%输出：
%     data_autocorrelation：data的一阶自相关
%     arival_point: 信号到达时间
data=fliplr(data);  %先倒序
data_n = length(data);  
data_autocorrelation = data(1:end-1).*data(2:end);    % 一阶自相关
g = zeros(1,data_n-1);     
for i=1:data_n-1
    g(i) = sum(abs(data_autocorrelation(1:i)));   % 倒叙一阶自相关累加
end

g_temp = g/max(g);
index1 = find(g_temp > 0.1, 1);
index2 = find(g_temp > 0.8, 1);
p=polyfit(index1:index2,g(index1:index2),1);   % 累加结果拟合
% plot(g);hold on;plot(1:1700,(1:1700)*p(1)+p(2))
cn = 0.1*p(1)*(1:data_n-1);  
g1 = g-cn;
[~, g_max_idx] = max(g1((data_n-850):end));
arival_point = data_n-(g_max_idx+(data_n-850)-1);
end