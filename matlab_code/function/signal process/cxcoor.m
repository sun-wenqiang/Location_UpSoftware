function [result1, result2] = cxcoor(x, y)
    % 广义互相关
    % 输入：两路信号x和y
    % 输出：result1：广义互相关，加权函数为：平滑相关变换SCOT
    %       result2：互相关
    max_len = max(length(x), length(y));
    x_fft = fft(x, max_len);
    y_fft = fft(y, max_len);
    h1 = conj(x_fft) .* x_fft;
    h2 = conj(y_fft) .* y_fft;
    h = sqrt(h1 .* h2);
    result1 = ifft(conj(x_fft) .* y_fft ./ h);
    result2 = ifft(conj(x_fft) .* y_fft);
end
