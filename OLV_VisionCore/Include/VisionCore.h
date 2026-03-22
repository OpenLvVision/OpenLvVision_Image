#pragma once
#include "Cast.h"
#include "LvError.h"
#include "LvImage.h"
#include <algorithm>
#include <cstdint>


extern "C" 
{

    _declspec(dllexport) void OLV_COPY(LvError::ErrorClusterPtr errorCluster,
        uint64_t srcPtr, uint64_t dstPtr,
        int32_t length);

    _declspec(dllexport) void OLV_CAST(LvError::ErrorClusterPtr errorCluster,
        LvImage::ImageClusterPtr src,
        LvImage::ImageClusterPtr dst,
        CastMethode methode, int32_t numberOfShifts,
        uint8_t bitdepthSrc, uint8_t bitdepthDst,
        float *LookupTable, int32_t LookupTableSize);
}