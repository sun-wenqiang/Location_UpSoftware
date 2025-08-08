#include "../algorithm/algorithm.h"
#include <iostream>

int main() {
    point2D node1 = {0, 0};
    point2D node2 = {-1.8343242536022,         0.891888149455403};
    point2D node3 = {0.324825134121168,        1.29820720407733};
    point2D node4 = {0.530398528893644,        2.03575598726156};

    std::vector<point2D> nodes_pos = {node1, node2, node3, node4};
    std::vector<double> delay = {0,  0.00126820877581185, 0.000142706262359215
    , 0.00051631964970499};

    std::vector<double> ans(2);
    ans = solveMultiNodes(nodes_pos, delay);
    if (ans.empty())
    {
        std::cerr<<"解算失败"<<std::endl;
    }
    else
    {
        std::cout<<"解算结果"<<"x = "<<ans[0]<<", y = "<<ans[1]<<std::endl;
    }

    return 0;
}
