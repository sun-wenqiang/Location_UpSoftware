function sendCommand(t, device_id, cmd_id, cmd_data)
    % 包头
    header = uint8([0xFF, 0xA5]);
    
    % 数据部分（用于CRC计算）
    id       = uint8(device_id);
    cmd      = uint8(cmd_id);
    content  = uint8(cmd_data);
    len      = uint8(length(content)+7);  % 固定
    
    payload = uint8([header, len, id, cmd, content]);

    % 计算 CRC
    crc = calculateCRC16Modbus(uint16(payload));

    % 最终帧
    tx = uint8([payload, crc]);

    % 发送
    write(t, tx, "uint8");
end
