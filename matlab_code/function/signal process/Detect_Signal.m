function [output, energy_27k, ratios] = Detect_Signal(data, fft_len, fs, f, threshold, ratio)
% 输入：
%      data:待检测信号
%      fft_len:对data进行FFT的点数，必须是2的次幂
%      fs:采样率
%      f:需要检测的目标信号频率
%      threshold:检测门限
%输出：
%      output:0或1，表示不存在/存在目标信号

data_fft = abs(fft(data, fft_len));

index_27kHz = floor(f/fs*fft_len);
index_40kHz = floor((f+3000)/fs*fft_len);
% index_25kHz = floor(25000/fs*fft_len);
% index_30kHz = floor(30000/fs*fft_len);
energy_27k = 0;
energy_40k = 0;
for k = -5:5
    index = index_27kHz + k;
    if index>0 && index<512
        energy_27k = energy_27k + data_fft(index);
    end
    index = index_40kHz + k;
    if index>0 && index<512
        energy_40k = energy_40k + data_fft(index);
    end
end
% energy_27k = sum(data_fft(index_27kHz-5:index_27kHz+5));
% energy_40k = sum(data_fft(index_40kHz-5:index_40kHz+5));
ratios = energy_27k/energy_40k;
% energy_25_30k = sum(data_fft(index_25kHz:index_30kHz));
% ratios = (energy_27k/energy_25_30k);
% if energy_27k>threshold %&& energy_40k < 50
%     if detect_inference && ratio < ratios
%         output = 1;
%     else
%         output = 0;
%     end
% else
%     output = 0;
% end

if energy_27k>threshold && (energy_40k * ratio < energy_27k)
    output = 1;
else
    output = 0;
end