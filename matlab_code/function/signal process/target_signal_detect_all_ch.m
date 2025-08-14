function [status] = target_signal_detect_all_ch(s,f,fs)
% 输入s是一个元组，检测每个元素信号是否包含目标信号
status = 1;
for i = 1:length(s)
    s_temp = cell2mat(s(i));
    status_temp = target_signal_detect(s_temp,f,fs);
    if status_temp==0  % 存在检测不到目标信号的信号
        status = 0;
        break;
    end
end
end