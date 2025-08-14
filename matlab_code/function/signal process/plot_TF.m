function plot_TF(data, fs)
t=(0:length(data)-1)/fs;

figure;
subplot(211)
plot(t, data,'LineWidth',1.5)
xlabel('时间/s');ylabel('幅度');title('时域波形')

subplot(212)
spectrogram(data, hanning(512), 500, 1024, fs,'yaxis');  % 信号，窗函数，重叠长度，FFT点数，采样率，频率信息显示在Y轴
title('时频谱')
end