#ifndef __SIGNAL_PROCESS_H
#define __SIGNAL_PROCESS_H

#include "cs5340.h"
#include <stdio.h>
#include <math.h>

typedef struct 
{
  uint8_t header[2];        //�����ֽڣ���ͷ
  uint8_t len;              //һ���ֽڣ�����
  uint8_t id;               //һ���ֽڣ��豸id
  uint8_t order_number;     //һ���ֽڣ�ָ���
  uint8_t lasttime[3];      //�����ֽڣ��������Ӧ��ʱ�䣬ʱ����
  uint32_t last_dma_num;    //�ĸ��ֽڣ������嵽��ʱ�̶�Ӧ��DMA�жϵĴ�������
  uint32_t dma_num_now;     //�ĸ��ֽڣ���ʼѰ���źŵ���ʱ��ʱDMA�жϵĴ�������
  uint16_t last_remain;     //�����ֽڣ������嵽��ʱ�̾�����һ��buffer����ʣ���uint16����
  uint16_t arrival_time;    //�����ֽڣ��źŵ���ʱ��
  float energy_27;          //���ֽڣ�27k����
  uint16_t crc;             //�����ֽڣ�У����
}data_stream1;

#pragma pack(1)
typedef struct
{
  uint8_t header[2];        //�����ֽڣ���ͷ
  uint8_t len;              //һ���ֽڣ�����
  uint8_t id;               //һ���ֽڣ��豸id
  uint8_t order_number;     //һ���ֽڣ�ָ���
  uint8_t magnification;    //1���ֽڣ��Ŵ���
  uint16_t threshold;       //2���ֽڣ��������
  uint16_t frequency;       //2���ֽڣ����Ƶ��
  uint8_t ratio;            //1���ֽڣ����ı�ֵ����
  uint16_t crc;             //�����ֽڣ�У����
}para_signal_process;
#pragma pack()

#pragma pack(1)
typedef struct 
{
  uint8_t header[2];        //�����ֽڣ���ͷ
  uint8_t len;              //һ���ֽڣ�����
  uint8_t id;               //һ���ֽڣ��豸id
  uint8_t order_number;     //һ���ֽڣ�ָ���
  float energy;             //�ĸ��ֽڣ��ź�ǿ��
  float energy_3;           //�ĸ��ֽڣ���Ϊ�Աȵ��ź�ǿ��
  uint16_t crc;             //�����ֽڣ�У����
}data_energy;
#pragma pack()

extern data_stream1 data_stream;
extern data_energy Energy;
extern float energy_27k;
extern float energy_;
extern uint8_t detect_ratio;

void Pcm2Vol_RemoveDC(int32_t * datain, float * dataout, uint8_t bits, uint8_t vol, size_t element_num);
uint8_t Detect_Signal(const float *data, size_t data_len, uint32_t fft_len, uint32_t fs, uint32_t f, uint16_t threshold);
void fliplr(float * data, size_t data_len);  
void first_order_corr(float * data_in, float * data_out, size_t data_len_in);
void get_max_value(float *data_in, size_t data_len, float *max_val, uint32_t *max_index);
int find_first_greater_than(float * data, float value, size_t data_len);
void polyfit(int x, float * y, size_t n, float * k, float * b);
uint16_t time_arrival_detection(float *data_in, size_t len);
void move_float_data(const float *source, float *destination, size_t count);
#endif
