#include "pack.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>

#include "color.h"
#include "euclid.h"

struct PackStream
{
    FILE* fd;
    int write;
};

#define pack754_32(f) (pack754((f), 32, 8))
#define pack754_64(f) (pack754((f), 64, 11))
#define unpack754_32(i) (unpack754((i), 32, 8))
#define unpack754_64(i) (unpack754((i), 64, 11))

static uint64_t pack754(double f, unsigned bits, unsigned expbits)
{
    double fnorm;
    int shift;
    long long sign, exp, significand;
    unsigned significandbits = bits - expbits - 1; // -1 for sign bit

    if (f == 0.0) return 0; // get this special case out of the way

    // check sign and begin normalization
    if (f < 0) { sign = 1; fnorm = -f; }
    else { sign = 0; fnorm = f; }

    // get the normalized form of f and track the exponent
    shift = 0;
    while(fnorm >= 2.0) { fnorm /= 2.0; shift++; }
    while(fnorm < 1.0) { fnorm *= 2.0; shift--; }
    fnorm = fnorm - 1.0;

    // calculate the binary form (non-float) of the significand data
    significand = fnorm * ((1LL<<significandbits) + 0.5f);

    // get the biased exponent
    exp = shift + ((1<<(expbits-1)) - 1); // shift + bias

    // return the final answer
    return (sign<<(bits-1)) | (exp<<(bits-expbits-1)) | significand;
}

static double unpack754(uint64_t i, unsigned bits, unsigned expbits)
{
    double result;
    long long shift;
    unsigned bias;
    unsigned significandbits = bits - expbits - 1; // -1 for sign bit

    if (i == 0) return 0.0;

    // pull the significand
    result = (i&((1LL<<significandbits)-1)); // mask
    result /= (1LL<<significandbits); // convert back to float
    result += 1.0f; // add the one back on

    // deal with the exponent
    bias = (1<<(expbits-1)) - 1;
    shift = ((i>>significandbits)&((1LL<<expbits)-1)) - bias;
    while(shift > 0) { result *= 2.0; shift--; }
    while(shift < 0) { result /= 2.0; shift++; }

    // sign it
    result *= (i>>(bits-1))&1? -1.0: 1.0;

    return result;
}

PackStream* packReadFile(char* filepath)
{
    PackStream* result;
    
    result = malloc(sizeof(PackStream));
    result->fd = fopen(filepath, "rb");
    result->write = 0;
    
    return result;
}

PackStream* packWriteFile(char* filepath)
{
    PackStream* result;
    
    result = malloc(sizeof(PackStream));
    result->fd = fopen(filepath, "wb");
    result->write = 1;
    
    return result;
}

void packCloseStream(PackStream* stream)
{
    if (stream->write)
    {
        fflush(stream->fd);
    }
    fclose(stream->fd);
    free(stream);
}

void packWriteDouble(PackStream* stream, double* value)
{
    uint64_t servalue;
    
    servalue = pack754_64(*value);
    fwrite(&servalue, sizeof(uint64_t), 1, stream->fd);
}

void packReadDouble(PackStream* stream, double* value)
{
    int read;
    uint64_t servalue;
    
    read = fread(&servalue, sizeof(uint64_t), 1, stream->fd);
    assert(read == 1);
    
    *value = unpack754_64(servalue);
}

void packWriteInt(PackStream* stream, int* value)
{
    fprintf(stream->fd, "%d;", *value);
}

void packReadInt(PackStream* stream, int* value)
{
    int read;
    
    read = fscanf(stream->fd, "%d;", value);
    assert(read == 1);
}
