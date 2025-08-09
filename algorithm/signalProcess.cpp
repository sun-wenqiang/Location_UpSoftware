#include "signalProcess.h"

float energy_27k = 0.0f;
float energy_ = 0.0f;
int detect_ratio = 2;  

void Pcm2Vol_RemoveDC(int32_t *datain, float *dataout, uint8_t bits, uint8_t vol, size_t element_num)
{
    if (bits >= 32)
    {
        bits = 31;
    }
    if (vol <= 0)
    {
        vol = 1;
    }
    uint32_t pcm_range = (1 << bits);
    float scale = ((float)vol) / pcm_range;
    for (size_t i = 0; i < element_num; i++)
    {
        dataout[i] = datain[i] * scale;
    }

    float dc = 0.0f;
    for (size_t i = 0; i < element_num; i++)
    {
        dc += dataout[i];
    }
    dc /= element_num;

    for (size_t i = 0; i < element_num; i++)
    {
        dataout[i] -= dc;
    }
}


uint8_t Detect_Signal(const float *data, size_t data_len, uint32_t fft_len, uint32_t fs, uint32_t f, uint16_t threshold)
{
    if (data_len > fft_len || data == nullptr || fft_len == 0) {
        return 2;  // 异常返回
    }

    // 准备FFT输入缓冲区，零填充
    std::vector<double> fft_input(fft_len, 0.0);
    for (size_t i = 0; i < data_len; i++) {
        fft_input[i] = static_cast<double>(data[i]);
    }

    // fftw输出为复数，长度为 fft_len/2 + 1
    std::vector<fftw_complex> fft_output(fft_len/2 + 1);

    // 创建FFT计划
    fftw_plan plan = fftw_plan_dft_r2c_1d(static_cast<int>(fft_len), fft_input.data(), fft_output.data(), FFTW_ESTIMATE);

    // 执行FFT
    fftw_execute(plan);

    // 计算幅值谱
    std::vector<double> magnitude(fft_len / 2);
    for (size_t i = 0; i < fft_len / 2; i++) {
        double real = fft_output[i][0];
        double imag = fft_output[i][1];
        magnitude[i] = sqrt(real*real + imag*imag);
    }

    // 计算目标频率对应的索引
    uint32_t index_27kHz = static_cast<uint32_t>((double)f / fs * fft_len);
    uint32_t inference_index = f + 3000;
    if (inference_index > 85250 / 2) {
        inference_index = 85250 / 2;
    }
    uint32_t index_inference = static_cast<uint32_t>((double)inference_index / fs * fft_len);

    energy_27k = 0.0;
    energy_ = 0.0;

    // 累加目标频率附近±5个点能量
    for (int i = -5; i <= 5; i++) {
        int idx1 = static_cast<int>(index_27kHz) + i;
        if (idx1 >= 0 && idx1 < static_cast<int>(magnitude.size())) {
            energy_27k += magnitude[idx1];
        }
        int idx2 = static_cast<int>(index_inference) + i;
        if (idx2 >= 0 && idx2 < static_cast<int>(magnitude.size())) {
            energy_ += magnitude[idx2];
        }
    }

    fftw_destroy_plan(plan);

    if (energy_27k > threshold && energy_27k > detect_ratio * energy_) {
        return 1;
    } else {
        return 0;
    }
}

/*翻转向量，验证OK*/
void fliplr(float *data, size_t data_len)
{
    size_t start = 0;
    size_t end = data_len - 1;
    while (start < end)
    {
        float temp = data[start];
        data[start] = data[end];
        data[end] = temp;
        start++;
        end--;
    }
}

/*一阶自相关，测试OK*/
void first_order_corr(float *data_in, float *data_out, size_t data_len_in)
{
    for (int k = 0; k < data_len_in - 1; k++)
    {
        data_out[k] = data_in[k] * data_in[k + 1];
    }
}

/*寻找data_in中的最大值，测试OK*/
void get_max_value(float *data_in, size_t data_len, float *max_val, uint32_t *max_index)
{
    if (data_len == 0 || data_in == nullptr || max_val == nullptr || max_index == nullptr)
        return;

    const float* max_ptr = std::max_element(data_in, data_in + data_len);
    *max_val = *max_ptr;
    *max_index = static_cast<uint32_t>(max_ptr - data_in);
}

/*寻找第一个大于某个值的索引，找不到则返回-1，测试OK*/
int find_first_greater_than(float *data, float value, size_t data_len)
{
    size_t index = 0;
    for (index = 0; index < data_len; index++)
    {
        if (data[index] > value)
        {
            return index;
        }
    }
    return -1;
}

// 输入x为自变量第一个值，之后的值步长为1进行递增；y为因变量数组，n为自变量个数；k和b分别为斜率和截距
void polyfit(int x, float *y, size_t n, float *k, float *b)
{
    float sum_x = 0.0f, sum_y = 0.0f, sum_x2 = 0.0f, sum_xy = 0.0f;
    for (size_t i = 0; i < n; i++)
    {
        sum_x += x + i;
        sum_y += y[i];
        sum_x2 += (x + i) * (x + i);
        sum_xy += (x + i) * y[i];
    }
    *k = (n * sum_xy - sum_x * sum_y) / (n * sum_x2 - sum_x * sum_x);
    *b = (sum_y - (*k) * sum_x) / n;
}

uint16_t time_arrival_detection(float *data_in, size_t len)
{
    float max_value1 = 0;
    uint32_t index = 0;
    int index_than1 = 0;
    int index_than2 = 0;
    float k = 0;
    float b = 0;

    fliplr(data_in, len);             // 将data_in进行翻转
    for (int i = 0; i < len - 1; i++) // 一阶自相关
    {
        data_in[i] = data_in[i] * data_in[i + 1];
    }

    for (int i = 0; i < len - 1; i++) // 对一阶自相关进行绝对值累加
    {
        float temp = fabs(data_in[i]);
        if (i == 0)
        {
            data_in[i] = temp;
        }
        else
        {
            data_in[i] = temp + data_in[i - 1];
        }
    }

    get_max_value(data_in, len - 1, &max_value1, &index); // 没问题

    for (int i = 0; i < len - 1; i++) // 除以最大值
    {
        data_in[i] = data_in[i] / max_value1;
    }

    index_than1 = find_first_greater_than(data_in, 0.1f, len - 1);
    index_than2 = find_first_greater_than(data_in, 0.8f, len - 1); // 没问题

    for (int i = 0; i < buffer_point_num - 1; i++) // 复原累加函数
    {
        data_in[i] = data_in[i] * max_value1;
    }

    polyfit(index_than1, data_in + index_than1, index_than2 - index_than1 + 1, &k, &b); // 一阶曲线拟合  没问题

    for (int i = 0; i < buffer_point_num - 1; i++) // 构造新的函数
    {
        data_in[i] = data_in[i] - 0.1f * k * i;
    }

    get_max_value(data_in + (len - 850 - 1), 851, &max_value1, &index);
    uint16_t arrival_point = len - ((len - 850 - 1) + index) - 2;
    return arrival_point;
}

void move_float_data(const float *source, float *destination, size_t count)
{
    // 参数校验，避免空指针
    if (source == NULL || destination == NULL)
    {
        return;
    }

    // 搬移数据，逐个复制
    for (size_t i = 0; i < count; i++)
    {
        destination[i] = source[i];
    }
}