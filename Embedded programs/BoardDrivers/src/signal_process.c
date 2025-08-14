#include "main.h"
#include "arm_math.h"
#include "stdint.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

data_stream1 data_stream={{0xff, 0xa5},26,0,5};  //��ʼ����ͷ�����ȣ�ID(��ʱ��Ϊ0)��ָ���
data_energy Energy = {{0xff, 0xa5},15,0,10};  //��ʼ����ͷ�����ȣ�ID(��ʱ��Ϊ0)��ָ���
float32_t fft_input[1024] = {0};
float32_t fft_output[1024] = {0};
float energy_27k = 0.0f;
float energy_ = 0.0f;

para_signal_process Para = {{0xff, 0xa5},13,0,0x0D};  //��ʼ����ͷ�����ȣ�ID(��ʱ��Ϊ0)��ָ���
float gainValues[8] = {-120, 0, 6, 14, 20, 26, 34, 40};

/*��pcm�ź�תΪfloat�������ݣ���֤��OK*/
void Pcm2Vol_RemoveDC(int32_t * datain, float * dataout, uint8_t bits, uint8_t vol, size_t element_num)
{
  if (bits>=32)
  {
    bits=31;
  }
  if (vol<=0)
  {
    vol=1;
  }
  uint32_t pcm_range = (1<<bits);
  float scale = ((float)vol)/pcm_range;
  for (size_t  i=0; i<element_num; i++)
  {
    dataout[i] = datain[i]*scale;
  }
  
  float dc = 0.0f;
  for (size_t  i=0; i<element_num; i++)
  {
    dc += dataout[i];
  }
  dc /= element_num;
  
  for (size_t  i=0; i<element_num; i++)
  {
    dataout[i] -= dc;
  }
}

/*�źż�⺯������֤OK*/
uint8_t Detect_Signal(const float *data, size_t data_len, uint32_t fft_len, uint32_t fs, uint32_t f, uint16_t threshold) 
{
    uint8_t out = 2;
    // ������������䵽�������������㣨���� data ����Ϊ fft_len��
    for (uint16_t i = 0; i < fft_len; i++) {
        fft_input[i] = (i < data_len) ? data[i] : 0.0f;  // �����
    }

    // ���� CMSIS FFT ʵ��
    arm_rfft_fast_instance_f32 S;
    arm_rfft_fast_init_f32(&S, fft_len);

    // ִ�� FFT
    arm_rfft_fast_f32(&S, fft_input, fft_output, 0);

    // �����ֵ��
    for (uint16_t i = 0; i < fft_len / 2; i++) 
    {
        float32_t real = fft_output[2 * i];
        float32_t imag = fft_output[2 * i + 1];
        fft_output[i] = sqrtf(real * real + imag * imag); // �����ֵ
    }

    // ����Ŀ��Ƶ������
    uint16_t index_27kHz = (uint16_t)((float)f / fs * fft_len);
    uint16_t inference_index = f+3000;
    if (inference_index>85250/2)   //��֤���������Ƶ��
    {
      inference_index = 85250/2;
    }
    uint16_t index_inference = (uint16_t)((float)inference_index / fs * fft_len);

    // �ۼ�Ŀ��Ƶ�ʸ���������
    energy_27k = 0.0f;
    energy_ = 0.0f;
    for (int16_t i = -5; i <= 5; i++) 
    {
        int16_t idx = index_27kHz + i;
        if (idx >= 0 && idx < fft_len / 2) 
        {
            energy_27k += fft_output[idx];
        }
        
        int16_t idx_ = index_inference + i;
        if (idx_ >= 0 && idx_ < fft_len / 2) 
        {
            energy_ += fft_output[idx_];
        }
        
    }

    // �о��ź��Ƿ����
    //uint8_t gain_now = Para.magnification;
    //float scale = pow(10.0f, (gainValues[gain_now] - 20.0f)/20);
    if (energy_27k > threshold && (energy_27k > ((float)detect_ratio * energy_))) {
        out = 1;
    } else 
    {
        out = 0;
    }
    return out;
}


/*��ת��������֤OK*/
void fliplr(float * data, size_t data_len)
{
  size_t start = 0;
  size_t end = data_len - 1;
  while(start<end)
  {
    float temp = data[start];
    data[start] = data[end];
    data[end] = temp;
    start++;
    end--;
  }
}

/*һ������أ�����OK*/
void first_order_corr(float * data_in, float * data_out, size_t data_len_in)
{
  for (int k=0;k<data_len_in-1;k++)
  {
    data_out[k] = data_in[k]*data_in[k+1];
  }
}

/*Ѱ��data_in�е����ֵ������OK*/
void get_max_value(float *data_in, size_t data_len, float *max_val, uint32_t *max_index) 
{
    // ���� arm_max_f32 �������ֵ
    arm_max_f32(data_in, data_len, max_val, max_index);
}

/*Ѱ�ҵ�һ������ĳ��ֵ���������Ҳ����򷵻�-1������OK*/
int find_first_greater_than(float * data, float value, size_t data_len)
{
  size_t index = 0;
  for (index=0;index<data_len;index++)
  {
    if (data[index]>value)
    {
      return index;
    }
  }
  return -1;
}

/*һ��������ϣ�����OK*/
//����xΪ�Ա�����һ��ֵ��֮���ֵ����Ϊ1���е�����yΪ��������飬nΪ�Ա���������k��b�ֱ�Ϊб�ʺͽؾ�
void polyfit(int x, float * y, size_t n, float * k, float * b)
{
  float sum_x=0.0f, sum_y=0.0f, sum_x2=0.0f, sum_xy=0.0f;
  for (size_t i=0;i<n;i++)
  {
    sum_x += x+i;
    sum_y += y[i];
    sum_x2 += (x+i) * (x+i);
    sum_xy += (x+i) * y[i];
  }    
  *k = (n * sum_xy - sum_x * sum_y) / (n * sum_x2 - sum_x * sum_x);
  *b = (sum_y - (*k) * sum_x) / n;
}

uint16_t time_arrival_detection(float *data_in, size_t len)
{
  float max_value1=0;
  uint32_t index=0;
  int index_than1=0;
  int index_than2=0;
  float k=0;
  float b=0;
  
  fliplr(data_in, len);           //��data_in���з�ת
  for (int i=0;i<len-1;i++)   //һ�������
  {
    data_in[i]=data_in[i]*data_in[i+1];
  }
  
  for (int i=0;i<len-1;i++)  //��һ������ؽ��о���ֵ�ۼ�
  {
    float temp=fabs(data_in[i]);
    if (i==0)
    {
      data_in[i]=temp;
    }
    else
    {
      data_in[i]=temp+data_in[i-1];
    }
  }
  
  get_max_value(data_in, len-1, &max_value1, &index);  //û����
  
  for (int i=0;i<len-1;i++)  //�������ֵ
  {
    data_in[i]=data_in[i]/max_value1;
  }
  
  index_than1 = find_first_greater_than(data_in, 0.1f, len-1);
  index_than2 = find_first_greater_than(data_in, 0.8f, len-1);  //û����
  
  for (int i=0;i<buffer_point_num-1;i++)  //��ԭ�ۼӺ���
  {
    data_in[i]=data_in[i]*max_value1;
  }
  
  polyfit(index_than1, data_in+index_than1, index_than2-index_than1+1, &k, &b);  //һ���������  û����
  
  for (int i=0;i<buffer_point_num-1;i++)  //�����µĺ���
  {
    data_in[i]=data_in[i]-0.1f*k*i;
  }
  
  get_max_value(data_in+(len-850-1), 851, &max_value1, &index);
  uint16_t arrival_point=len-((len-850-1)+index)-2;
  return arrival_point;
}


void move_float_data(const float *source, float *destination, size_t count) 
{
    // ����У�飬�����ָ��
    if (source == NULL || destination == NULL) {
        return;
    }

    // �������ݣ��������
    for (size_t i = 0; i < count; i++) {
        destination[i] = source[i];
    }
}