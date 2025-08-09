#ifndef SIGNALPROCESS_H
#define SIGNALPROCESS_H
#include "stdint.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <algorithm>
#include <fftw3.h>
#include <vector>

#define buffer_point_num 1705

void Pcm2Vol_RemoveDC(int32_t *datain, float *dataout, uint8_t bits, uint8_t vol, size_t element_num);
uint8_t Detect_Signal(const float *data, size_t data_len, uint32_t fft_len, uint32_t fs, uint32_t f, uint16_t threshold);
void fliplr(float *data, size_t data_len);
void first_order_corr(float *data_in, float *data_out, size_t data_len_in);
void get_max_value(float *data_in, size_t data_len, float *max_val, uint32_t *max_index);
int find_first_greater_than(float *data, float value, size_t data_len);
void polyfit(int x, float *y, size_t n, float *k, float *b);
uint16_t time_arrival_detection(float *data_in, size_t len);
void move_float_data(const float *source, float *destination, size_t count);

#endif