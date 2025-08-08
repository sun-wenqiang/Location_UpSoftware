/**
 * @file algorithm.h
 * @brief TDOA (Time Difference of Arrival) 目标定位算法实现
 * @author Sun Wenqiang
 * @date 2025-08-08
 * @copyright Copyright (c) 2025 Sun Wenqiang. All rights reserved.
 */
#ifndef ALGORITHM_H
#define ALGORITHM_H
#include <Eigen/Dense>
#include <vector>
#include <cmath>
#include <limits>

using namespace Eigen;

const double c_water = 1500;  // 水下声速
struct  point2D
{
    double x;
    double y;
};

double computeKx(const point2D& p);
double computeRx1(double delay, double c);
double computeXx1(const point2D& pi, const point2D& p1);
double computeYx1(const point2D& pi, const point2D& p1);

std::vector<double> solveThreeNodes(
    const std::vector<point2D>& nodes_pos,
    const std::vector<double>& delay);

std::vector<double> solveMultiNodes(
    const std::vector<point2D>& nodes_pos,
    const std::vector<double>& delay);


#endif