function crc_hex = calculateCRC16Modbus(data)
    % calculateCRC16Modbus - 计算CRC-16/MODBUS校验码
    % 输入:
    %   data: uint16数组，表示需要计算的字节数组，取值范围为 0-255。
    % 输出:
    %   crc_hex: 1x2 uint8数组，表示高字节和低字节的十六进制值。

    % 校验多项式和初始值
    POLY = uint16(hex2dec('A001')); % CRC-16/MODBUS多项式，反射形式
    crc = uint16(hex2dec('FFFF')); % 初始值

    % 确保输入为uint16类型
    if ~isa(data, 'uint16')
        error('输入数据必须为 uint16 类型');
    end

    % 按字节处理数据
    for i = 1:length(data)
        crc = bitxor(crc, data(i)); % 初始异或

        for j = 1:8 % 遍历每一位
            if bitand(crc, 1) % 检查最低有效位
                crc = bitshift(crc, -1); % 右移一位
                crc = bitxor(crc, POLY); % 异或多项式
            else
                crc = bitshift(crc, -1); % 右移一位
            end
        end
    end

    % 转化为高字节和低字节
    crc_high = bitshift(crc, -8); % 高字节
    crc_low = bitand(crc, 255); % 低字节

    % 返回结果为十六进制表示
    crc_hex = uint8([crc_high, crc_low]);
end
