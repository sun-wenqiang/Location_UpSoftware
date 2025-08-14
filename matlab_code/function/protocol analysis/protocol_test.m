clc;clear;

%% 
tcp_main();

%% 主控制函数
function tcp_main()
    save_folder = "./test";  % 存储数据的根目录
    if ~exist(save_folder, 'dir')
        mkdir(save_folder);
    end
    fs = 85250;  % 采样率（Hz）
    frame_len = 852;  % 每帧点数
    fft_len = 1024;   % FFT长度
    target_freq = 27000; % 目标频率（Hz）
    % 可选设备 ID（0~5）
    device_id = input('请输入设备ID（0~5）: ');
    if device_id < 0 || device_id > 5
        error('无效ID');
    end
    
    % 根据ID自动匹配IP与端口
    ip = sprintf('192.168.1.%d', 160 + device_id);
    port = 5000 + device_id;
    % ip='192.168.1.165';
    % port=5005;
    
    % 建立TCP连接
    t = tcpclient(ip, port, 'Timeout', 20);
    fprintf("已连接至设备 ID=%d，IP=%s，端口=%d\n", device_id, ip, port);
    disp("======== 指令菜单 ========");
    disp("1. 获取电量");
    disp("2. 开始工作");
    disp("3. 停止工作");
    disp("4. 设置放大倍数");
    disp("5. 设置设备ID");
    disp("6. 设置检测的能量阈值");
    disp("7. 查询GPS有效性");
    disp("8. 获取经纬度");
    disp("9. 重启设备");
    disp("10. 获取27kHz和30kHz的能量");
    disp("11. 获取MCU回复");
    disp("12. 持续监控");
    disp("13. 采集连续信号");
    disp("14. 设置检测频率");
    disp("15. 获取放大倍数、检测频率、检测的能量和比值阈值");
    disp("16. 设置检测的比值阈值");
    disp("0. 退出程序");

    % 发送菜单
    while true
        choice = input("请输入操作编号: ");
        switch choice
            case 1
                sendCommand(t, device_id, hex2dec('00'), hex2dec('00'));
            case 2
                option = input("0：返回到达时间；\n1：返回脉冲信号。\n其他值默认为0: ");
                if option == 1
                    duration = input("请输入监控时长（0~30s）: ");
                    if duration > 30
                        duration = 30;
                        disp("监测时长大于30s，已经限制到30s！")
                    elseif duration < 3
                        duration = 3;
                        disp("监测时长小于3s，已经增大到3s！")
                    end
                    sendCommand(t, device_id, hex2dec('02'), [hex2dec('CC') duration]);

                    total_Bytes = 0;
                    currentTime = datetime('now');  % 获取当前时间
                    formattedTime = datestr(currentTime, 'yyyy-mm-dd_HH-MM-SS');   %#ok<DATST> % 格式化时间
    
                    fileName = sprintf('%s/node%d_%s_%ds_pulse.bin', ...
                        save_folder, device_id, formattedTime, duration);  % 生成文件名
                    fileID = fopen(fileName, 'w');  % 创建文件
                    if fileID == -1
                        error('无法创建文件: %s', fileName);
                    end
                    first = 1;
                    start = tic;
                    while toc(start)<(duration+3)
                        if t.NumBytesAvailable >0
                            data = read(t, t.NumBytesAvailable);
                            if first
                                disp(data(1:8))
                                data = data(9:end);
                                first = 0;
                            end
                            fwrite(fileID, data, 'uint8');
                            total_Bytes = total_Bytes + length(data)/4;
                        end
                    end
                    fprintf("接收完成，共收到%d个脉冲\n", total_Bytes/1705);
                    fclose(fileID);
                    if total_Bytes == 0
                        continue
                    end
                    fileID = fopen(fileName, 'r');  % 读取刚存的文件
                    signal = fread(fileID,'float32');
                    fclose(fileID);
                    % -----------------------
                    % 绘图
                    % -----------------------
                    t1 = (0:length(signal)-1)/fs;  % 时间轴
                    figure;
                    subplot(2,1,1);
                    plot(t1, signal);
                    xlabel('Time (s)');
                    ylabel('Amplitude');
                    title('Time Domain Signal');
                    grid on;
    
                    subplot(2,1,2);
                    spectrogram(signal, 256, 200, 512, fs, 'yaxis'); % 窗长256，重叠200，FFT点数512
                    title('Spectrogram');
                    grid on;
                    colorbar;
                    continue
                else
                    sendCommand(t, device_id, hex2dec('02'), hex2dec('AA'));
                end
            case 3
                sendCommand(t, device_id, hex2dec('02'), hex2dec('BB'));
            case 4
                gainValues = [-120, 0, 6, 14, 20, 26, 34, 40];
                fprintf('放大倍数选择列表（默认4，20dB，修改后设备会重启）：\n');
                for i = 0:7
                    fprintf('  x = %d -> 增益 = %d dB\n', i, gainValues(i+1));
                end
                x = input('输入增益等级(0-7): ');
                sendCommand(t, device_id, hex2dec('03'), x);
            case 5
                newID = input('输入新的设备ID(0-255)，修改后设备会重启: ');
                sendCommand(t, device_id, hex2dec('04'), newID);
            case 6
                threshold = input('输入阈值(0-65535)，修改后设备会重启: ');
                low_byte = bitand(threshold, 255);          % 低8位
                high_byte = bitshift(threshold, -8);        % 高8位
                sendCommand(t, device_id, hex2dec('06'), [low_byte, high_byte]);
            case 7
                sendCommand(t, device_id, hex2dec('07'), hex2dec('00'));
            case 8
                sendCommand(t, device_id, hex2dec('08'), hex2dec('00'));
            case 9
                sendCommand(t, device_id, hex2dec('09'), hex2dec('00'));
            case 10
                sendCommand(t, device_id, hex2dec('0A'), hex2dec('00')); 
            case 11
                [~] = readAndParseResponse(t);
                continue
            case 12
                x = input('输入监控时长(s): ');
                pause_time = 0.5;
                x = x/pause_time;
                while x>0
                    [~] = readAndParseResponse(t);
                    pause(pause_time)
                    x = x-1;
                end
            case 13
                close all;
                x = input('输入采集时长(s): ');
                sendCommand(t, device_id, hex2dec('0B'), uint8(x));
                
                % pause(0.015)
                % [~] = readAndParseResponse(t);
                total_Bytes = x*85250*4;
                currentTime = datetime('now');  % 获取当前时间
                formattedTime = datestr(currentTime, 'yyyy-mm-dd_HH-MM-SS');   %#ok<DATST> % 格式化时间

                % 创建以当前时间命名的文件
                % frequency = input('请输入频率: ', 's');
                fileName = sprintf('%s/node%d_%s_%ds.bin', ...
                        save_folder, device_id, formattedTime, x);    % 生成文件名
                % namePart = input('请输入文件名（不带扩展名）: ', 's');
                % fileName = ['./带通验证_20250617/', namePart, '.bin'];
                fileID = fopen(fileName, 'w');  % 创建文件
                if fileID == -1
                    error('无法创建文件: %s', fileName);
                end
                first = 1;
                start = tic;
                while total_Bytes>0
                    if t.NumBytesAvailable >0
                        data = read(t, t.NumBytesAvailable);
                        if first
                            disp(data(1:8))
                            data = data(9:end);
                            first = 0;
                        end
                        fwrite(fileID, data, 'uint8');
                        total_Bytes = total_Bytes - length(data);
                    end
                    end_time = toc(start);
                    if end_time>x+3
                        break
                    end
                end
                clear x;
                fclose(fileID);
                fileID = fopen(fileName, 'r');  % 读取刚存的文件
                signal = fread(fileID,'float32');
                fclose(fileID);
                % disp(max(signal(floor(2*fs):floor(2.05*fs))))
                % -----------------------
                % 绘图
                % -----------------------
                t1 = (0:length(signal)-1)/fs;  % 时间轴
                figure;
                set(gcf, 'Position', [750, 150, 700, 500]);  % 第一张图左上
                subplot(2,1,1);
                plot(t1, signal);
                xlabel('Time (s)');ylabel('Amplitude');title('Time Domain Signal');grid on;

                subplot(2,1,2);
                spectrogram(signal, 256, 200, 512, fs, 'yaxis'); % 窗长256，重叠200，FFT点数512
                title('Spectrogram');grid on;colorbar;

                numFrames = floor(length(signal) / frame_len);
                energy_array = zeros(1, numFrames);
                target_index = round(target_freq / fs * fft_len);  % 目标频率对应的索引
                index_range = (target_index-5):(target_index+5);   % ±5范围

                for i = 1:numFrames
                    frame = signal((i-1)*frame_len + (1:frame_len));

                    % 零填充到1024点FFT
                    fft_result = abs(fft(frame, fft_len));

                    % 取目标频率±5索引的幅度和
                    target_amplitude_sum = sum(fft_result(index_range));
                    energy_array(i) = target_amplitude_sum;
                end
                % 画幅度和变化图
                figure
                % 'Position' → [left, bottom, width, height]
                set(gcf, 'Position', [100, 100, 600, 400]);  % 第一张图左上
                plot(1:numFrames, energy_array, '-o');grid on
                xlabel('Frame Index');ylabel('Amplitude Sum');
                title(['Sum Amplitude around 27kHz ±5 (', fileName, ')'],'Interpreter','none');
                continue
            case 14
                freq = input('输入检测频率(单位Hz)，修改后设备会重启: ');
                low_byte = bitand(freq, 255);          % 低8位
                high_byte = bitshift(freq, -8);        % 高8位
                sendCommand(t, device_id, hex2dec('0C'), [low_byte, high_byte]);
            case 15
                sendCommand(t, device_id, hex2dec('0D'), 0);
            case 16
                ratio = input("请输入比值ratio：（27k/30k）>ratio作为检测条件，修改后设备会重启\n");
                sendCommand(t, device_id, hex2dec('0E'), uint8(ratio));
            case 0
                clear t;
                disp("连接已关闭。");
                break;
            otherwise
                disp("无效输入");
        end
        pause(0.02)
        [~] = readAndParseResponse(t);
    end
end

