function reportStatus(code, allow05)
    if nargin < 2, allow05 = false; end
    switch code
        case 0
            disp('操作成功');
        case hex2dec('05')
            if allow05
                disp('错误：内存写入失败');
            else
                reportError(code);
            end
        otherwise
            reportError(code);
    end
end