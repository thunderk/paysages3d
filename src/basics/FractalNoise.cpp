#include "FractalNoise.h"

FractalNoise::FractalNoise()
{
    scaling = 1.0;
    height = 1.0;
    step_scaling = 2.0;
    step_height = 0.5;
    slope = 0.0;
    ridge = 0.0;
}

void FractalNoise::setScaling(double scaling, double height)
{
    this->scaling = scaling < 0.00000001 ? 0.0 : 1.0 / scaling;
    this->height = scaling * height;
}

void FractalNoise::setStep(double scaling_factor, double height_factor)
{
    this->step_scaling = scaling_factor < 0.00000001 ? 0.0 : 1.0 / scaling_factor;
    this->step_height = scaling_factor * height_factor;
}

void FractalNoise::setSlope(double slope_factor)
{
    this->slope = slope_factor;
}

void FractalNoise::setRidge(double ridge_factor)
{
    this->ridge = ridge_factor;
}

double FractalNoise::get1d(double detail, double x) const
{
    double current_scaling = scaling;
    double current_height = height;
    double result = 0.0;

    while (current_height >= detail)
    {
        result += getBase1d(x * current_scaling) * current_height;

        current_scaling *= step_scaling;
        current_height *= step_height;
    }

    return result;
}

double FractalNoise::get2d(double detail, double x, double y) const
{
    double current_scaling = scaling;
    double current_height = height;
    double result = 0.0;

    while (current_height >= detail)
    {
        result += getBase2d(x * current_scaling, y * current_scaling) * current_height;

        current_scaling *= step_scaling;
        current_height *= step_height;
    }

    return result;
}

double FractalNoise::get3d(double detail, double x, double y, double z) const
{
    double current_scaling = scaling;
    double current_height = height;
    double result = 0.0;

    while (current_height >= detail)
    {
        result += getBase3d(x * current_scaling, y * current_scaling, z * current_scaling) * current_height;

        current_scaling *= step_scaling;
        current_height *= step_height;
    }

    return result;
}

double FractalNoise::getBase1d(double x) const
{
    return getBase2d(x, 0.0);
}

double FractalNoise::getBase2d(double x, double y) const
{
    return getBase3d(x, y, 0.0);
}
