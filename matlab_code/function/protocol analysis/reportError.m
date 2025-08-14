function reportError(code)
    switch code
        case hex2dec('03')
            disp('错误：指令号错误');
        case hex2dec('04')
            disp('错误：指令内容错误');
        case hex2dec('05')
            disp('错误：内存写入失败');
        case hex2dec('07')
            disp('错误：长度错误');
        case hex2dec('08')
            disp('错误：CRC校验失败');
        case hex2dec('09')
            disp('错误：读取超时');
        case hex2dec('0A')
            disp('错误：GPS未定位');
        otherwise
            fprintf("未知错误码：0x%02X\n", code);
    end
end