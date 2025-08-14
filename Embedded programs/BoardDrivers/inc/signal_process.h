#ifndef __SIGNAL_PROCESS_H
#define __SIGNAL_PROCESS_H

#include "cs5340.h"
#include <stdio.h>
#include <math.h>

typedef struct 
{
  uint8_t header[2];        //两个字节，包头
  uint8_t len;              //一个字节，长度
  uint8_t id;               //一个字节，设备id
  uint8_t order_number;     //一个字节，指令号
  uint8_t lasttime[3];      //三个字节，秒脉冲对应的时间，时分秒
  uint32_t last_dma_num;    //四个字节，秒脉冲到达时刻对应的DMA中断的触发次数
  uint32_t dma_num_now;     //四个字节，开始寻找信号到达时间时DMA中断的触发次数
  uint16_t last_remain;     //两个字节，秒脉冲到达时刻距离下一个buffer存满剩余的uint16个数
  uint16_t arrival_time;    //两个字节，信号到达时刻
  float energy_27;          //四字节，27k能量
  uint16_t crc;             //两个字节，校验码
}data_stream1;

#pragma pack(1)
typedef struct
{
  uint8_t header[2];        //两个字节，包头
  uint8_t len;              //一个字节，长度
  uint8_t id;               //一个字节，设备id
  uint8_t order_number;     //一个字节，指令号
  uint8_t magnification;    //1个字节，放大倍数
  uint16_t threshold;       //2个字节，检测门限
  uint16_t frequency;       //2个字节，检测频率
  uint8_t ratio;            //1个字节，检测的比值门限
  uint16_t crc;             //两个字节，校验码
}para_signal_process;
#pragma pack()

#pragma pack(1)
typedef struct 
{
  uint8_t header[2];        //两个字节，包头
  uint8_t len;              //一个字节，长度
  uint8_t id;               //一个字节，设备id
  uint8_t order_number;     //一个字节，指令号
  float energy;             //四个字节，信号强度
  float energy_3;           //四个字节，作为对比的信号强度
  uint16_t crc;             //两个字节，校验码
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
