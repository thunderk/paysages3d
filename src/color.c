#include <stdlib.h>

#include "shared/types.h"
#include "shared/constants.h"
#include "shared/functions.h"

Color COLOR_TRANSPARENT = {0.0, 0.0, 0.0, 0.0};
Color COLOR_BLACK = {0.0, 0.0, 0.0, 1.0};
Color COLOR_RED = {1.0, 0.0, 0.0, 1.0};
Color COLOR_GREEN = {0.0, 1.0, 0.0, 1.0};
Color COLOR_BLUE = {0.0, 0.0, 1.0, 1.0};
Color COLOR_WHITE = {1.0, 1.0, 1.0, 1.0};
Color COLOR_GREY = {0.5, 0.5, 0.5, 1.0};

void colorSave(Color col, FILE* f)
{
    toolsSaveDouble(f, col.r);
    toolsSaveDouble(f, col.g);
    toolsSaveDouble(f, col.b);
    toolsSaveDouble(f, col.a);
}

Color colorLoad(FILE* f)
{
    Color col;
    col.r = toolsLoadDouble(f);
    col.g = toolsLoadDouble(f);
    col.b = toolsLoadDouble(f);
    col.a = toolsLoadDouble(f);
    return col;
}

unsigned int colorTo32BitRGBA(Color* col)
{
    return (((unsigned int)(col->a * 255.0)) << 24) | (((unsigned int)(col->b * 255.0)) << 16) | (((unsigned int)(col->g * 255.0)) << 8) | ((unsigned int)(col->r * 255.0));
}

unsigned int colorTo32BitBGRA(Color* col)
{
    return (((unsigned int)(col->a * 255.0)) << 24) | (((unsigned int)(col->r * 255.0)) << 16) | (((unsigned int)(col->g * 255.0)) << 8) | ((unsigned int)(col->b * 255.0));
}

unsigned int colorTo32BitARGB(Color* col)
{
    return (((unsigned int)(col->b * 255.0)) << 24) | (((unsigned int)(col->g * 255.0)) << 16) | (((unsigned int)(col->r * 255.0)) << 8) | ((unsigned int)(col->a * 255.0));
}

unsigned int colorTo32BitABGR(Color* col)
{
    return (((unsigned int)(col->r * 255.0)) << 24) | (((unsigned int)(col->g * 255.0)) << 16) | (((unsigned int)(col->b * 255.0)) << 8) | ((unsigned int)(col->a * 255.0));
}

void colorMask(Color* base, Color* mask)
{
    double new_a;
    new_a = base->a + mask->a - (base->a * mask->a);
    base->r = (mask->r * mask->a + base->r * base->a - base->r * base->a * mask->a) / new_a;
    base->g = (mask->g * mask->a + base->g * base->a - base->g * base->a * mask->a) / new_a;
    base->b = (mask->b * mask->a + base->b * base->a - base->b * base->a * mask->a) / new_a;
    base->a = new_a;
    
    /*double mask_weight = mask->a;
    double base_weight = 1.0 - mask_weight;

    base->r = mask->r * mask_weight + base->r * base_weight;
    base->g = mask->g * mask_weight + base->g * base_weight;
    base->b = mask->b * mask_weight + base->b * base_weight;
    base->a = base->a + mask_weight * (1.0 - base->a);*/
}

double colorNormalize(Color* col)
{
    double max = colorGetValue(col);

    if (max > 1.0)
    {
        col->r /= max;
        col->g /= max;
        col->b /= max;
    }
    return max;
}

double colorGetValue(Color* col)
{
    double max;

    max = col->r;
    if (col->g > max)
    {
        max = col->g;
    }
    if (col->b > max)
    {
        max = col->b;
    }
    return max;
}

ColorGradation colorGradationCreate()
{
    ColorGradation result;

    result.nbparts = 0;

    return result;
}

int _part_compare(const void* part1, const void* part2)
{
    if (((_ColorGradationPart*)part1)->start > ((_ColorGradationPart*)part2)->start)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

void colorGradationAdd(ColorGradation* gradation, double value, Color* col)
{
    if (gradation->nbparts == MAX_COLORGRADATION_PARTS)
    {
        return;
    }
    else
    {
        gradation->parts[gradation->nbparts].start = value;
        gradation->parts[gradation->nbparts].col = *col;

        if (gradation->nbparts++ > 1)
        {
            qsort(gradation->parts, gradation->nbparts, sizeof(_ColorGradationPart), _part_compare);
        }
    }
}

void colorGradationAddRgba(ColorGradation* gradation, double value, double r, double g, double b, double a)
{
    Color col;
    col.r = r;
    col.g = g;
    col.b = b;
    col.a = a;
    colorGradationAdd(gradation, value, &col);
}

Color colorGradationGet(ColorGradation* gradation, double value)
{
    Color result;
    int i;
    double fact;

    if (gradation->nbparts == 0)
    {
        return COLOR_TRANSPARENT;
    }
    else if (gradation->nbparts == 1 || value <= gradation->parts[0].start)
    {
        return gradation->parts[0].col;
    }
    else if (value >= gradation->parts[gradation->nbparts - 1].start)
    {
        return gradation->parts[gradation->nbparts - 1].col;
    }
    else
    {
        for (i = 1; i < gradation->nbparts; i++)
        {
            if (value < gradation->parts[i].start)
            {
                fact = (value - gradation->parts[i - 1].start) / (gradation->parts[i].start - gradation->parts[i - 1].start);
                result.r = gradation->parts[i - 1].col.r + (gradation->parts[i].col.r - gradation->parts[i - 1].col.r) * fact;
                result.g = gradation->parts[i - 1].col.g + (gradation->parts[i].col.g - gradation->parts[i - 1].col.g) * fact;
                result.b = gradation->parts[i - 1].col.b + (gradation->parts[i].col.b - gradation->parts[i - 1].col.b) * fact;
                result.a = gradation->parts[i - 1].col.a + (gradation->parts[i].col.a - gradation->parts[i - 1].col.a) * fact;
                return result;
            }
        }
        return gradation->parts[gradation->nbparts - 1].col;
    }
}

