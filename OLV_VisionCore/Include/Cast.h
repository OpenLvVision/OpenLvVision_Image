#pragma once
#include "LvError.h"
#include "LvImage.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <immintrin.h>
#include <limits>
#include <type_traits>

enum CastMethode : int32_t
{ 
    UseBitDepth = 0,
    Shift = 1,
    UseLookUpTable = 2 
};

#pragma region Cast_BitDepth

MgErr Cast_BitDepth(LvImage::ImageClusterPtr src, LvImage::ImageClusterPtr dst,
    uint8_t bitdepthSrc, uint8_t bitdepthDst);

static uint8_t GetMaxBitdepth(LvImage::ImageFormat format);

#pragma endregion

#pragma region Cast_Shift

MgErr Cast_Shift(LvImage::ImageClusterPtr src, LvImage::ImageClusterPtr dst,
    int32_t numberOfShifts);

#pragma region MonoU8

void Cast_Shift_Integer_U8toU8(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride, int32_t shifts);
	
void Cast_Shift_Integer_U8toU16(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride, int32_t shifts);

void Cast_Shift_Integer_U8toI16(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride, int32_t shifts);

void Cast_Floating_U8toFloat(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride);

void Cast_Shift_Color_U8ToU32(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride, int32_t shifts);

void Cast_Shift_Color_U8ToU64(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride, int32_t shifts);

void Cast_Floating_U8ToComplex(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride);

#pragma endregion

#pragma region MonoU16

void Cast_Shift_Integer_U16toU8(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride, int32_t shifts);

void Cast_Shift_Integer_U16toU16(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride, int32_t shifts);

void Cast_Shift_Integer_U16toI16(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride, int32_t shifts);

void Cast_Floating_U16toSGL(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride);

void Cast_Shift_Color_U16ToU32(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride, int32_t shifts);

void Cast_Shift_Color_U16ToU64(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride, int32_t shifts);

void Cast_Floating_U16ToComplex(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride);

#pragma endregion

#pragma region MonoI16

void Cast_Shift_Integer_I16toU8(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride, int32_t shifts);

void Cast_Shift_Integer_I16toU16(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride, int32_t shifts);

void Cast_Shift_Integer_I16toI16(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride, int32_t shifts);

void Cast_Floating_I16toSGL(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride);

void Cast_Shift_Color_I16ToU32(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride, int32_t shifts);

void Cast_Shift_Color_I16ToU64(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride, int32_t shifts);

void Cast_Floating_I16ToComplex(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride);

#pragma endregion

#pragma region MonoSGL

void Cast_Floating_SGLtoU8(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride);

void Cast_Floating_SGLtoU16(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride);

void Cast_Floating_SGLtoI16(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride);

void Cast_Floating_SGLtoSGL(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride);

void Cast_Floating_SGLtoColorU32(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride);

void Cast_Floating_SGLtoColorU64(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride);

void Cast_Floating_SGLtoComplex(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride);

#pragma endregion

#pragma region ColorU32

void Cast_Shift_Color_U32toU8(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride, int32_t shifts);

void Cast_Shift_Color_U32toU16(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride, int32_t shifts);
	
void Cast_Shift_Color_U32toI16(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride, int32_t shifts);

void Cast_Floating_ColorU32toSGL(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride);

void Cast_Shift_Color_U32toU32(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride, int32_t shifts);

void Cast_Shift_Color_U32toU64(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride, int32_t shifts);

void Cast_Floating_ColorU32toComplex(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride);

#pragma endregion

#pragma region ColorU64

void Cast_Shift_Color_U64toU8(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride, int32_t shifts);

void Cast_Shift_Color_U64toU16(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride, int32_t shifts);

void Cast_Shift_Color_U64toI16(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride, int32_t shifts);

void Cast_Floating_ColorU64toSGL(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride);

void Cast_Shift_Color_U64toU32(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride, int32_t shifts);

void Cast_Shift_Color_U64toU64(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride, int32_t shifts);

void Cast_Floating_ColorU64toComplex(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride);

#pragma endregion

#pragma region ComplexSGL

void Cast_Floating_ComplextoU8(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride);

void Cast_Floating_ComplextoU16(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride);

void Cast_Floating_ComplextoI16(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride);

void Cast_Floating_ComplextoSGL(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride);

void Cast_Floating_ComplextoColorU32(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride);

void Cast_Floating_ComplextoColorU64(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride);

void Cast_Floating_ComplextoComplex(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride);

#pragma endregion

#pragma endregion

MgErr Cast_LUT(LvImage::ImageClusterPtr src, LvImage::ImageClusterPtr dst,
    float *lut, int32_t lutSize);