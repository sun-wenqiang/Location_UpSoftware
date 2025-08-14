function [tao_h, tao] = calculate_delay(x, y, time_domain)
% 利用广义互相关和一般的互相关计算时延，计算的是x相对于y的时延
% 输入：两路信号x、y和time_domain
% 输出：tao_h为使用广义互相关求出的时延，tao是利用一般的互相关求出的时延
% 如果时延为负值，表示x超前于y，否则为x延后于y
if time_domain
    [xc, lags] = xcorr(x, y);
    [~, idx] = max(xc);
    tao = lags(idx);
    tao_h = tao;
else
    max_len = max(length(x), length(y));
    [corr_h, corr] = cxcoor(x, y);

    [~, peak_index] = max(corr);    % 获取corr中的最大值的索引
    [~, peak_index_h] = max(corr_h); % 获取corr_h中的最大值的索引

    if peak_index > max_len / 2
        tao_h = (max_len - peak_index_h);
        tao = (max_len - peak_index);
    else
        tao_h = -peak_index_h;
        tao = -peak_index;
    end
end
end
