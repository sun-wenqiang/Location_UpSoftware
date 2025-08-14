function [index, root, p] = curve_fit(x,data,T)
%输入：x:需要拟合的自变量
%      data:需要拟合的因变量
%      T:连续T点上升则看作信号到来
%输出：index:信号连续T点在上升的起始点索引
%      root:进行完曲线拟合之后求得的零点
%      p:拟合系数


data_diff = diff(data);
n=length(data_diff);
i=1;
index = -1;
root = NaN;
p=NaN;
while i<=n-T+1
    % 从i点开始往后数T点，如果这T点都在上升，那么这个i就是信号起始的点
    data_temp1 = data_diff(i:i+T-1);
    if sum(data_temp1>0)>=T-2  % 连续上升，不连续上升的点不大于两点，允许有一点波动
        index = i;
        break;
    end
    i=i+1;
end
if index<0
    return;
else
    % 拟合一次极值点
    idx=index+5:index+10+5-1;
    p=polyfit(x(idx),data(idx),1);  % 一阶拟合曲线
    root = roots(p);
end
end

%% 
% function [index, root, p] = curve_fit(x,y,data,T)
% %输入：x:需要拟合的自变量
% %      y:需要拟合的因变量
% %      data:寻找起始点的数据
% %      T:连续T点上升则看作信号到来
% %输出：index:信号连续T点在上升的起始点索引
% %      root:进行完曲线拟合之后求得的零点
% %      p:拟合系数
% 
% 
% data1 = abs(data)/max(abs(data));  % 归一化
% data_diff = diff(data1);
% n=length(data_diff);
% i=1;
% index = -1;
% root = NaN;
% p=NaN;
% while i<=n-T+1
%     % 从i点开始往后数T点，如果这T点都在上升，那么这个i就是信号起始的点
%     data_temp1 = data_diff(i:i+T-1);
%     if sum(data_temp1>0)>=T-2  % 连续上升
%         index = i;
%         break;
%     end
%     i=i+1;
% end
% if index<0
%     return;
% else
%     % 拟合一次极值点
%     index_max = find(data(index)==y);
%     idx=index_max:index_max+T*2-1;
%     p=polyfit(x(idx),y(idx),1);  % 一阶拟合曲线
%     root = roots(p);
% end
% end