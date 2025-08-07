#include "CoordConverter.h"

CoordConverter::CoordConverter(void)
{
}

CoordConverter::~CoordConverter(void)
{
}

double CoordConverter::deg2rad(double deg) const
{
    return deg / 180 * M_PI;
}

double CoordConverter::rad2deg(double rad) const
{
    return rad / M_PI * 180;
}