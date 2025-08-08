#include "algorithm.h"
#include <iostream>
// #define DEBUG_MODE

double computeKx(const point2D& p)
{
    return p.x * p.x + p.y * p.y;
}

double computeRx1(double delay, double c) {
    return delay * c;
}

double computeXx1(const point2D& pi, const point2D& p1) {
    return pi.x - p1.x;
}

double computeYx1(const point2D& pi, const point2D& p1) {
    return pi.y - p1.y;
}


/**
 * @brief 三节点TDOA定位解算
 * @param nodes_pos 节点位置数组(至少3个节点)
 * @param delay 相对于第一个节点的时延数组(单位:秒)
 * @return 目标位置[x,y]的数组
 * 
 * @note 算法原理:
 * 1. 基于双曲线相交定位原理
 * 2. 通过求解二次方程得到目标位置
 * 3. 适用于3节点二维定位场景
 * 
 * @warning 
 * - 输入节点数必须≥3
 * - 时延数组大小必须比节点数少1
 * 
 * @copyright Copyright (c) 2025 Sun Wenqiang. All rights reserved.
 */
std::vector<double> solveThreeNodes(
    const std::vector<point2D>& nodes_pos,
    const std::vector<double>& delay)
{
    if (nodes_pos.size() < 3 || delay.size() < 2)
    {
        std::cerr << "错误：输入节点数或时延数不足！\n";
        return std::vector<double>{};
    }

    #ifdef DEBUG_MODE
        std::cout << "\n=== 开始解算 ===\n";
        std::cout << "节点坐标:\n";
        for (int i = 0; i < nodes_pos.size(); ++i) {
            std::cout << "  节点" << i << ": (" << nodes_pos[i].x << ", " << nodes_pos[i].y << ")\n";
        }
        std::cout << "相对时延: [0, " << delay[1] << ", " << delay[2] << "]\n";
    #endif

    double R21 = computeRx1(delay[1], c_water);
    double R31 = computeRx1(delay[2], c_water);

    #ifdef DEBUG_MODE
    std::cout << "\n[1] 计算相对距离:\n";
    std::cout << "  R21 = " << R21 << " m\n";
    std::cout << "  R31 = " << R31 << " m\n";
    #endif

    double Y31 = computeYx1(nodes_pos[2], nodes_pos[0]);
    double Y21 = computeYx1(nodes_pos[1], nodes_pos[0]);
    double X31 = computeXx1(nodes_pos[2], nodes_pos[0]);
    double X21 = computeXx1(nodes_pos[1], nodes_pos[0]);

    #ifdef DEBUG_MODE
    std::cout << "\n[2] 计算坐标差:\n";
    std::cout << "  X21 = " << X21 << ", Y21 = " << Y21 << "\n";
    std::cout << "  X31 = " << X31 << ", Y31 = " << Y31 << "\n";
    #endif

    double K1  = computeKx(nodes_pos[0]);
    double K2  = computeKx(nodes_pos[1]);
    double K3  = computeKx(nodes_pos[2]);

    #ifdef DEBUG_MODE
    std::cout << "\n[3] 计算节点模平方:\n";
    std::cout << "  K1 = " << K1 << ", K2 = " << K2 << ", K3 = " << K3 << "\n";
    #endif

    double X1  = nodes_pos[0].x;
    double Y1  = nodes_pos[0].y;

    double q1 = (R21 * Y31 - R31 * Y21) / (X31 * Y21 - X21 * Y31);
    double p1 = ((K1 - K2) * Y31 + (K3 - K1) * Y21 +
                R21 * R21 * Y31 - R31 * R31 * Y21) /
                (2 * (X31 * Y21 - X21 * Y31));
    double q2 = (R21 * X31 - R31 * X21) / (Y31 * X21 - Y21 * X31);
    double p2 = ((K1 - K2) * X31 + (K3 - K1) * X21 +
                R21 * R21 * X31 - R31 * R31 * X21) /
                (2 * (Y31 * X21 - Y21 * X31));

    #ifdef DEBUG_MODE
    std::cout << "\n[4] 中间参数:\n";
    std::cout << "  q1 = " << q1 << ", p1 = " << p1 << "\n";
    std::cout << "  q2 = " << q2 << ", p2 = " << p2 << "\n";
    #endif


    // 解方程求 R1
    double a = q1 * q1 + q2 * q2 - 1;
    double b = 2 * (q1 * p1 + q2 * p2 - X1 * q1 - Y1 * q2);
    double cc = p1 * p1 + p2 * p2 + K1 - 2 * X1 * p1 - 2 * Y1 * p2;

    #ifdef DEBUG_MODE
    double discriminant = b*b - 4*a*cc;
    if (discriminant < 0) {
        std::cerr << "错误：无实数解 (判别式=" << discriminant << ")\n";
        return {};
    }

    std::cout << "\n[5] 二次方程系数:\n";
    std::cout << "  a = " << a << ", b = " << b << ", c = " << cc << "\n";
    #endif

    double R1 = (-b - std::sqrt(b * b - 4 * a * cc)) / (2 * a);
    double x  = q1 * R1 + p1;
    double y  = q2 * R1 + p2;

    #ifdef DEBUG_MODE
    std::cout << "\n[6] 最终解:\n";
    std::cout << "  R1 = " << R1 << "\n";
    std::cout << "  x = " << x << ", y = " << y << "\n";
    std::cout << "=== 解算结束 ===\n\n";
    #endif

    return {x, y};
}


/**
 * @brief 多节点TDOA最小二乘定位解算
 * @param nodes_pos 节点位置数组
 * @param delay 相对于第一个节点的时延数组(单位:秒)
 * @return 目标位置[x,y]的数组
 * 
 * @note 算法原理:
 * 1. 基于最小二乘法求解超定方程组
 * 2. 适用于节点数≥4的场景
 * 3. 相比三节点法具有更好的抗噪性能
 * 
 * @warning 
 * - 输入节点数必须≥4
 * - 时延数组大小必须和节点数相等
 * 
 * @copyright Copyright (c) 2025 Sun Wenqiang. All rights reserved.
 */
std::vector<double> solveMultiNodes(
    const std::vector<point2D>& nodes_pos,
    const std::vector<double>& delay)
{
    size_t n = nodes_pos.size();
    if (n < 4 || delay.size() < 4)
    {
        return std::vector<double>{};
    }

    std::vector<double> A(n-2), B(n-2), D(n-2);
    for (size_t i = 2; i < n; i++)
    {
        A[i - 2] = -(nodes_pos[i].x - nodes_pos[0].x) * 2 / c_water / delay[i]
                   + (nodes_pos[1].x - nodes_pos[0].x) * 2 / c_water / delay[1];
        B[i - 2] = -(nodes_pos[i].y - nodes_pos[0].y) * 2 / c_water / delay[i]
                   + (nodes_pos[1].y - nodes_pos[0].y) * 2 / c_water / delay[1];
        double D1 = c_water * (delay[1] - delay[i]);
        double D2 = nodes_pos[0].x * nodes_pos[0].x + nodes_pos[0].y * nodes_pos[0].y
                    - nodes_pos[i].x * nodes_pos[i].x - nodes_pos[i].y * nodes_pos[i].y;
        double D3 = nodes_pos[0].x * nodes_pos[0].x + nodes_pos[0].y * nodes_pos[0].y
                    - nodes_pos[1].x * nodes_pos[1].x - nodes_pos[1].y * nodes_pos[1].y;

        D[i - 2] = -D2 / c_water / delay[i] + D3 / c_water / delay[1] + D1;      
    }

    // 构造最小二乘 (A,B) 系统
    double sumAA = 0, sumAB = 0, sumBB = 0, sumAD = 0, sumBD = 0;
    for (size_t i = 0; i < A.size(); i++) {
        sumAA += A[i] * A[i];
        sumAB += A[i] * B[i];
        sumBB += B[i] * B[i];
        sumAD += A[i] * (-D[i]);
        sumBD += B[i] * (-D[i]);
    }

    double det = sumAA * sumBB - sumAB * sumAB;
    if (std::fabs(det) < 1e-12) {
        return {}; // 奇异矩阵
    }

    double x = (sumBB * sumAD - sumAB * sumBD) / det;
    double y = (-sumAB * sumAD + sumAA * sumBD) / det;

    return {x, y};
}