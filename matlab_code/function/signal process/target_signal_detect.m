function [status] = target_signal_detect(s,f,fs)
% 检测s中f频率目标信号是否存在
% 计算s中哪个频率分量最大，这个频率在f的±1K范围内，则视为检测到信号
n=ceil(log2(length(s)));
s_fft = (abs(fft(s,2^n)));
s_fft = s_fft(1:2^(n-1));
[~, index] = max(s_fft);
max_f = index/n*fs;
if max_f>(f-1000)||max_f<(f+1000)
    status = 1;
else
    status = 0;
end
end