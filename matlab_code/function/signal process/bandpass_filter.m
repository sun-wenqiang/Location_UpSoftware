function y = bandpass_filter(x, Fs, f_low, f_high, n)
    % bandpass_filter: 对输入信号进行带通滤波
    % 
    % 输入参数:
    %   x      - 输入信号
    %   Fs     - 采样频率 (Hz)
    %   f_low  - 带通滤波器的低截止频率 (Hz)
    %   f_high - 带通滤波器的高截止频率 (Hz)
    %   n      - 滤波器的阶数
    %
    % 输出参数:
    %   y      - 带通滤波后的信号

    % 归一化频率 (相对于 Nyquist 频率)
    Wn = [f_low f_high] / (Fs / 2); 

    % 设计 Butterworth 带通滤波器
    [b, a] = butter(n, Wn, 'bandpass'); 

    % % 显示频率响应
    % figure;
    % freqz(b, a, 1024, Fs); % 1024 为频率点数，可以根据需要调整
    % title('Bandpass Filter Frequency Response');

    % 应用滤波器
    y = filter(b, a, x); 
end
