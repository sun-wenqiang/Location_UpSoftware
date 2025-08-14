function reply = readAndParseResponse(t)
    reply = 0;
    if t.NumBytesAvailable == 0
        % disp('无返回数据');
        reply = -1;
        return;
    end

    raw = read(t, t.NumBytesAvailable, 'uint8');

    % 查找包头
    headIdx = find(raw(1:end-1) == 0xFF & raw(2:end) == 0xA5, 1);
    if isempty(headIdx) || headIdx + 2 > length(raw)
        disp('包头错误');
        reply = -1;
        return;
    end

    % 解析长度字段
    totalLen = raw(headIdx + 2);
    if headIdx + totalLen - 1 > length(raw)
        disp('数据长度不足');
        reply = -1;
        return;
    end

    frame = raw(headIdx : headIdx + totalLen - 1);

    % 验证 CRC
    dataToCheck = frame(1:end-2);  
    crcReceived = frame(end-1:end);
    crcCalculated = calculateCRC16Modbus(uint16(dataToCheck));

    if ~isequal(crcReceived, crcCalculated)
        disp('CRC校验失败');
        reply = -1;
        return;
    end

    % 解析字段
    id = frame(4);
    cmd = frame(5);
    payload = frame(6:end-2);   % 指令内容
    fprintf('Recieve response of node %d\n', id)

    % 分支解析
    switch cmd
        case hex2dec('00')  % 获取电量
            if totalLen == 9
                adc_val = typecast(uint8(payload(1:2)), 'uint16');
                voltage = double(adc_val) / 4095 * 3.1 * 11 + 0.1;
                fprintf("电量ADC = %d，电压 = %.2fV\n", adc_val, voltage);
            elseif totalLen == 8
                reportError(payload(1));
            else
                disp('电量返回长度异常');
            end

        case {hex2dec('02'), hex2dec('07'), hex2dec('09'), hex2dec('0B')}  % 采集控制，GPS是否有效，重启
            reportStatus(payload(1));

        case {hex2dec('03'), hex2dec('04'), hex2dec('06'), hex2dec('0C'), hex2dec('0E')}  % 放大倍数，设备ID，检测阈值
            reportStatus(payload(1), true);

        case hex2dec('0A')
            if totalLen == 11
                energy_27k = typecast(uint8(payload(1:4)), 'single');
                energy_30k = typecast(uint8(payload(5:8)), 'single');
                fprintf("27kHz能量为: %.6f， 30kHz能量为：%.6f。\n", energy_27k, energy_30k);
            else
                disp("能量返回长度异常");
            end
        case hex2dec('08')  % 获取经纬度
            if totalLen == 18
                hour = payload(1);
                minute = payload(2);
                second = payload(3);
                lon = typecast(uint8(payload(4:7)), 'single');
                lat = typecast(uint8(payload(8:11)), 'single');
                fprintf("PPS时间: %02d:%02d:%02d\n", hour, minute, second);
                fprintf("经度: %.9f，纬度: %.9f\n", lon, lat);
            else
                disp("经纬度返回长度异常");
            end

        case hex2dec('05')  % 到达时间
            hour = payload(1);
            minute = payload(2);
            second = payload(3);
            disp('******************************')
            fprintf("到达时间: %02d:%02d:%02d\n", hour, minute, second);
            DMA_num_last = double(typecast(uint8(payload(4:7)), 'uint32'));
            DMA_num_now = double(typecast(uint8(payload(8:11)), 'uint32'));
            Remain_last = double(typecast(uint8(payload(12:13)), 'uint16'));
            Arrival_time = double(typecast(uint8(payload(14:15)), 'uint16'));
            energy_27k = double(typecast(uint8(payload(16:19)), 'single'));
            fprintf('The offset is %.5f\n',(DMA_num_now-DMA_num_last-3)*0.01+Remain_last/3410*0.01+Arrival_time/1705*0.02)
            fprintf('The energy of 27kHz is %.5f\n',energy_27k)
            disp('******************************')

        case hex2dec('0D')
            gainValues = [-120, 0, 6, 14, 20, 26, 34, 40];
            magnification = typecast(uint8(payload(1)), 'uint8');
            threshold = typecast(uint8(payload(2:3)), 'uint16');
            frequency = typecast(uint8(payload(4:5)), 'uint16');
            ratio = typecast(uint8(payload(6)), 'uint8');
            fprintf(['放大倍数：%ddB, ' ...
                '检测频率：%dHz, ' ...
                '检测能量阈值：%d, ' ...
                '检测比值阈值：%d.\n'], ...
                gainValues(magnification+1), frequency, threshold, ratio)

        otherwise
            fprintf("未知指令号：0x%02X\n", cmd);
    end
end
