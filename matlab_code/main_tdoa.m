clc;clear;close all

%% 参数设置
% 信号参数
node_ids = [1 2 3 4 5];
fs = 85250;             % 采样率
SNR = 50;               % 信噪比
mic_num = length(node_ids);            % 麦克风数量
c = 1500;               % 声速
frequency = 27e3;       % 信号频率
Tp = 3;                 % 总时长
pulse_duration = 0.02;  % 脉冲持续时间
interval = 0.98;        % 脉冲出现间隔

error_all = 0;
location_num = 1;
delay_errors = (0:0.001:0.6)*1e-3;
pos_errors = (0:0.01:1);  % 位置节点自身位置误差
for l = 1:location_num
    % 位置参数
    % source_loc = [randi([0,50]), randi([0,50])];  % 声源坐标
    % mic_loc = [1, 0.5;      % 麦克风坐标
    %            48, 25;
    %            25, 49;
    %            40, 5;
    %            6, 30;
    %            20, 9];
    % source_loc = [-24.85 17.07];
    source_loc = [30 16];
    % source_loc = [25 16];
    % mic_loc = [-40 60;
    %     -16.7 -15.46;
    %     14.8  21.8];
    mic_locs = [-3.60241979345541e-10	3.94054433144055e-05	1.15683717958935e-05
                % -40 60 0
               -16.7042534849670	-15.4571452487947	-2.90937536178149e-05
               -33.0432515342156	63.4795872077147	-0.000391156576775131
               -59.3436997764090	3.92353383541436	-0.000265479713165817
               % -20 50 0
               14.8022808123470	   21.8006517020465	   -4.30050919284497e-05];
    
    % mic_loc = mic_locs(node_ids, 1:2);
    mic_loc = [0 0;25 3;45 30;25 46;15 35];
    % mic_loc = [0 0;38 10;48 42;2 47];
    spread_time = zeros(1, mic_num);  % 传播时长，s为单位

    % 检测参数
    len_s = 10e-3;    % 一个buffer的数据量
    len_n = floor(len_s*fs);  %每次处理的信号长度
    threshold = 25;  % 信号检测阈值
    cross = zeros(1,mic_num);  % 跨越反射的参数
    storage = ones(1,mic_num);  % 存储数据位置的参数
    find_27k = zeros(1,mic_num);  % 寻找到信号的参数
    ready = zeros(1,mic_num);  % 完成到达时间寻找的标志
    buffer_index = zeros(1,mic_num);  % 信号到达时间的buffer索引
    arival_point = zeros(1,mic_num);  % 信号到达点索引
    data_arrival = cell(1,mic_num);   % 存放进行到达点寻找的数据
    for h = 1:mic_num
        data_arrival{h} = zeros(1,2*len_n);
    end

    % 定位参数
    error = 0;  % 一个位置的蒙特卡洛仿真平均误差
    delay = zeros(mic_num);     % 存放计算得到的时延
    Location = [];  % 每一列一个结果
    Location_error = [];

    figure;
    plot(mic_loc(:,1),mic_loc(:,2),'o','LineWidth',1.5)
    hold on;
    plot(source_loc(1),source_loc(2),'*','LineWidth',1.5)
    text(source_loc(1),source_loc(2), "信标",'FontSize',18)
    hold off;
    legend('水听器','信标','FontSize',18);
    xlabel('X(m)');ylabel('Y(m)','FontSize',18);
    title('节点布局','FontSize',18);grid on
    for ii = 1:size(mic_loc, 1)
        text(mic_loc(ii, 1), mic_loc(ii, 2), sprintf("node %d", node_ids(ii)),'FontSize',15)
    end
    hold on;
    min_x = min([mic_loc(:, 1);source_loc(1)]);
    max_x = max([mic_loc(:, 1);source_loc(1)]);
    min_y = min([mic_loc(:, 2);source_loc(2)]);
    max_y = max([mic_loc(:, 2);source_loc(2)]);
    xlim([min_x-5, max_x+5])
    ylim([min_y-5, max_y+5])
    

    %% 生成发射信号
    t = 0:1/fs:Tp-1/fs;                   % 时间
    signal_transmit = zeros(size(t));     % 初始化信号数组

    % 生成信号
    end_times = pulse_duration : (interval+pulse_duration) : Tp;
    for i = 1 : length(end_times)
        end_time = end_times(i);

        start_idx = round((end_time-pulse_duration) * fs) + 1;  % 计算该脉冲信号的采样点范围
        end_idx = round(end_time * fs);

        % 生成该脉冲信号
        if end_idx <= length(signal_transmit)  % 确保不超过信号数组的长度
            signal_transmit(start_idx:end_idx) = sin(2 * pi * frequency * t(start_idx:end_idx));
        end
    end
    % plot_TF(signal_transmit, fs)
    % sgtitle("信标发射信号")

    %% 生成接收信号（根据距离计算添加时延，不加噪声）
    % 根据距离计算传播时长，存到spread_time中，并利用传播时长在signal_transmit前补零
    data_temp = cell(1, mic_num);
    for i = 1:mic_num
        distance = calculation_distance(source_loc, mic_loc(i,:));
        spread_time(i) = distance/c;
        spread_time_n = floor(spread_time(i)*fs);
        data_temp{i} = [zeros(1,spread_time_n) signal_transmit];
    end
    data_length = cellfun(@length, data_temp);
    max_len = max(data_length);
    data_node_no_noise = zeros(mic_num, max_len);  % 每个节点采集到的信号，不加噪声

    % 给末尾补零，让各个信号长度一致，得到每个节点收到的信号data_node(没加噪声)
    for i = 1:length(data_temp)
        if data_length(i)<max_len
            data_node_no_noise(i,:) = [data_temp{i} zeros(1, max_len-data_length(i))];
        else
            data_node_no_noise(i,:) = data_temp{i};
        end
    end

    % 计算时延真值
    delay_true = zeros(mic_num);
    for ii = 1:mic_num-1
        for iii = ii+1:mic_num
            delay_true(ii,iii) = spread_time(iii)-spread_time(ii);
        end
    end

    %% 添加噪声, 蒙特卡洛仿真计算平均误差，滑窗处理
    load 'noise.mat';
    num = floor(length(data_node_no_noise(1,:))/(len_n));  % buffer数量
    epoch = 200;
    
    errors = zeros(length(delay_errors), epoch);
    % errors = zeros(length(pos_errors), epoch);
    for kkk = 1:length(delay_errors)
        delay_error = delay_errors(kkk);
    % for kkk = 1:length(pos_errors)
    for iii = 1:epoch
        %% 计算不同时延精度下的定位误差
        delay_in = delay_true(1,:) + delay_error * randn(1,length(delay_true(1,:)));
        % delay_in = delay_true(1,:);
        pos_node = mic_loc;
        % pos_node = pos_node + randn(size(pos_node))*pos_errors(kkk);
        Location_temp = LS(pos_node-pos_node(1,:), delay_in-delay_in(1), c);
        Location_temp = Location_temp + pos_node(1,:);
        errors(kkk, iii) = calculation_distance(Location_temp, source_loc);

        %%
        %-----------------------------------------添加真实环境噪声--------------------------------------------%
        % data_node_with_noise = zeros(mic_num, max_len);
        % for i = 1:mic_num
        %     data_node_with_noise(i,:) = add_environment_noise(data_node_no_noise(i,:), SNR, noise);
        % end

        %% 信号仿真

        % start_time=[170 170 1 2 3;170 170 1 2 4;170 170 1 2 2;170 170 1 2 3];
        % data_node_with_noise=data_node_with_noise(:,1:85250*20);
        %
        % data1=data_node_with_noise(1,:);
        % data1=data1(85250+1:end);  % 19s
        % data1=data1(1:18*85250);  % 18s
        %
        % data2=data_node_with_noise(2,:);
        % data2=data2(85250*2+1:end);  % 18s
        %
        % data3=data_node_with_noise(3,:);  %20s
        % data3=data3(1:18*85250);  %18s
        %
        % data4=data_node_with_noise(4,:);
        % data4=data4(85250+1:end);  % 19s
        % data4=data4(1:85250*18);  % 18s
        %
        % for i=1:18
        %     data1=[data1(1:85250*i+(i-1)*5) start_time(1,:) data1(85250*i+(i-1)*5+1:end)];
        %     start_time(1,5)=start_time(1,5)+1;
        % end
        % for i=1:18
        %     data3=[data3(1:85250*i+(i-1)*5) start_time(3,:) data3(85250*i+(i-1)*5+1:end)];
        %     start_time(3,5)=start_time(3,5)+1;
        % end
        % for i=1:18
        %     data2=[data2(1:85250*i+(i-1)*5) start_time(2,:) data2(85250*i+(i-1)*5+1:end)];
        %     start_time(2,5)=start_time(2,5)+1;
        % end
        % for i=1:18
        %     data4=[data4(1:85250*i+(i-1)*5) start_time(4,:) data4(85250*i+(i-1)*5+1:end)];
        %     start_time(4,5)=start_time(4,5)+1;
        % end
        %%
        % if iii==1&&l==1
        %     plot_TF(data_node_with_noise(mic_num,:),fs)
        %     sgtitle("水听器接收信号")
        % end

        %-----------------------------------------信号检测和寻找到达时间--------------------------------------%
        % for k = 1:num
        %     for g = 1:mic_num
        %         if cross(g)~=0
        %             cross(g) = cross(g)-1;
        %         else
        %             data_temp = data_node_with_noise(g,:);   % 第g路信号
        %             data = data_temp((k-1)*len_n+1:k*len_n);  % 第g路信号中需要检测的部分
        %             fft_len = 2^(ceil(log2(length(data))));
        %             [output, energy_27k]=Detect_Signal(data, fft_len, fs, 27e3, threshold);   % 信号检测
        %             if output==1
        %                 data_arrival{g}((storage(g)-1)*len_n+1:storage(g)*len_n) = data;      % 第一次检测到
        %                 storage(g) = 2;
        %                 find_27k(g) = find_27k(g)+1;
        %                 if find_27k(g)==2
        %                     cross(g) = 70;    % 寻找到到达时间了，之后30个buffer(300ms)不看
        %                     find_27k(g) = 0;
        %                     storage(g) = 1;
        %                     arival_point(g) = first_order_autocorrelation(data_arrival{g}(1:852));  % 寻找到达时间
        %                     buffer_index(g) = k-1;
        %                     ready(g) = 1;
        %                 end
        %             else
        %                 storage(g) = 1;
        %                 find_27k(g) = 0;
        %             end
        %         end
        %         if ready~=0
        %             break
        %         end
        %     end
        % 
        %     %-------------------------------------均寻找完到达时间，开始位置解算-----------------------------------%
        %     if ready~=0
        %         ready = zeros(1,mic_num);
        %         for i = 1:mic_num-1
        %             for j = i+1:mic_num
        %                 delay(i,j) = (buffer_index(j)-buffer_index(i))*len_s+(arival_point(j)-arival_point(i))/len_n*len_s;
        %             end
        %         end
        %         pos_node = mic_loc(1:mic_num,:);
        %         % pos_node = pos_node + randn(size(pos_node));
        %         Location_temp = LS(pos_node-pos_node(1,:), delay(1,:), c);
        %         Location_temp = Location_temp + pos_node(1,:);
        %         if isnan(Location_temp)
        %             error('NaN');
        %         end
        %         if all(abs(Location_temp)<100)
        %             Location = [Location; Location_temp];
        %             Location_error_temp = calculation_distance(Location_temp, source_loc);
        %             Location_error = [Location_error Location_error_temp];
        %         end
        %     end
        % end

        % error = error + mean(Location_error);
    end
    end
    rmse_errors = sqrt(mean(errors.^2, 2));
    figure
    plot(delay_errors*1000, rmse_errors, '-*', "LineWidth",1.5)
    % plot(pos_errors, rmse_errors, '-*', "LineWidth",1.5)
    yline(1, 'r-', 'RMSE=1m', 'LineWidth', 2,'FontSize',18);  % y = 3 的红色横线，并带文字
    grid on
    % xlabel("节点自身位置误差/m",'FontSize',18)
    xlabel("到达时间误差/ms",'FontSize',18)
    ylabel("RMSE/m",'FontSize',18)
    % title(sprintf("不同节点自身位置误差的定位误差(%d节点)", length(node_ids)),'FontSize',18)
    title(sprintf("不同到达时间误差下的定位误差(%d节点)", length(node_ids)),'FontSize',18)
    
    %% 绘制节点和定位点分布
    figure;
    axis equal
    plot(mic_loc(1:mic_num,1),mic_loc(1:mic_num,2),'o','LineWidth',1.5)
    for ii = 1:size(mic_loc, 1)
        text(mic_loc(ii, 1), mic_loc(ii, 2), sprintf("node %d", node_ids(ii)),'FontSize',15)
    end
    hold on;
    plot(source_loc(1),source_loc(2),'*','LineWidth',1.5)
    plot(Location(:,1), Location(:,2),'.','LineWidth',0.8)
    hold off;
    legend('节点','信标真实位置','定位结果','FontSize',15);
    xlabel('X(m)','FontSize',15);
    ylabel('Y(m)','FontSize',15);
    title('节点、信标与定位结果布局','FontSize',15);grid on

    error = error/epoch;
    disp(['位置',num2str(l),', SNR=',num2str(SNR),'dB, 节点数量=',num2str(mic_num), ', 蒙特卡洛仿真',num2str(epoch),'次，平均误差(m)：',num2str(error)])
    error_all = error_all+error;
end
error_all = error_all/location_num;
disp(['所有位置平均误差(m): ',num2str(error_all)])