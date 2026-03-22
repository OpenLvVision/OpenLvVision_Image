#include "../Include/Cast.h"

#pragma region Cast_BitDepth

static uint8_t GetMaxBitdepth(LvImage::ImageFormat fmt) {
  switch (fmt) 
  {
    case LvImage::MonoU8:    return 8;
    case LvImage::MonoU16:   return 16;
    case LvImage::MonoI16:   return 15;
    case LvImage::ColorU32:  return 8;
    case LvImage::ColorU64:  return 16;
    default:                 return 0;
  }
}

MgErr Cast_BitDepth(LvImage::ImageClusterPtr src, LvImage::ImageClusterPtr dst,
    uint8_t bitdepthSrc, uint8_t bitdepthDst)
{

  bool supported =
      (src->imageFormat == LvImage::MonoU16  && dst->imageFormat == LvImage::MonoU8)   ||
      (src->imageFormat == LvImage::MonoI16  && dst->imageFormat == LvImage::MonoU8)   ||
      (src->imageFormat == LvImage::ColorU64 && dst->imageFormat == LvImage::ColorU32);

  if (!supported)
  {
      return Cast_Shift(src, dst, 0);
  }
  
  if (bitdepthSrc == 0)
  {
      bitdepthSrc = GetMaxBitdepth(src->imageFormat);
  }

  if (bitdepthDst == 0)
  {
      bitdepthDst = GetMaxBitdepth(dst->imageFormat);
  }

  int32_t numberOfShifts = static_cast<int32_t>(bitdepthSrc) - static_cast<int32_t>(bitdepthDst);

  return Cast_Shift(src, dst, numberOfShifts);
}

#pragma endregion

MgErr Cast_LUT(LvImage::ImageClusterPtr src, LvImage::ImageClusterPtr dst,
    float *lut, int32_t lutSize)
{
  if (!src || !dst || src->imagePointer == 0 || dst->imagePointer == 0)
  {
      return mgArgErr;
  }
    
  if (src->width != dst->width || src->height != dst->height) 
  {
      return mgArgErr;
  }    

  const uint8_t *srcPtrRaw = reinterpret_cast<const uint8_t *>(static_cast<uintptr_t>(src->imagePointer));
  uint8_t *dstPtrRaw = reinterpret_cast<uint8_t *>(static_cast<uintptr_t>(dst->imagePointer));

  int32_t w = src->width;
  int32_t h = src->height;
  int32_t sStr = src->lineWidth;
  int32_t dStr = dst->lineWidth;

  if (src->imageFormat == LvImage::MonoU8 && dst->imageFormat == LvImage::MonoU16) 
  {
    for (int32_t y = 0; y < h; ++y) 
    {
      const uint8_t *sRow = srcPtrRaw + (y * sStr);
      uint16_t *dRow = reinterpret_cast<uint16_t *>(dstPtrRaw + (y * dStr));

      for (int32_t x = 0; x < w; ++x) 
      {
        uint8_t val = sRow[x];
        dRow[x] = static_cast<uint16_t>(val < lutSize ? lut[val] : val);
      }
    }
    return mgNoErr;
  }
  else if (src->imageFormat == LvImage::MonoU16 && dst->imageFormat == LvImage::MonoU8) 
  {
    for (int32_t y = 0; y < h; ++y) 
    {
      const uint16_t *sRow = reinterpret_cast<const uint16_t *>(srcPtrRaw + (y * sStr));
      uint8_t *dRow = dstPtrRaw + (y * dStr);

      for (int32_t x = 0; x < w; ++x) 
      {
        uint16_t val = sRow[x];
        dRow[x] = static_cast<uint8_t>(val < lutSize ? lut[val] : val);
      }
    }
    return mgNoErr;
  }
  else if (src->imageFormat == LvImage::MonoU8 && dst->imageFormat == LvImage::MonoSGL) {
    for (int32_t y = 0; y < h; ++y) 
    {
      const uint8_t *sRow = srcPtrRaw + (y * sStr);
      float *dRow = reinterpret_cast<float *>(dstPtrRaw + (y * dStr));

      for (int32_t x = 0; x < w; ++x) 
      {
        uint8_t val = sRow[x];
        dRow[x] = static_cast<float>(val < lutSize ? lut[val] : val);
      }
    }
    return mgNoErr;
  }
  else if (src->imageFormat == LvImage::MonoU16 && dst->imageFormat == LvImage::MonoSGL) {
    for (int32_t y = 0; y < h; ++y) 
    {
      const uint16_t *sRow = reinterpret_cast<const uint16_t *>(srcPtrRaw + (y * sStr));
      float *dRow = reinterpret_cast<float *>(dstPtrRaw + (y * dStr));

      for (int32_t x = 0; x < w; ++x) 
      {
        uint16_t val = sRow[x];
        dRow[x] = static_cast<float>(val < lutSize ? lut[val] : val);
      }
    }
    return mgNoErr;
  }

  return mgNotSupported;
}

#pragma region Cast_Shift

MgErr Cast_Shift(LvImage::ImageClusterPtr src, LvImage::ImageClusterPtr dst,
    int32_t numberOfShifts) {
  // Extract the raw pointers
  const uint8_t *srcPtr = reinterpret_cast<const uint8_t *>(
      static_cast<uintptr_t>(src->imagePointer));
  uint8_t *dstPtr =
      reinterpret_cast<uint8_t *>(static_cast<uintptr_t>(dst->imagePointer));

  // Get dimensions and strides
  int32_t w = src->width;
  int32_t h = src->height;
  int32_t sStr = src->lineWidth;
  int32_t dStr = dst->lineWidth;

  switch (src->imageFormat) 
  {
    case LvImage::MonoU8:
      switch (dst->imageFormat) {
      case LvImage::MonoU8:
        Cast_Shift_Integer_U8toU8(srcPtr, dstPtr, w, h, sStr, dStr, numberOfShifts);
        return mgNoErr;
      case LvImage::MonoU16:
        Cast_Shift_Integer_U8toU16(srcPtr, dstPtr, w, h, sStr, dStr, numberOfShifts);
        return mgNoErr;
      case LvImage::MonoI16:
        Cast_Shift_Integer_U8toI16(srcPtr, dstPtr, w, h, sStr, dStr, numberOfShifts);
        return mgNoErr;
      case LvImage::MonoSGL:
        Cast_Floating_U8toFloat(srcPtr, dstPtr, w, h, sStr, dStr);
        return mgNoErr;
      case LvImage::ColorU32:
        Cast_Shift_Color_U8ToU32(srcPtr, dstPtr, w, h, sStr, dStr, numberOfShifts);
        return mgNoErr;
      case LvImage::ColorU64:
        Cast_Shift_Color_U8ToU64(srcPtr, dstPtr, w, h, sStr, dStr, numberOfShifts);
        return mgNoErr;
      case LvImage::ComplexCSGL:
        Cast_Floating_U8ToComplex(srcPtr, dstPtr, w, h, sStr, dStr);
        return mgNoErr;
      default:
        return mgNotSupported;
    }

  case LvImage::MonoU16:
    switch (dst->imageFormat) 
    {
    case LvImage::MonoU8:
      Cast_Shift_Integer_U16toU8(srcPtr, dstPtr, w, h, sStr, dStr, numberOfShifts);
      return mgNoErr;
    case LvImage::MonoU16:
      Cast_Shift_Integer_U16toU16(srcPtr, dstPtr, w, h, sStr, dStr, numberOfShifts);
      return mgNoErr;
    case LvImage::MonoI16:
      Cast_Shift_Integer_U16toI16(srcPtr, dstPtr, w, h, sStr, dStr, numberOfShifts);
      return mgNoErr;
    case LvImage::MonoSGL:
      Cast_Floating_U16toSGL(srcPtr, dstPtr, w, h, sStr, dStr);
      return mgNoErr;
    case LvImage::ColorU32:
      Cast_Shift_Color_U16ToU32(srcPtr, dstPtr, w, h, sStr, dStr, numberOfShifts);
      return mgNoErr;
    case LvImage::ColorU64:
      Cast_Shift_Color_U16ToU64(srcPtr, dstPtr, w, h, sStr, dStr, numberOfShifts);
      return mgNoErr;
    case LvImage::ComplexCSGL:
      Cast_Floating_U16ToComplex(srcPtr, dstPtr, w, h, sStr, dStr);
      return mgNoErr;
    default:
      return mgNotSupported;
    }

  case LvImage::MonoI16:
    switch (dst->imageFormat) 
    {
    case LvImage::MonoU8:
      Cast_Shift_Integer_I16toU8(srcPtr, dstPtr, w, h, sStr, dStr,
                                 numberOfShifts);
      return mgNoErr;
    case LvImage::MonoU16:
      Cast_Shift_Integer_I16toU16(srcPtr, dstPtr, w, h, sStr, dStr,
                                  numberOfShifts);
      return mgNoErr;
    case LvImage::MonoI16:
      Cast_Shift_Integer_I16toI16(srcPtr, dstPtr, w, h, sStr, dStr,
                                  numberOfShifts);
      return mgNoErr;
    case LvImage::MonoSGL:
      Cast_Floating_I16toSGL(srcPtr, dstPtr, w, h, sStr, dStr);
      return mgNoErr;
    case LvImage::ColorU32:
      Cast_Shift_Color_I16ToU32(srcPtr, dstPtr, w, h, sStr, dStr, numberOfShifts);
      return mgNoErr;
    case LvImage::ColorU64:
      Cast_Shift_Color_I16ToU64(srcPtr, dstPtr, w, h, sStr, dStr, numberOfShifts);
      return mgNoErr;
    case LvImage::ComplexCSGL:
      Cast_Floating_I16ToComplex(srcPtr, dstPtr, w, h, sStr, dStr);
      return mgNoErr;
    default:
      return mgNotSupported;
    }

  case LvImage::MonoSGL:
    switch (dst->imageFormat) 
    {
    case LvImage::MonoU8:
      Cast_Floating_SGLtoU8(srcPtr, dstPtr, w, h, sStr, dStr);
      return mgNoErr;
    case LvImage::MonoU16:
      Cast_Floating_SGLtoU16(srcPtr, dstPtr, w, h, sStr, dStr);
      return mgNoErr;
    case LvImage::MonoI16:
      Cast_Floating_SGLtoI16(srcPtr, dstPtr, w, h, sStr, dStr);
      return mgNoErr;
    case LvImage::MonoSGL:
      Cast_Floating_SGLtoSGL(srcPtr, dstPtr, w, h, sStr, dStr);
      return mgNoErr;
    case LvImage::ColorU32:
      Cast_Floating_SGLtoColorU32(srcPtr, dstPtr, w, h, sStr, dStr);
      return mgNoErr;
    case LvImage::ColorU64:
      Cast_Floating_SGLtoColorU64(srcPtr, dstPtr, w, h, sStr, dStr);
      return mgNoErr;
    case LvImage::ComplexCSGL:
      Cast_Floating_SGLtoComplex(srcPtr, dstPtr, w, h, sStr, dStr);
      return mgNoErr;
    default:
      return mgNotSupported;
    }

  case LvImage::ColorU32:
    switch (dst->imageFormat) 
    {
    case LvImage::MonoU8:
      Cast_Shift_Color_U32toU8(srcPtr, dstPtr, w, h, sStr, dStr, numberOfShifts);
      return mgNoErr;
    case LvImage::MonoU16:
      Cast_Shift_Color_U32toU16(srcPtr, dstPtr, w, h, sStr, dStr, numberOfShifts);
      return mgNoErr;
    case LvImage::MonoI16:
      Cast_Shift_Color_U32toI16(srcPtr, dstPtr, w, h, sStr, dStr, numberOfShifts);
      return mgNoErr;
    case LvImage::MonoSGL:
      Cast_Floating_ColorU32toSGL(srcPtr, dstPtr, w, h, sStr, dStr);
      return mgNoErr;
    case LvImage::ColorU32:
      Cast_Shift_Color_U32toU32(srcPtr, dstPtr, w, h, sStr, dStr, numberOfShifts);
      return mgNoErr;
    case LvImage::ColorU64:
      Cast_Shift_Color_U32toU64(srcPtr, dstPtr, w, h, sStr, dStr, numberOfShifts);
      return mgNoErr;
    case LvImage::ComplexCSGL:
      Cast_Floating_ColorU32toComplex(srcPtr, dstPtr, w, h, sStr, dStr);
      return mgNoErr;
    default:
      return mgNotSupported;
    }

  case LvImage::ColorU64:
    switch (dst->imageFormat) {
    case LvImage::MonoU8:
      Cast_Shift_Color_U64toU8(srcPtr, dstPtr, w, h, sStr, dStr, numberOfShifts);
      return mgNoErr;
    case LvImage::MonoU16:
      Cast_Shift_Color_U64toU16(srcPtr, dstPtr, w, h, sStr, dStr, numberOfShifts);
      return mgNoErr;
    case LvImage::MonoI16:
      Cast_Shift_Color_U64toI16(srcPtr, dstPtr, w, h, sStr, dStr, numberOfShifts);
      return mgNoErr;
    case LvImage::MonoSGL:
      Cast_Floating_ColorU64toSGL(srcPtr, dstPtr, w, h, sStr, dStr);
      return mgNoErr;
    case LvImage::ColorU32:
      Cast_Shift_Color_U64toU32(srcPtr, dstPtr, w, h, sStr, dStr, numberOfShifts);
      return mgNoErr;
    case LvImage::ColorU64:
      Cast_Shift_Color_U64toU64(srcPtr, dstPtr, w, h, sStr, dStr, numberOfShifts);
      return mgNoErr;
    case LvImage::ComplexCSGL:
      Cast_Floating_ColorU64toComplex(srcPtr, dstPtr, w, h, sStr, dStr);
      return mgNoErr;
    default:
      return mgNotSupported;
    }

  case LvImage::ComplexCSGL:
    switch (dst->imageFormat) 
    {
    case LvImage::MonoU8:
      Cast_Floating_ComplextoU8(srcPtr, dstPtr, w, h, sStr, dStr);
      return mgNoErr;
    case LvImage::MonoU16:
      Cast_Floating_ComplextoU16(srcPtr, dstPtr, w, h, sStr, dStr);
      return mgNoErr;
    case LvImage::MonoI16:
      Cast_Floating_ComplextoI16(srcPtr, dstPtr, w, h, sStr, dStr);
      return mgNoErr;
    case LvImage::MonoSGL:
      Cast_Floating_ComplextoSGL(srcPtr, dstPtr, w, h, sStr, dStr);
      return mgNoErr;
    case LvImage::ColorU32:
      Cast_Floating_ComplextoColorU32(srcPtr, dstPtr, w, h, sStr, dStr);
      return mgNoErr;
    case LvImage::ColorU64:
      Cast_Floating_ComplextoColorU64(srcPtr, dstPtr, w, h, sStr, dStr);
      return mgNoErr;
    case LvImage::ComplexCSGL:
      Cast_Floating_ComplextoComplex(srcPtr, dstPtr, w, h, sStr, dStr);
      return mgNoErr;
    default:
      return mgNotSupported;
    }

  default:
    return mgNotSupported;
  }
}

#pragma endregion

#pragma region MonoU8

void Cast_Shift_Integer_U8toU8(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride,
    int32_t shifts)
{

  // Shifts >= 8 result in zeros
  if (shifts >= 8) {
    for (int32_t y = 0; y < height; ++y) 
    {
      std::memset(dstBase + (y * dstStride), 0, width);
    }
    return;
  }

  // Calculate mask to prevent crossing byte boundaries
  uint8_t maskByte = 0xFF >> shifts;

  // BROADCAST MASK: Duplicate mask across 32 bytes
  __m256i v_mask = _mm256_set1_epi8(maskByte);

  for (int32_t y = 0; y < height; ++y) 
  {
    const uint8_t *srcRow = srcBase + (y * srcStride);
    uint8_t *dstRow = dstBase + (y * dstStride);

    int32_t x = 0;
    for (; x <= width - 32; x += 32) 
    {
      // READ MEMORY: Load 32 bytes
      __m256i v_in = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(srcRow + x));

      // BITWISE SHIFT: Shift 16-bit array values right
      __m256i v_shifted = _mm256_srli_epi16(v_in, shifts);

      // APPLY MASK: Clean neighboring bytes
      __m256i v_final = _mm256_and_si256(v_shifted, v_mask);

      // STORE RESULT: Store 32 bytes to memory
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(dstRow + x), v_final);
    }

    // Cleanup remaining
    for (; x < width; ++x) {
      dstRow[x] = srcRow[x] >> shifts;
    }
  }
}

void Cast_Shift_Integer_U8toU16(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride,
    int32_t shifts)
{

  // Shifts >= 8 result in zeros
  if (shifts >= 8) {
    for (int32_t y = 0; y < height; ++y) 
    {
      std::memset(dstBase + (y * dstStride), 0, width * sizeof(uint16_t));
    }
    return;
  }

  for (int32_t y = 0; y < height; ++y) 
  {
    const uint8_t *srcRow = srcBase + (y * srcStride);
    uint16_t *dstRow = reinterpret_cast<uint16_t *>(dstBase + (y * dstStride));

    int32_t x = 0;
    for (; x <= width - 16; x += 16)
    {
      // READ MEMORY: Load 16 bytes
      __m128i v_in_8 = _mm_loadu_si128(reinterpret_cast<const __m128i *>(srcRow + x));

      // HARDWARE EXPAND: Expand 16 bytes to 16-bit integers
      __m256i v_in_16 = _mm256_cvtepu8_epi16(v_in_8);

      // BITWISE SHIFT: Shift 16-bit array values right
      __m256i v_shifted = _mm256_srli_epi16(v_in_16, shifts);

      // STORE RESULT: Store 16 bytes to memory
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(dstRow + x), v_shifted);
    }

    // Cleanup remaining
    for (; x < width; ++x) 
    {
      dstRow[x] = static_cast<uint16_t>(srcRow[x] >> shifts);
    }
  }
}

void Cast_Shift_Integer_U8toI16(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride,
    int32_t shifts)
{

  // Shifts >= 8 result in zeros
  if (shifts >= 8) {
    for (int32_t y = 0; y < height; ++y) 
    {
      std::memset(dstBase + (y * dstStride), 0, width * sizeof(int16_t));
    }
    return;
  }

  for (int32_t y = 0; y < height; ++y) 
  {
    const uint8_t *srcRow = srcBase + (y * srcStride);
    int16_t *dstRow = reinterpret_cast<int16_t *>(dstBase + (y * dstStride));

    int32_t x = 0;
    for (; x <= width - 16; x += 16) {
      // READ MEMORY: Load 16 bytes
      __m128i v_in_8 = _mm_loadu_si128(reinterpret_cast<const __m128i *>(srcRow + x));

      // HARDWARE EXPAND: Expand 16 bytes to 16-bit integers
      __m256i v_in_16 = _mm256_cvtepu8_epi16(v_in_8);

      // BITWISE SHIFT: Shift 16-bit array values right
      __m256i v_shifted = _mm256_srli_epi16(v_in_16, shifts);

      // STORE RESULT: Store 16 bytes to memory
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(dstRow + x), v_shifted);
    }

    // Cleanup remaining
    for (; x < width; ++x) 
    {
      dstRow[x] = static_cast<int16_t>(srcRow[x] >> shifts);
    }
  }
}

void Cast_Floating_U8toFloat(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride)
{
  for (int32_t y = 0; y < height; ++y) {
    const uint8_t *srcRow = srcBase + (y * srcStride);
    float *dstRow = reinterpret_cast<float *>(dstBase + (y * dstStride));

    int32_t x = 0;
    for (; x <= width - 8; x += 8) 
    {
      // READ MEMORY: Load 8 bytes
      __m128i v_in_8 = _mm_loadl_epi64(reinterpret_cast<const __m128i *>(srcRow + x));

      // HARDWARE EXPAND: Expand 8 bytes to 32-bit integers
      __m256i v_in_32 = _mm256_cvtepu8_epi32(v_in_8);

      // CAST: Convert 32-bit integers to single-precision float
      __m256 v_float = _mm256_cvtepi32_ps(v_in_32);

      // STORE RESULT: Store 8 floats to memory
      _mm256_storeu_ps(dstRow + x, v_float);
    }

    // Cleanup remaining
    for (; x < width; ++x) 
    {
      dstRow[x] = static_cast<float>(srcRow[x]);
    }
  }
}

void Cast_Shift_Color_U8ToU32(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride,
    int32_t shifts)
{

  // Shifts >= 8 result in zeros
  if (shifts >= 8) 
  {
    for (int32_t y = 0; y < height; ++y) 
    {
      LvImage::Pixel_ColorU32 *dstRow = reinterpret_cast<LvImage::Pixel_ColorU32 *>(dstBase + (y * dstStride));

      for (int32_t x = 0; x < width; ++x) 
      {
        dstRow[x] = {0, 0, 0, 0};
      }
    }
    return;
  }

  // Expand 1 byte of Mono to 4 bytes of Color
  // Mono maps B=mono G=mono R=mono
  // LabVIEW expects Alpha=0
  __m256i shuffleMask = _mm256_setr_epi8(
      0, 0, 0, 0x80,     // Pixel 0 mapped B,G,R=Index 0 | Alpha zeroed
      4, 4, 4, 0x80,     // Pixel 1 mapped B,G,R=Index 4
      8, 8, 8, 0x80,     // Pixel 2 mapped B,G,R=Index 8
      12, 12, 12, 0x80,  // Pixel 3 mapped B,G,R=Index 12
      0, 0, 0, 0x80,     // Pixel 4 (Lane 1 starts)
      4, 4, 4, 0x80,     // Pixel 5
      8, 8, 8, 0x80,     // Pixel 6
      12, 12, 12, 0x80); // Pixel 7

  for (int32_t y = 0; y < height; ++y)
  {
    const uint8_t *srcRow = srcBase + (y * srcStride);
    LvImage::Pixel_ColorU32 *dstRow = reinterpret_cast<LvImage::Pixel_ColorU32 *>(dstBase + (y * dstStride));

    int32_t x = 0;
    for (; x <= width - 8; x += 8) 
    {
      // READ MEMORY: Load 8 bytes
      __m128i v_in_8 = _mm_loadl_epi64(reinterpret_cast<const __m128i *>(srcRow + x));

      // HARDWARE EXPAND: Expand 8 bytes to 32-bit integers
      __m256i v_in_32 = _mm256_cvtepu8_epi32(v_in_8);

      // BITWISE SHIFT: Shift 32-bit array values right
      __m256i v_shifted = _mm256_srli_epi32(v_in_32, shifts);

      // MEMORY SHUFFLE: Shuffle channels to form BGR
      __m256i v_final = _mm256_shuffle_epi8(v_shifted, shuffleMask);

      // STORE RESULT: Store 8 bytes to memory
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(dstRow + x), v_final);
    }

    // Cleanup remaining
    for (; x < width; ++x) 
    {
      uint8_t val = srcRow[x] >> shifts;
      dstRow[x] = {val, val, val, 0};
    }
  }
}

void Cast_Shift_Color_U8ToU64(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride,
    int32_t shifts)
{

  // Shifts >= 8 result in zeros
  if (shifts >= 8) {
    for (int32_t y = 0; y < height; ++y) {
      LvImage::Pixel_ColorU64 *dstRow = reinterpret_cast<LvImage::Pixel_ColorU64 *>(dstBase + (y * dstStride));
      for (int32_t x = 0; x < width; ++x) {
        dstRow[x] = {0, 0, 0, 0};
      }
    }
    return;
  }

  // Expand 1 byte of Mono to 8 bytes of Color
  // First byte maps lower half of U16 with top half zeroed
  // LabVIEW expects Alpha=0
  __m256i shuffleMask = _mm256_setr_epi8(
      0, 0x80, 0, 0x80, 0, 0x80, 0x80, 0x80, // Pixel 0
      8, 0x80, 8, 0x80, 8, 0x80, 0x80, 0x80, // Pixel 1
      0, 0x80, 0, 0x80, 0, 0x80, 0x80, 0x80, // Pixel 2 (Lane 1 begins)
      8, 0x80, 8, 0x80, 8, 0x80, 0x80, 0x80);// Pixel 3

  for (int32_t y = 0; y < height; ++y) {
    const uint8_t *srcRow = srcBase + (y * srcStride);
    LvImage::Pixel_ColorU64 *dstRow = reinterpret_cast<LvImage::Pixel_ColorU64 *>(dstBase + (y * dstStride));

    int32_t x = 0;
    for (; x <= width - 4; x += 4) 
    {
      // READ MEMORY: Load 4 bytes
      int32_t chunk;
      std::memcpy(&chunk, srcRow + x, 4);
      __m128i v_in_8 = _mm_cvtsi32_si128(chunk);

      // HARDWARE EXPAND: Expand 4 bytes to 64-bit integers
      __m256i v_in_64 = _mm256_cvtepu8_epi64(v_in_8);

      // BITWISE SHIFT: Shift 64-bit array values right
      __m256i v_shifted = _mm256_srli_epi64(v_in_64, shifts);

      // MEMORY SHUFFLE: Shuffle channels
      __m256i v_final = _mm256_shuffle_epi8(v_shifted, shuffleMask);

      // STORE RESULT: Store 4 bytes to memory
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(dstRow + x), v_final);
    }

    // Cleanup remaining
    for (; x < width; ++x) 
    {
      uint16_t val = static_cast<uint16_t>(srcRow[x] >> shifts);
      dstRow[x] = {val, val, val, 0};
    }
  }
}

void Cast_Floating_U8ToComplex(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride)
{
  for (int32_t y = 0; y < height; ++y) 
  {
    const uint8_t *srcRow = srcBase + (y * srcStride);
    LvImage::Pixel_ComplexSGL *dstRow = reinterpret_cast<LvImage::Pixel_ComplexSGL *>(dstBase + (y * dstStride));

    int32_t x = 0;
    for (; x <= width - 8; x += 8) 
    {
      // READ MEMORY: Load 8 bytes
      __m128i v_in_8 = _mm_loadl_epi64(reinterpret_cast<const __m128i *>(srcRow + x));

      // HARDWARE EXPAND: Expand 8 bytes to 32-bit integers
      __m256i v_in_32 = _mm256_cvtepu8_epi32(v_in_8);

      // CAST: Convert 32-bit integers to single-precision float
      __m256 v_float = _mm256_cvtepi32_ps(v_in_32);

      // COMPLEX ZERO: Zero array for imaginary components
      __m256 v_zero = _mm256_setzero_ps();

      // UNPACK AND INTERLEAVE: Interleave real floats with imaginary zeros
      __m256 v_lo = _mm256_unpacklo_ps(v_float, v_zero);
      __m256 v_hi = _mm256_unpackhi_ps(v_float, v_zero);

      // EXTRACT MEMORY BANKS: Extract portions for alignment serialization
      __m128 lo_0 = _mm256_castps256_ps128(v_lo);
      __m128 lo_1 = _mm256_extractf128_ps(v_lo, 1);
      __m128 hi_0 = _mm256_castps256_ps128(v_hi);
      __m128 hi_1 = _mm256_extractf128_ps(v_hi, 1);

      // STORE RESULT: Store 8 items to memory
      _mm_storeu_ps(reinterpret_cast<float *>(dstRow + x), lo_0);
      _mm_storeu_ps(reinterpret_cast<float *>(dstRow + x + 2), hi_0);
      _mm_storeu_ps(reinterpret_cast<float *>(dstRow + x + 4), lo_1);
      _mm_storeu_ps(reinterpret_cast<float *>(dstRow + x + 6), hi_1);
    }

    // Cleanup remaining
    for (; x < width; ++x) 
    {
      dstRow[x] = {static_cast<float>(srcRow[x]), 0.0f};
    }
  }
}

#pragma endregion

#pragma region MonoU16

void Cast_Shift_Integer_U16toU8(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride,
    int32_t shifts) 
{
  for (int32_t y = 0; y < height; ++y) {
    const uint16_t *srcRow = reinterpret_cast<const uint16_t *>(srcBase + (y * srcStride));
    uint8_t *dstRow = dstBase + (y * dstStride);

    int32_t x = 0;
    for (; x <= width - 32; x += 32) {
      // READ MEMORY: Load 32 16-bit integers across two registers
      __m256i v_in_1 = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(srcRow + x));
      __m256i v_in_2 = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(srcRow + x + 16));

      // BITWISE SHIFT: Shift 16-bit array values right
      __m256i v_shift_1 = _mm256_srli_epi16(v_in_1, shifts);
      __m256i v_shift_2 = _mm256_srli_epi16(v_in_2, shifts);

      // PACK SATURATE: Pack 16-bit to 8-bit with saturation at 255
      __m256i v_pack = _mm256_packus_epi16(v_shift_1, v_shift_2);

      // FIX LANE ORDER: Correct lane-wise pack ordering with permute
      __m256i v_perm = _mm256_permute4x64_epi64(v_pack, _MM_SHUFFLE(3, 1, 2, 0));

      // STORE RESULT: Store 32 bytes to memory
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(dstRow + x), v_perm);
    }
    // Cleanup remaining
    for (; x < width; ++x) {
      int32_t val = static_cast<int32_t>(srcRow[x]) >> shifts;
      dstRow[x] = static_cast<uint8_t>(std::min(std::max(val, 0), 255));
    }
  }
}

void Cast_Shift_Integer_U16toU16(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride,
    int32_t shifts) 
{
  // Shifts >= 16 result in zeros
  if (shifts >= 16) {
    for (int32_t y = 0; y < height; ++y) {
      std::memset(dstBase + (y * dstStride), 0, width * sizeof(uint16_t));
    }
    return;
  }
  for (int32_t y = 0; y < height; ++y) {
    const uint16_t *srcRow = reinterpret_cast<const uint16_t *>(srcBase + (y * srcStride));
    uint16_t *dstRow = reinterpret_cast<uint16_t *>(dstBase + (y * dstStride));
    
    int32_t x = 0;
    for (; x <= width - 16; x += 16) {
      // READ MEMORY: Load 16 16-bit integers
      __m256i v_in = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(srcRow + x));

      // BITWISE SHIFT: Shift 16-bit array values right
      __m256i v_shifted = _mm256_srli_epi16(v_in, shifts);

      // STORE RESULT: Store 16 values to memory
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(dstRow + x), v_shifted);
    }
    // Cleanup remaining
    for (; x < width; ++x) {
      dstRow[x] = srcRow[x] >> shifts;
    }
  }
}

void Cast_Shift_Integer_U16toI16(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride,
    int32_t shifts) {
  // Shifts >= 16 result in zeros
  if (shifts >= 16) {
    for (int32_t y = 0; y < height; ++y) {
      std::memset(dstBase + (y * dstStride), 0, width * sizeof(int16_t));
    }
    return;
  }
  
  // BROADCAST LIMIT: Maximum positive I16 value
  __m256i v_max_i16 = _mm256_set1_epi16(32767);
  
  for (int32_t y = 0; y < height; ++y) {
    const uint16_t *srcRow = reinterpret_cast<const uint16_t *>(srcBase + (y * srcStride));
    int16_t *dstRow = reinterpret_cast<int16_t *>(dstBase + (y * dstStride));
    
    int32_t x = 0;
    for (; x <= width - 16; x += 16) {
      // READ MEMORY: Load 16 16-bit integers
      __m256i v_in = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(srcRow + x));

      // BITWISE SHIFT: Shift 16-bit array values right
      __m256i v_shifted = _mm256_srli_epi16(v_in, shifts);

      // CLAMP LIMITS: Saturate against maximum signed boundary
      __m256i v_clamped = _mm256_min_epu16(v_shifted, v_max_i16);

      // STORE RESULT: Store 16 values to memory
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(dstRow + x), v_clamped);
    }
    // Cleanup remaining
    for (; x < width; ++x) {
      dstRow[x] = static_cast<int16_t>(std::min(static_cast<int32_t>(srcRow[x] >> shifts), 32767));
    }
  }
}

void Cast_Floating_U16toSGL(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride) 
{
  for (int32_t y = 0; y < height; ++y) {
    const uint16_t *srcRow = reinterpret_cast<const uint16_t *>(srcBase + (y * srcStride));
    float *dstRow = reinterpret_cast<float *>(dstBase + (y * dstStride));
    
    int32_t x = 0;
    for (; x <= width - 8; x += 8) {
      // READ MEMORY: Load 8 16-bit integers
      __m128i v_in_16 = _mm_loadu_si128(reinterpret_cast<const __m128i *>(srcRow + x));

      // HARDWARE EXPAND: Expand 16-bit integers to 32-bit integers
      __m256i v_in_32 = _mm256_cvtepu16_epi32(v_in_16);

      // CAST: Convert 32-bit integers to single-precision float
      __m256 v_float = _mm256_cvtepi32_ps(v_in_32);

      // STORE RESULT: Store 8 floats to memory
      _mm256_storeu_ps(dstRow + x, v_float);
    }
    // Cleanup remaining
    for (; x < width; ++x) {
      dstRow[x] = static_cast<float>(srcRow[x]);
    }
  }
}

void Cast_Shift_Color_U16ToU32(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride,
    int32_t shifts) {
  // Shuffle mask to map lower 8 bits of components
  // LabVIEW expects Alpha=0
  __m256i shuffleMask = _mm256_setr_epi8(
      0, 0, 0, 0x80, // Pixel 0: B,G,R mapped | Alpha zeroed
      4, 4, 4, 0x80, // Pixel 1
      8, 8, 8, 0x80, // Pixel 2
      12, 12, 12, 0x80, // Pixel 3
      0, 0, 0, 0x80, // Pixel 4 (Lane 1 starts)
      4, 4, 4, 0x80, // Pixel 5
      8, 8, 8, 0x80, // Pixel 6
      12, 12, 12, 0x80); // Pixel 7

  // BROADCAST LIMIT: Cap RGB at 8-bit maximum (255)
  __m256i max_color_limit = _mm256_set1_epi32(255);

  for (int32_t y = 0; y < height; ++y) {
    const uint16_t *srcRow = reinterpret_cast<const uint16_t *>(srcBase + (y * srcStride));
    LvImage::Pixel_ColorU32 *dstRow = reinterpret_cast<LvImage::Pixel_ColorU32 *>(dstBase + (y * dstStride));

    int32_t x = 0;
    for (; x <= width - 8; x += 8) {
      // READ MEMORY: Load 8 16-bit integers
      __m128i v_in_16 = _mm_loadu_si128(reinterpret_cast<const __m128i *>(srcRow + x));

      // HARDWARE EXPAND: Expand 16-bit integers to 32-bit integers
      __m256i v_in_32 = _mm256_cvtepu16_epi32(v_in_16);

      // BITWISE SHIFT: Shift 32-bit array values right
      __m256i v_shifted = _mm256_srli_epi32(v_in_32, shifts);

      // CLAMP LIMITS: Saturate against 8-bit maximum (255)
      __m256i v_clamped = _mm256_min_epu32(v_shifted, max_color_limit);

      // MEMORY SHUFFLE: Shuffle channels to form BGR
      __m256i v_final = _mm256_shuffle_epi8(v_clamped, shuffleMask);

      // STORE RESULT: Store 8 pixels to memory
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(dstRow + x), v_final);
    }
    // Cleanup remaining
    for (; x < width; ++x) {
      uint8_t val = static_cast<uint8_t>(std::min(static_cast<uint32_t>(srcRow[x] >> shifts), 255u));
      dstRow[x] = {val, val, val, 0};
    }
  }
}

void Cast_Shift_Color_U16ToU64(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride,
    int32_t shifts) 
{
  // Shifts >= 16 result in zeros
  if (shifts >= 16) {
    for (int32_t y = 0; y < height; ++y) {
      LvImage::Pixel_ColorU64 *dstRow = reinterpret_cast<LvImage::Pixel_ColorU64 *>(dstBase + (y * dstStride));
      for (int32_t x = 0; x < width; ++x) {
        dstRow[x] = {0, 0, 0, 0};
      }
    }
    return;
  }

  // Shuffle mask to duplicate 16-bit spans
  // LabVIEW expects Alpha=0
  __m256i shuffleMask = _mm256_setr_epi8(
      0, 1, 0, 1, 0, 1, 0x80, 0x80, // Pixel 0: B,G,R mapped | Alpha zeroed
      8, 9, 8, 9, 8, 9, 0x80, 0x80, // Pixel 1
      0, 1, 0, 1, 0, 1, 0x80, 0x80, // Pixel 2 (Lane 1 starts)
      8, 9, 8, 9, 8, 9, 0x80, 0x80); // Pixel 3

  for (int32_t y = 0; y < height; ++y) {
    const uint16_t *srcRow = reinterpret_cast<const uint16_t *>(srcBase + (y * srcStride));
    LvImage::Pixel_ColorU64 *dstRow = reinterpret_cast<LvImage::Pixel_ColorU64 *>(dstBase + (y * dstStride));

    int32_t x = 0;
    for (; x <= width - 4; x += 4) {
      // READ MEMORY: Load 4 16-bit integers
      __m128i v_in_16 = _mm_loadl_epi64(reinterpret_cast<const __m128i *>(srcRow + x));

      // HARDWARE EXPAND: Expand 16-bit integers to 64-bit integers
      __m256i v_in_64 = _mm256_cvtepu16_epi64(v_in_16);

      // BITWISE SHIFT: Shift 64-bit array values right
      __m256i v_shifted = _mm256_srli_epi64(v_in_64, shifts);

      // MEMORY SHUFFLE: Shuffle channels to form BGR
      __m256i v_final = _mm256_shuffle_epi8(v_shifted, shuffleMask);

      // STORE RESULT: Store 4 pixels to memory
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(dstRow + x), v_final);
    }
    // Cleanup remaining
    for (; x < width; ++x) {
      uint16_t val = static_cast<uint16_t>(srcRow[x] >> shifts);
      dstRow[x] = {val, val, val, 0};
    }
  }
}

void Cast_Floating_U16ToComplex(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride) 
{
  for (int32_t y = 0; y < height; ++y) {
    const uint16_t *srcRow = reinterpret_cast<const uint16_t *>(srcBase + (y * srcStride));
    LvImage::Pixel_ComplexSGL *dstRow = reinterpret_cast<LvImage::Pixel_ComplexSGL *>(dstBase + (y * dstStride));
    
    int32_t x = 0;
    for (; x <= width - 8; x += 8) {
      // READ MEMORY: Load 8 16-bit integers
      __m128i v_in_16 = _mm_loadu_si128(reinterpret_cast<const __m128i *>(srcRow + x));

      // HARDWARE EXPAND: Expand 16-bit integers to 32-bit integers
      __m256i v_in_32 = _mm256_cvtepu16_epi32(v_in_16);

      // CAST: Convert 32-bit integers to single-precision float
      __m256 v_float = _mm256_cvtepi32_ps(v_in_32);

      // COMPLEX ZERO: Zero array for imaginary components
      __m256 v_zero = _mm256_setzero_ps();

      // UNPACK AND INTERLEAVE: Interleave real floats with imaginary zeros
      __m256 v_lo = _mm256_unpacklo_ps(v_float, v_zero);
      __m256 v_hi = _mm256_unpackhi_ps(v_float, v_zero);

      // EXTRACT MEMORY BANKS: Extract portions for alignment serialization
      __m128 lo_0 = _mm256_castps256_ps128(v_lo);
      __m128 lo_1 = _mm256_extractf128_ps(v_lo, 1);
      __m128 hi_0 = _mm256_castps256_ps128(v_hi);
      __m128 hi_1 = _mm256_extractf128_ps(v_hi, 1);

      // STORE RESULT: Store 8 complex values to memory
      _mm_storeu_ps(reinterpret_cast<float *>(dstRow + x), lo_0);
      _mm_storeu_ps(reinterpret_cast<float *>(dstRow + x + 2), hi_0);
      _mm_storeu_ps(reinterpret_cast<float *>(dstRow + x + 4), lo_1);
      _mm_storeu_ps(reinterpret_cast<float *>(dstRow + x + 6), hi_1);
    }
    // Cleanup remaining
    for (; x < width; ++x) {
      dstRow[x] = {static_cast<float>(srcRow[x]), 0.0f};
    }
  }
}
#pragma endregion

#pragma region MonoI16

void Cast_Shift_Integer_I16toU8(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride,
    int32_t shifts) 
{
  // BROADCAST ZERO: Zero vector for clamping negatives
  __m256i v_zero = _mm256_setzero_si256();
  for (int32_t y = 0; y < height; ++y) {
    const int16_t *srcRow =
        reinterpret_cast<const int16_t *>(srcBase + (y * srcStride));
    uint8_t *dstRow = dstBase + (y * dstStride);

    int32_t x = 0;
    for (; x <= width - 32; x += 32) {
      // READ MEMORY: Load 32 16-bit integers across two registers
      __m256i v_in_1 =
          _mm256_loadu_si256(reinterpret_cast<const __m256i *>(srcRow + x));
      __m256i v_in_2 = _mm256_loadu_si256(
          reinterpret_cast<const __m256i *>(srcRow + x + 16));

      // ARITHMETIC SHIFT: Shift 16-bit signed values right
      __m256i v_shift_1 = _mm256_srai_epi16(v_in_1, shifts);
      __m256i v_shift_2 = _mm256_srai_epi16(v_in_2, shifts);

      // CLAMP LIMITS: Clamp negatives to zero before unsigned packing
      v_shift_1 = _mm256_max_epi16(v_shift_1, v_zero);
      v_shift_2 = _mm256_max_epi16(v_shift_2, v_zero);

      // PACK SATURATE: Pack 16-bit to 8-bit with saturation at 255
      __m256i v_pack = _mm256_packus_epi16(v_shift_1, v_shift_2);

      // FIX LANE ORDER: Correct lane-wise pack ordering with permute
      __m256i v_perm =
          _mm256_permute4x64_epi64(v_pack, _MM_SHUFFLE(3, 1, 2, 0));

      // STORE RESULT: Store 32 bytes to memory
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(dstRow + x), v_perm);
    }

    // Cleanup remaining
    for (; x < width; ++x) {
      int32_t val = static_cast<int32_t>(srcRow[x]) >> shifts;
      dstRow[x] = static_cast<uint8_t>(std::min(std::max(val, 0), 255));
    }
  }
}

void Cast_Shift_Integer_I16toU16(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride,
    int32_t shifts) 
{
  // Shifts >= 16 result in zeros
  if (shifts >= 16) {
    for (int32_t y = 0; y < height; ++y) {
      std::memset(dstBase + (y * dstStride), 0, width * sizeof(uint16_t));
    }
    return;
  }

  // BROADCAST ZERO: Zero vector for clamping negatives
  __m256i v_zero = _mm256_setzero_si256();
  for (int32_t y = 0; y < height; ++y) {
    const int16_t *srcRow =
        reinterpret_cast<const int16_t *>(srcBase + (y * srcStride));
    uint16_t *dstRow = reinterpret_cast<uint16_t *>(dstBase + (y * dstStride));
    int32_t x = 0;
    for (; x <= width - 16; x += 16) {
      // READ MEMORY: Load 16 16-bit integers
      __m256i v_in =
          _mm256_loadu_si256(reinterpret_cast<const __m256i *>(srcRow + x));

      // ARITHMETIC SHIFT: Shift 16-bit signed values right
      __m256i v_shifted = _mm256_srai_epi16(v_in, shifts);

      // CLAMP LIMITS: Clamp negatives to zero
      __m256i v_clamped = _mm256_max_epi16(v_shifted, v_zero);

      // STORE RESULT: Store 16 values to memory
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(dstRow + x), v_clamped);
    }
    // Cleanup remaining
    for (; x < width; ++x) {
      dstRow[x] = static_cast<uint16_t>(
          std::max(0, static_cast<int32_t>(srcRow[x] >> shifts)));
    }
  }
}

void Cast_Shift_Integer_I16toI16(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride,
    int32_t shifts) 
{
  // Shifts >= 16 with signed values collapse to -1 or 0
  if (shifts >= 16) {
    for (int32_t y = 0; y < height; ++y) {
      const int16_t *srcRow =
          reinterpret_cast<const int16_t *>(srcBase + (y * srcStride));
      int16_t *dstRow = reinterpret_cast<int16_t *>(dstBase + (y * dstStride));
      for (int32_t x = 0; x < width; ++x) {
        dstRow[x] =
            static_cast<int16_t>(static_cast<int32_t>(srcRow[x] >> shifts));
      }
    }
    return;
  }
  for (int32_t y = 0; y < height; ++y) {
    const int16_t *srcRow =
        reinterpret_cast<const int16_t *>(srcBase + (y * srcStride));
    int16_t *dstRow = reinterpret_cast<int16_t *>(dstBase + (y * dstStride));
    int32_t x = 0;
    for (; x <= width - 16; x += 16) {
      // READ MEMORY: Load 16 16-bit integers
      __m256i v_in =
          _mm256_loadu_si256(reinterpret_cast<const __m256i *>(srcRow + x));

      // ARITHMETIC SHIFT: Shift 16-bit signed values right
      __m256i v_shifted = _mm256_srai_epi16(v_in, shifts);

      // STORE RESULT: Store 16 values to memory
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(dstRow + x), v_shifted);
    }
    // Cleanup remaining
    for (; x < width; ++x) {
      dstRow[x] = static_cast<int16_t>(srcRow[x] >> shifts);
    }
  }
}

void Cast_Floating_I16toSGL(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride) 
{
  for (int32_t y = 0; y < height; ++y) {
    const int16_t *srcRow =
        reinterpret_cast<const int16_t *>(srcBase + (y * srcStride));
    float *dstRow = reinterpret_cast<float *>(dstBase + (y * dstStride));
    int32_t x = 0;
    for (; x <= width - 8; x += 8) {
      // READ MEMORY: Load 8 16-bit integers
      __m128i v_in_16 =
          _mm_loadu_si128(reinterpret_cast<const __m128i *>(srcRow + x));

      // HARDWARE EXPAND: Expand signed 16-bit integers to 32-bit integers
      __m256i v_in_32 = _mm256_cvtepi16_epi32(v_in_16);

      // CAST: Convert 32-bit integers to single-precision float
      __m256 v_float = _mm256_cvtepi32_ps(v_in_32);

      // STORE RESULT: Store 8 floats to memory
      _mm256_storeu_ps(dstRow + x, v_float);
    }
    // Cleanup remaining
    for (; x < width; ++x) {
      dstRow[x] = static_cast<float>(srcRow[x]);
    }
  }
}

void Cast_Shift_Color_I16ToU32(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride,
    int32_t shifts) 
{
  // Shifts >= 16 result in zeros
  if (shifts >= 16) {
    for (int32_t y = 0; y < height; ++y) {
      LvImage::Pixel_ColorU32 *dstRow =
          reinterpret_cast<LvImage::Pixel_ColorU32 *>(dstBase +
                                                      (y * dstStride));
      for (int32_t x = 0; x < width; ++x) {
        dstRow[x] = {0, 0, 0, 0};
      }
    }
    return;
  }

  // Shuffle mask to map lower 8 bits of components
  // LabVIEW expects Alpha=0
  __m256i shuffleMask = _mm256_setr_epi8(
      0, 0, 0, 0x80, 4, 4, 4, 0x80, 8, 8, 8, 0x80, 12, 12, 12, 0x80, 0, 0, 0,
      0x80, 4, 4, 4, 0x80, 8, 8, 8, 0x80, 12, 12, 12, 0x80);

  // BROADCAST ZERO: Zero vector for clamping negatives
  __m256i v_zero = _mm256_setzero_si256();

  // BROADCAST LIMIT: Cap RGB at 8-bit maximum (255)
  __m256i max_color_limit = _mm256_set1_epi32(255);

  for (int32_t y = 0; y < height; ++y) {
    const int16_t *srcRow =
        reinterpret_cast<const int16_t *>(srcBase + (y * srcStride));
    LvImage::Pixel_ColorU32 *dstRow =
        reinterpret_cast<LvImage::Pixel_ColorU32 *>(dstBase + (y * dstStride));
    int32_t x = 0;
    for (; x <= width - 8; x += 8) {
      // READ MEMORY: Load 8 16-bit integers
      __m128i v_in_16 =
          _mm_loadu_si128(reinterpret_cast<const __m128i *>(srcRow + x));

      // HARDWARE EXPAND: Expand signed 16-bit integers to 32-bit integers
      __m256i v_in_32 = _mm256_cvtepi16_epi32(v_in_16);

      // ARITHMETIC SHIFT: Shift 32-bit signed values right
      __m256i v_shifted = _mm256_srai_epi32(v_in_32, shifts);

      // CLAMP LIMITS: Floor negatives to zero
      __m256i v_floored = _mm256_max_epi32(v_shifted, v_zero);

      // CLAMP LIMITS: Saturate against 8-bit maximum (255)
      __m256i v_clamped = _mm256_min_epu32(v_floored, max_color_limit);

      // MEMORY SHUFFLE: Shuffle channels to form BGR
      __m256i v_final = _mm256_shuffle_epi8(v_clamped, shuffleMask);

      // STORE RESULT: Store 8 pixels to memory
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(dstRow + x), v_final);
    }
    // Cleanup remaining
    for (; x < width; ++x) {
      uint8_t val = static_cast<uint8_t>(std::min(
          std::max(0, static_cast<int32_t>(srcRow[x] >> shifts)), 255));
      dstRow[x] = {val, val, val, 0};
    }
  }
}

void Cast_Shift_Color_I16ToU64(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride,
    int32_t shifts) 
{
  // Shifts >= 16 result in zeros
  if (shifts >= 16) {
    for (int32_t y = 0; y < height; ++y) {
      LvImage::Pixel_ColorU64 *dstRow =
          reinterpret_cast<LvImage::Pixel_ColorU64 *>(dstBase +
                                                      (y * dstStride));
      for (int32_t x = 0; x < width; ++x) {
        dstRow[x] = {0, 0, 0, 0};
      }
    }
    return;
  }

  // Shuffle mask to duplicate 16-bit spans
  // LabVIEW expects Alpha=0
  __m256i shuffleMask = _mm256_setr_epi8(
      0, 1, 0, 1, 0, 1, 0x80, 0x80, 8, 9, 8, 9, 8, 9, 0x80, 0x80, 0, 1, 0, 1, 0,
      1, 0x80, 0x80, 8, 9, 8, 9, 8, 9, 0x80, 0x80);

  // BROADCAST ZERO: Zero vector for clamping negatives
  __m256i v_zero = _mm256_setzero_si256();

  for (int32_t y = 0; y < height; ++y) {
    const int16_t *srcRow =
        reinterpret_cast<const int16_t *>(srcBase + (y * srcStride));
    LvImage::Pixel_ColorU64 *dstRow =
        reinterpret_cast<LvImage::Pixel_ColorU64 *>(dstBase + (y * dstStride));
    int32_t x = 0;
    for (; x <= width - 4; x += 4) {
      // READ MEMORY: Load 4 16-bit integers
      __m128i v_in_16 =
          _mm_loadl_epi64(reinterpret_cast<const __m128i *>(srcRow + x));

      // HARDWARE EXPAND: Expand signed 16-bit integers to 64-bit integers
      __m256i v_in_64 = _mm256_cvtepi16_epi64(v_in_16);

      // ARITHMETIC SHIFT: Shift 64-bit signed values right
      __m256i v_shifted = _mm256_srai_epi64(v_in_64, shifts);

      // CLAMP LIMITS: Clamp negatives to zero
      __m256i v_clamped = _mm256_max_epi32(v_shifted, v_zero);

      // MEMORY SHUFFLE: Shuffle channels to form BGR
      __m256i v_final = _mm256_shuffle_epi8(v_clamped, shuffleMask);

      // STORE RESULT: Store 4 pixels to memory
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(dstRow + x), v_final);
    }
    // Cleanup remaining
    for (; x < width; ++x) {
      uint16_t val = static_cast<uint16_t>(
          std::max(0, static_cast<int32_t>(srcRow[x] >> shifts)));
      dstRow[x] = {val, val, val, 0};
    }
  }
}

void Cast_Floating_I16ToComplex(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride)
{
  for (int32_t y = 0; y < height; ++y) {
    const int16_t *srcRow =
        reinterpret_cast<const int16_t *>(srcBase + (y * srcStride));
    LvImage::Pixel_ComplexSGL *dstRow =
        reinterpret_cast<LvImage::Pixel_ComplexSGL *>(dstBase +
                                                      (y * dstStride));
    int32_t x = 0;
    for (; x <= width - 8; x += 8) {
      // READ MEMORY: Load 8 16-bit integers
      __m128i v_in_16 =
          _mm_loadu_si128(reinterpret_cast<const __m128i *>(srcRow + x));

      // HARDWARE EXPAND: Expand signed 16-bit integers to 32-bit integers
      __m256i v_in_32 = _mm256_cvtepi16_epi32(v_in_16);

      // CAST: Convert 32-bit integers to single-precision float
      __m256 v_float = _mm256_cvtepi32_ps(v_in_32);

      // COMPLEX ZERO: Zero array for imaginary components
      __m256 v_zero = _mm256_setzero_ps();

      // UNPACK AND INTERLEAVE: Interleave real floats with imaginary zeros
      __m256 v_lo = _mm256_unpacklo_ps(v_float, v_zero);
      __m256 v_hi = _mm256_unpackhi_ps(v_float, v_zero);

      // EXTRACT MEMORY BANKS: Extract portions for alignment serialization
      __m128 lo_0 = _mm256_castps256_ps128(v_lo);
      __m128 lo_1 = _mm256_extractf128_ps(v_lo, 1);
      __m128 hi_0 = _mm256_castps256_ps128(v_hi);
      __m128 hi_1 = _mm256_extractf128_ps(v_hi, 1);

      // STORE RESULT: Store 8 complex values to memory
      _mm_storeu_ps(reinterpret_cast<float *>(dstRow + x), lo_0);
      _mm_storeu_ps(reinterpret_cast<float *>(dstRow + x + 2), hi_0);
      _mm_storeu_ps(reinterpret_cast<float *>(dstRow + x + 4), lo_1);
      _mm_storeu_ps(reinterpret_cast<float *>(dstRow + x + 6), hi_1);
    }
    // Cleanup remaining
    for (; x < width; ++x) {
      dstRow[x] = {static_cast<float>(srcRow[x]), 0.0f};
    }
  }
}

#pragma endregion

#pragma region MonoSGL

void Cast_Floating_SGLtoU8(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride)
{
  for (int32_t y = 0; y < height; ++y) {
    const float *srcRow =
        reinterpret_cast<const float *>(srcBase + (y * srcStride));
    uint8_t *dstRow = dstBase + (y * dstStride);

    int32_t x = 0;
    for (; x <= width - 32; x += 32) {
      // READ MEMORY: Load 32 floats across four registers
      __m256 v_f1 = _mm256_loadu_ps(srcRow + x);
      __m256 v_f2 = _mm256_loadu_ps(srcRow + x + 8);
      __m256 v_f3 = _mm256_loadu_ps(srcRow + x + 16);
      __m256 v_f4 = _mm256_loadu_ps(srcRow + x + 24);

      // CLAMP LIMITS: Clamp float values between 0 and 255
      __m256 v_max = _mm256_set1_ps(255.0f);
      __m256 v_min = _mm256_setzero_ps();

      v_f1 = _mm256_max_ps(v_min, _mm256_min_ps(v_f1, v_max));
      v_f2 = _mm256_max_ps(v_min, _mm256_min_ps(v_f2, v_max));
      v_f3 = _mm256_max_ps(v_min, _mm256_min_ps(v_f3, v_max));
      v_f4 = _mm256_max_ps(v_min, _mm256_min_ps(v_f4, v_max));

      // CAST: Convert single-precision float to 32-bit integer
      __m256i v_i1 = _mm256_cvttps_epi32(v_f1);
      __m256i v_i2 = _mm256_cvttps_epi32(v_f2);
      __m256i v_i3 = _mm256_cvttps_epi32(v_f3);
      __m256i v_i4 = _mm256_cvttps_epi32(v_f4);

      // PACK SATURATE: Pack 32-bit to 16-bit with saturation
      __m256i v_pack1 = _mm256_packus_epi32(v_i1, v_i2);
      __m256i v_pack2 = _mm256_packus_epi32(v_i3, v_i4);

      // FIX LANE ORDER: Correct lane-wise pack ordering with permute
      v_pack1 = _mm256_permute4x64_epi64(v_pack1, _MM_SHUFFLE(3, 1, 2, 0));
      v_pack2 = _mm256_permute4x64_epi64(v_pack2, _MM_SHUFFLE(3, 1, 2, 0));

      // PACK SATURATE: Pack 16-bit to 8-bit with saturation
      __m256i v_final = _mm256_packus_epi16(v_pack1, v_pack2);

      // FIX LANE ORDER: Correct lane-wise pack ordering with permute
      v_final = _mm256_permute4x64_epi64(v_final, _MM_SHUFFLE(3, 1, 2, 0));

      // STORE RESULT: Store 32 bytes to memory
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(dstRow + x), v_final);
    }

    // Cleanup remaining
    for (; x < width; ++x) {
      float val = std::min(std::max(srcRow[x], 0.0f), 255.0f);
      dstRow[x] = static_cast<uint8_t>(val);
    }
  }
}

void Cast_Floating_SGLtoU16(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride)
{
  for (int32_t y = 0; y < height; ++y) {
    const float *srcRow =
        reinterpret_cast<const float *>(srcBase + (y * srcStride));
    uint16_t *dstRow = reinterpret_cast<uint16_t *>(dstBase + (y * dstStride));
    int32_t x = 0;
    for (; x <= width - 8; x += 8) {
      // READ MEMORY: Load 8 floats
      __m256 v_in = _mm256_loadu_ps(srcRow + x);

      // CLAMP LIMITS: Clamp float values between 0 and 65535
      __m256 v_max = _mm256_set1_ps(65535.0f);
      __m256 v_min = _mm256_setzero_ps();
      __m256 v_clamped = _mm256_max_ps(v_min, _mm256_min_ps(v_in, v_max));

      // CAST: Convert single-precision float to 32-bit integer
      __m256i v_i32 = _mm256_cvttps_epi32(v_clamped);

      // PACK SATURATE: Pack 32-bit to 16-bit with saturation
      __m256i v_i16 = _mm256_packus_epi32(v_i32, _mm256_setzero_si256());

      // FIX LANE ORDER: Correct lane-wise pack ordering with permute
      __m256i v_perm = _mm256_permute4x64_epi64(v_i16, _MM_SHUFFLE(3, 1, 2, 0));

      // EXTRACT: Extract lower 128-bit lane
      __m128i v_final = _mm256_castsi256_si128(v_perm);

      // STORE RESULT: Store 8 16-bit integers to memory
      _mm_storeu_si128(reinterpret_cast<__m128i *>(dstRow + x), v_final);
    }
    // Cleanup remaining
    for (; x < width; ++x) {
      float val = std::min(std::max(srcRow[x], 0.0f), 65535.0f);
      dstRow[x] = static_cast<uint16_t>(val);
    }
  }
}

void Cast_Floating_SGLtoI16(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride)
{
  for (int32_t y = 0; y < height; ++y) {
    const float *srcRow =
        reinterpret_cast<const float *>(srcBase + (y * srcStride));
    int16_t *dstRow = reinterpret_cast<int16_t *>(dstBase + (y * dstStride));
    int32_t x = 0;
    for (; x <= width - 8; x += 8) {
      // READ MEMORY: Load 8 floats
      __m256 v_in = _mm256_loadu_ps(srcRow + x);

      // CLAMP LIMITS: Clamp float values between -32768 and 32767
      __m256 v_max = _mm256_set1_ps(32767.0f);
      __m256 v_min = _mm256_set1_ps(-32768.0f);
      __m256 v_clamped = _mm256_max_ps(v_min, _mm256_min_ps(v_in, v_max));

      // CAST: Convert single-precision float to 32-bit integer
      __m256i v_i32 = _mm256_cvttps_epi32(v_clamped);

      // PACK SATURATE: Pack 32-bit to signed 16-bit with saturation
      __m256i v_i16 = _mm256_packs_epi32(v_i32, _mm256_setzero_si256());

      // FIX LANE ORDER: Correct lane-wise pack ordering with permute
      __m256i v_perm = _mm256_permute4x64_epi64(v_i16, _MM_SHUFFLE(3, 1, 2, 0));

      // EXTRACT: Extract lower 128-bit lane
      __m128i v_final = _mm256_castsi256_si128(v_perm);

      // STORE RESULT: Store 8 16-bit integers to memory
      _mm_storeu_si128(reinterpret_cast<__m128i *>(dstRow + x), v_final);
    }
    // Cleanup remaining
    for (; x < width; ++x) {
      float val = std::min(std::max(srcRow[x], -32768.0f), 32767.0f);
      dstRow[x] = static_cast<int16_t>(val);
    }
  }
}

void Cast_Floating_SGLtoSGL(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride)
{
  for (int32_t y = 0; y < height; ++y) {
    const float *srcRow =
        reinterpret_cast<const float *>(srcBase + (y * srcStride));
    float *dstRow = reinterpret_cast<float *>(dstBase + (y * dstStride));
    int32_t x = 0;
    for (; x <= width - 8; x += 8) {
      // READ MEMORY: Load 8 floats
      __m256 v_in = _mm256_loadu_ps(srcRow + x);

      // STORE RESULT: Store 8 floats to memory
      _mm256_storeu_ps(dstRow + x, v_in);
    }
    // Cleanup remaining
    for (; x < width; ++x) {
      dstRow[x] = srcRow[x];
    }
  }
}

void Cast_Floating_SGLtoColorU32(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride)
{
  // Shuffle mask to map lower 8 bits of components
  // LabVIEW expects Alpha=0
  __m256i shuffleMask = _mm256_setr_epi8(
      0, 0, 0, 0x80, 4, 4, 4, 0x80, 8, 8, 8, 0x80, 12, 12, 12, 0x80, 0, 0, 0,
      0x80, 4, 4, 4, 0x80, 8, 8, 8, 0x80, 12, 12, 12, 0x80);

  for (int32_t y = 0; y < height; ++y) {
    const float *srcRow =
        reinterpret_cast<const float *>(srcBase + (y * srcStride));
    LvImage::Pixel_ColorU32 *dstRow =
        reinterpret_cast<LvImage::Pixel_ColorU32 *>(dstBase + (y * dstStride));
    int32_t x = 0;
    for (; x <= width - 8; x += 8) {
      // READ MEMORY: Load 8 floats
      __m256 v_in = _mm256_loadu_ps(srcRow + x);

      // CLAMP LIMITS: Clamp float values between 0 and 255
      __m256 v_max = _mm256_set1_ps(255.0f);
      __m256 v_min = _mm256_setzero_ps();
      __m256 v_clamped = _mm256_max_ps(v_min, _mm256_min_ps(v_in, v_max));

      // CAST: Convert single-precision float to 32-bit integer
      __m256i v_i32 = _mm256_cvttps_epi32(v_clamped);

      // MEMORY SHUFFLE: Shuffle channels to form BGR
      __m256i v_final = _mm256_shuffle_epi8(v_i32, shuffleMask);

      // STORE RESULT: Store 8 pixels to memory
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(dstRow + x), v_final);
    }
    // Cleanup remaining
    for (; x < width; ++x) {
      uint8_t val =
          static_cast<uint8_t>(std::min(std::max(srcRow[x], 0.0f), 255.0f));
      dstRow[x] = {val, val, val, 0};
    }
  }
}

void Cast_Floating_SGLtoColorU64(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride)
{
  // Shuffle mask to duplicate 16-bit spans
  // LabVIEW expects Alpha=0
  __m256i shuffleMask = _mm256_setr_epi8(
      0, 1, 0, 1, 0, 1, 0x80, 0x80, 4, 5, 4, 5, 4, 5, 0x80, 0x80, 8, 9, 8, 9, 8,
      9, 0x80, 0x80, 12, 13, 12, 13, 12, 13, 0x80, 0x80);

  for (int32_t y = 0; y < height; ++y) {
    const float *srcRow =
        reinterpret_cast<const float *>(srcBase + (y * srcStride));
    LvImage::Pixel_ColorU64 *dstRow =
        reinterpret_cast<LvImage::Pixel_ColorU64 *>(dstBase + (y * dstStride));
    int32_t x = 0;
    for (; x <= width - 4; x += 4) {
      // READ MEMORY: Load 4 floats
      __m128 v_in = _mm_loadu_ps(srcRow + x);

      // CLAMP LIMITS: Clamp float values between 0 and 65535
      __m128 v_max = _mm_set1_ps(65535.0f);
      __m128 v_min = _mm_setzero_ps();
      __m128 v_clamped = _mm_max_ps(v_min, _mm_min_ps(v_in, v_max));

      // CAST: Convert single-precision float to 32-bit integer
      __m128i v_i32 = _mm_cvttps_epi32(v_clamped);

      // HARDWARE EXPAND: Expand 32-bit integers to 64-bit integers
      __m256i v_i64 = _mm256_cvtepi32_epi64(v_i32);

      // MEMORY SHUFFLE: Shuffle channels to form BGR
      __m256i v_final = _mm256_shuffle_epi8(v_i64, shuffleMask);

      // STORE RESULT: Store 4 pixels to memory
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(dstRow + x), v_final);
    }
    // Cleanup remaining
    for (; x < width; ++x) {
      uint16_t val =
          static_cast<uint16_t>(std::min(std::max(srcRow[x], 0.0f), 65535.0f));
      dstRow[x] = {val, val, val, 0};
    }
  }
}

void Cast_Floating_SGLtoComplex(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride)
{
  for (int32_t y = 0; y < height; ++y) {
    const float *srcRow =
        reinterpret_cast<const float *>(srcBase + (y * srcStride));
    LvImage::Pixel_ComplexSGL *dstRow =
        reinterpret_cast<LvImage::Pixel_ComplexSGL *>(dstBase +
                                                      (y * dstStride));
    int32_t x = 0;
    for (; x <= width - 8; x += 8) {
      // READ MEMORY: Load 8 floats
      __m256 v_in = _mm256_loadu_ps(srcRow + x);

      // COMPLEX ZERO: Zero array for imaginary components
      __m256 v_zero = _mm256_setzero_ps();

      // UNPACK AND INTERLEAVE: Interleave real floats with imaginary zeros
      __m256 v_lo = _mm256_unpacklo_ps(v_in, v_zero);
      __m256 v_hi = _mm256_unpackhi_ps(v_in, v_zero);

      // EXTRACT MEMORY BANKS: Extract portions for alignment serialization
      __m128 lo_0 = _mm256_castps256_ps128(v_lo);
      __m128 lo_1 = _mm256_extractf128_ps(v_lo, 1);
      __m128 hi_0 = _mm256_castps256_ps128(v_hi);
      __m128 hi_1 = _mm256_extractf128_ps(v_hi, 1);

      // STORE RESULT: Store 8 complex values to memory
      _mm_storeu_ps(reinterpret_cast<float *>(dstRow + x), lo_0);
      _mm_storeu_ps(reinterpret_cast<float *>(dstRow + x + 2), hi_0);
      _mm_storeu_ps(reinterpret_cast<float *>(dstRow + x + 4), lo_1);
      _mm_storeu_ps(reinterpret_cast<float *>(dstRow + x + 6), hi_1);
    }
    // Cleanup remaining
    for (; x < width; ++x) {
      dstRow[x] = {srcRow[x], 0.0f};
    }
  }
}

#pragma endregion

#pragma region ColorU32

void Cast_Shift_Color_U32toU8(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride,
    int32_t shifts) 
{

  // Shuffle mask to isolate B channel (byte 0) of each 32-bit pixel
  __m256i shufB = _mm256_setr_epi8(
      0, 0x80, 4, 0x80, 8, 0x80, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
      0x80, 0x80, 0, 0x80, 4, 0x80, 8, 0x80, 12, 0x80, 0x80, 0x80, 0x80, 0x80,
      0x80, 0x80, 0x80, 0x80);

  // Shuffle mask to isolate G channel (byte 1) of each 32-bit pixel
  __m256i shufG = _mm256_setr_epi8(
      1, 0x80, 5, 0x80, 9, 0x80, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
      0x80, 0x80, 1, 0x80, 5, 0x80, 9, 0x80, 13, 0x80, 0x80, 0x80, 0x80, 0x80,
      0x80, 0x80, 0x80, 0x80);

  // Shuffle mask to isolate R channel (byte 2) of each 32-bit pixel
  __m256i shufR = _mm256_setr_epi8(
      2, 0x80, 6, 0x80, 10, 0x80, 14, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
      0x80, 0x80, 2, 0x80, 6, 0x80, 10, 0x80, 14, 0x80, 0x80, 0x80, 0x80, 0x80,
      0x80, 0x80, 0x80, 0x80);

  // BROADCAST DIVISOR: Fixed-point reciprocal of 3 (21846 ≈ 65536/3)
  __m256i v_div3 = _mm256_set1_epi16(21846);

  for (int32_t y = 0; y < height; ++y) {
    const LvImage::Pixel_ColorU32 *srcRow =
        reinterpret_cast<const LvImage::Pixel_ColorU32 *>(srcBase +
                                                          (y * srcStride));
    uint8_t *dstRow = dstBase + (y * dstStride);
    int32_t x = 0;
    for (; x <= width - 8; x += 8) {
      // READ MEMORY: Load 8 ColorU32 pixels (32 bytes)
      __m256i v_in =
          _mm256_loadu_si256(reinterpret_cast<const __m256i *>(srcRow + x));

      // MEMORY SHUFFLE: Isolate B, G, R channels to 16-bit lanes
      __m256i v_b = _mm256_shuffle_epi8(v_in, shufB);
      __m256i v_g = _mm256_shuffle_epi8(v_in, shufG);
      __m256i v_r = _mm256_shuffle_epi8(v_in, shufR);

      // ACCUMULATE SUM: Add R + G + B in 16-bit
      __m256i v_sum = _mm256_add_epi16(v_b, _mm256_add_epi16(v_g, v_r));

      // DIVIDE BY 3: Multiply by reciprocal and take high 16 bits
      __m256i v_avg = _mm256_mulhi_epu16(v_sum, v_div3);

      // BITWISE SHIFT: Shift 16-bit averaged values right
      __m256i v_shifted = _mm256_srli_epi16(v_avg, shifts);

      // PACK SATURATE: Pack 16-bit to 8-bit with saturation
      __m256i v_packed = _mm256_packus_epi16(v_shifted, _mm256_setzero_si256());

      // FIX LANE ORDER: Correct lane-wise pack ordering with permute
      __m256i v_perm =
          _mm256_permute4x64_epi64(v_packed, _MM_SHUFFLE(3, 1, 2, 0));

      // STORE RESULT: Store 8 bytes to memory
      _mm_storel_epi64(reinterpret_cast<__m128i *>(dstRow + x),
                       _mm256_castsi256_si128(v_perm));
    }

    // Cleanup remaining
    for (; x < width; ++x) {
      uint32_t avg = (srcRow[x].r + srcRow[x].g + srcRow[x].b) / 3;
      uint32_t val = avg >> shifts;
      dstRow[x] = static_cast<uint8_t>(std::min(val, 255u));
    }
  }
}

void Cast_Shift_Color_U32toU16(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride,
    int32_t shifts) 
{

  // Shuffle mask to isolate B channel (byte 0) of each 32-bit pixel
  __m256i shufB = _mm256_setr_epi8(
      0, 0x80, 4, 0x80, 8, 0x80, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
      0x80, 0x80, 0, 0x80, 4, 0x80, 8, 0x80, 12, 0x80, 0x80, 0x80, 0x80, 0x80,
      0x80, 0x80, 0x80, 0x80);

  // Shuffle mask to isolate G channel (byte 1) of each 32-bit pixel
  __m256i shufG = _mm256_setr_epi8(
      1, 0x80, 5, 0x80, 9, 0x80, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
      0x80, 0x80, 1, 0x80, 5, 0x80, 9, 0x80, 13, 0x80, 0x80, 0x80, 0x80, 0x80,
      0x80, 0x80, 0x80, 0x80);

  // Shuffle mask to isolate R channel (byte 2) of each 32-bit pixel
  __m256i shufR = _mm256_setr_epi8(
      2, 0x80, 6, 0x80, 10, 0x80, 14, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
      0x80, 0x80, 2, 0x80, 6, 0x80, 10, 0x80, 14, 0x80, 0x80, 0x80, 0x80, 0x80,
      0x80, 0x80, 0x80, 0x80);

  // BROADCAST DIVISOR: Fixed-point reciprocal of 3 (21846 ≈ 65536/3)
  __m256i v_div3 = _mm256_set1_epi16(21846);

  for (int32_t y = 0; y < height; ++y) {
    const LvImage::Pixel_ColorU32 *srcRow =
        reinterpret_cast<const LvImage::Pixel_ColorU32 *>(srcBase +
                                                          (y * srcStride));
    uint16_t *dstRow = reinterpret_cast<uint16_t *>(dstBase + (y * dstStride));
    int32_t x = 0;
    for (; x <= width - 8; x += 8) {
      // READ MEMORY: Load 8 ColorU32 pixels (32 bytes)
      __m256i v_in =
          _mm256_loadu_si256(reinterpret_cast<const __m256i *>(srcRow + x));

      // MEMORY SHUFFLE: Isolate B, G, R channels to 16-bit lanes
      __m256i v_b = _mm256_shuffle_epi8(v_in, shufB);
      __m256i v_g = _mm256_shuffle_epi8(v_in, shufG);
      __m256i v_r = _mm256_shuffle_epi8(v_in, shufR);

      // ACCUMULATE SUM: Add R + G + B in 16-bit
      __m256i v_sum = _mm256_add_epi16(v_b, _mm256_add_epi16(v_g, v_r));

      // DIVIDE BY 3: Multiply by reciprocal and take high 16 bits
      __m256i v_avg = _mm256_mulhi_epu16(v_sum, v_div3);

      // BITWISE SHIFT: Shift 16-bit averaged values right
      __m256i v_shifted = _mm256_srli_epi16(v_avg, shifts);

      // EXTRACT MEMORY BANKS: Combine 4 values from each lane
      __m128i lo = _mm256_castsi256_si128(v_shifted);
      __m128i hi = _mm256_extracti128_si256(v_shifted, 1);
      __m128i v_final = _mm_unpacklo_epi64(lo, hi);

      // STORE RESULT: Store 8 16-bit integers to memory
      _mm_storeu_si128(reinterpret_cast<__m128i *>(dstRow + x), v_final);
    }

    // Cleanup remaining
    for (; x < width; ++x) {
      uint32_t sum = srcRow[x].r + srcRow[x].g + srcRow[x].b;
      uint16_t val = static_cast<uint16_t>((sum / 3) >> shifts);
      dstRow[x] = val;
    }
  }
}

void Cast_Shift_Color_U32toI16(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride,
    int32_t shifts)
{

  // Shuffle mask to isolate B channel (byte 0) of each 32-bit pixel
  __m256i shufB = _mm256_setr_epi8(
      0, 0x80, 4, 0x80, 8, 0x80, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
      0x80, 0x80, 0, 0x80, 4, 0x80, 8, 0x80, 12, 0x80, 0x80, 0x80, 0x80, 0x80,
      0x80, 0x80, 0x80, 0x80);

  // Shuffle mask to isolate G channel (byte 1) of each 32-bit pixel
  __m256i shufG = _mm256_setr_epi8(
      1, 0x80, 5, 0x80, 9, 0x80, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
      0x80, 0x80, 1, 0x80, 5, 0x80, 9, 0x80, 13, 0x80, 0x80, 0x80, 0x80, 0x80,
      0x80, 0x80, 0x80, 0x80);

  // Shuffle mask to isolate R channel (byte 2) of each 32-bit pixel
  __m256i shufR = _mm256_setr_epi8(
      2, 0x80, 6, 0x80, 10, 0x80, 14, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
      0x80, 0x80, 2, 0x80, 6, 0x80, 10, 0x80, 14, 0x80, 0x80, 0x80, 0x80, 0x80,
      0x80, 0x80, 0x80, 0x80);

  // BROADCAST DIVISOR: Fixed-point reciprocal of 3 (21846 ≈ 65536/3)
  __m256i v_div3 = _mm256_set1_epi16(21846);

  // BROADCAST LIMIT: Maximum positive I16 value
  __m128i v_max_i16 = _mm_set1_epi16(32767);

  for (int32_t y = 0; y < height; ++y) {
    const LvImage::Pixel_ColorU32 *srcRow =
        reinterpret_cast<const LvImage::Pixel_ColorU32 *>(srcBase +
                                                          (y * srcStride));
    int16_t *dstRow = reinterpret_cast<int16_t *>(dstBase + (y * dstStride));
    int32_t x = 0;
    for (; x <= width - 8; x += 8) {
      // READ MEMORY: Load 8 ColorU32 pixels (32 bytes)
      __m256i v_in =
          _mm256_loadu_si256(reinterpret_cast<const __m256i *>(srcRow + x));

      // MEMORY SHUFFLE: Isolate B, G, R channels to 16-bit lanes
      __m256i v_b = _mm256_shuffle_epi8(v_in, shufB);
      __m256i v_g = _mm256_shuffle_epi8(v_in, shufG);
      __m256i v_r = _mm256_shuffle_epi8(v_in, shufR);

      // ACCUMULATE SUM: Add R + G + B in 16-bit
      __m256i v_sum = _mm256_add_epi16(v_b, _mm256_add_epi16(v_g, v_r));

      // DIVIDE BY 3: Multiply by reciprocal and take high 16 bits
      __m256i v_avg = _mm256_mulhi_epu16(v_sum, v_div3);

      // BITWISE SHIFT: Shift 16-bit averaged values right
      __m256i v_shifted = _mm256_srli_epi16(v_avg, shifts);

      // EXTRACT MEMORY BANKS: Combine 4 values from each lane
      __m128i lo = _mm256_castsi256_si128(v_shifted);
      __m128i hi = _mm256_extracti128_si256(v_shifted, 1);
      __m128i v_combined = _mm_unpacklo_epi64(lo, hi);

      // CLAMP LIMITS: Cap at maximum positive I16 value
      __m128i v_clamped = _mm_min_epi16(v_combined, v_max_i16);

      // STORE RESULT: Store 8 16-bit integers to memory
      _mm_storeu_si128(reinterpret_cast<__m128i *>(dstRow + x), v_clamped);
    }

    // Cleanup remaining
    for (; x < width; ++x) {
      uint32_t sum = srcRow[x].r + srcRow[x].g + srcRow[x].b;
      int16_t val = static_cast<int16_t>(
          std::min(static_cast<int32_t>((sum / 3) >> shifts), 32767));
      dstRow[x] = val;
    }
  }
}

void Cast_Floating_ColorU32toSGL(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride)
{

  // Shuffle mask to isolate B channel (byte 0) zero-extended to 32-bit
  __m256i shufB = _mm256_setr_epi8(
      0, 0x80, 0x80, 0x80, 4, 0x80, 0x80, 0x80, 8, 0x80, 0x80, 0x80, 12, 0x80,
      0x80, 0x80, 0, 0x80, 0x80, 0x80, 4, 0x80, 0x80, 0x80, 8, 0x80, 0x80,
      0x80, 12, 0x80, 0x80, 0x80);

  // Shuffle mask to isolate G channel (byte 1) zero-extended to 32-bit
  __m256i shufG = _mm256_setr_epi8(
      1, 0x80, 0x80, 0x80, 5, 0x80, 0x80, 0x80, 9, 0x80, 0x80, 0x80, 13, 0x80,
      0x80, 0x80, 1, 0x80, 0x80, 0x80, 5, 0x80, 0x80, 0x80, 9, 0x80, 0x80,
      0x80, 13, 0x80, 0x80, 0x80);

  // Shuffle mask to isolate R channel (byte 2) zero-extended to 32-bit
  __m256i shufR = _mm256_setr_epi8(
      2, 0x80, 0x80, 0x80, 6, 0x80, 0x80, 0x80, 10, 0x80, 0x80, 0x80, 14,
      0x80, 0x80, 0x80, 2, 0x80, 0x80, 0x80, 6, 0x80, 0x80, 0x80, 10, 0x80,
      0x80, 0x80, 14, 0x80, 0x80, 0x80);

  // BROADCAST DIVISOR: Floating-point reciprocal of 3
  __m256 v_div3 = _mm256_set1_ps(1.0f / 3.0f);

  for (int32_t y = 0; y < height; ++y) {
    const LvImage::Pixel_ColorU32 *srcRow =
        reinterpret_cast<const LvImage::Pixel_ColorU32 *>(srcBase +
                                                          (y * srcStride));
    float *dstRow = reinterpret_cast<float *>(dstBase + (y * dstStride));
    int32_t x = 0;
    for (; x <= width - 8; x += 8) {
      // READ MEMORY: Load 8 ColorU32 pixels (32 bytes)
      __m256i v_in =
          _mm256_loadu_si256(reinterpret_cast<const __m256i *>(srcRow + x));

      // MEMORY SHUFFLE: Isolate B, G, R channels to 32-bit lanes
      __m256i v_b = _mm256_shuffle_epi8(v_in, shufB);
      __m256i v_g = _mm256_shuffle_epi8(v_in, shufG);
      __m256i v_r = _mm256_shuffle_epi8(v_in, shufR);

      // ACCUMULATE SUM: Add R + G + B in 32-bit integer
      __m256i v_sum = _mm256_add_epi32(v_b, _mm256_add_epi32(v_g, v_r));

      // CAST: Convert 32-bit integer sum to single-precision float
      __m256 v_float = _mm256_cvtepi32_ps(v_sum);

      // DIVIDE BY 3: Multiply by floating-point reciprocal
      __m256 v_avg = _mm256_mul_ps(v_float, v_div3);

      // STORE RESULT: Store 8 floats to memory
      _mm256_storeu_ps(dstRow + x, v_avg);
    }

    // Cleanup remaining
    for (; x < width; ++x) {
      uint32_t sum = srcRow[x].r + srcRow[x].g + srcRow[x].b;
      dstRow[x] = static_cast<float>(sum / 3);
    }
  }
}

void Cast_Shift_Color_U32toU32(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride,
    int32_t shifts)
{

  // Shifts >= 8 zero out all 8-bit channels
  if (shifts >= 8) {
    for (int32_t y = 0; y < height; ++y) {
      std::memset(dstBase + (y * dstStride), 0,
                  width * sizeof(LvImage::Pixel_ColorU32));
    }
    return;
  }

  // Calculate mask to prevent crossing byte boundaries
  // Combined with alpha zero: B,G,R get cleaned mask, A gets 0x00
  uint8_t maskByte = 0xFF >> shifts;

  // BROADCAST MASK: Duplicate combined mask across 32 bytes (alpha=0)
  __m256i v_mask = _mm256_setr_epi8(
      maskByte, maskByte, maskByte, 0, maskByte, maskByte, maskByte, 0,
      maskByte, maskByte, maskByte, 0, maskByte, maskByte, maskByte, 0,
      maskByte, maskByte, maskByte, 0, maskByte, maskByte, maskByte, 0,
      maskByte, maskByte, maskByte, 0, maskByte, maskByte, maskByte, 0);

  for (int32_t y = 0; y < height; ++y) {
    const LvImage::Pixel_ColorU32 *srcRow =
        reinterpret_cast<const LvImage::Pixel_ColorU32 *>(srcBase +
                                                          (y * srcStride));
    LvImage::Pixel_ColorU32 *dstRow =
        reinterpret_cast<LvImage::Pixel_ColorU32 *>(dstBase + (y * dstStride));
    int32_t x = 0;
    for (; x <= width - 8; x += 8) {
      // READ MEMORY: Load 8 ColorU32 pixels (32 bytes)
      __m256i v_in =
          _mm256_loadu_si256(reinterpret_cast<const __m256i *>(srcRow + x));

      // BITWISE SHIFT: Shift 16-bit array values right (shifts all 4 channels)
      __m256i v_shifted = _mm256_srli_epi16(v_in, shifts);

      // APPLY MASK: Clean neighboring bytes and zero alpha channel
      __m256i v_final = _mm256_and_si256(v_shifted, v_mask);

      // STORE RESULT: Store 8 ColorU32 pixels to memory
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(dstRow + x), v_final);
    }

    // Cleanup remaining
    for (; x < width; ++x) {
      dstRow[x] = {
          static_cast<uint8_t>(srcRow[x].b >> shifts),
          static_cast<uint8_t>(srcRow[x].g >> shifts),
          static_cast<uint8_t>(srcRow[x].r >> shifts), 0};
    }
  }
}

void Cast_Shift_Color_U32toU64(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride,
    int32_t shifts)
{

  // Shifts >= 8 zero out all 8-bit channels
  if (shifts >= 8) {
    for (int32_t y = 0; y < height; ++y) {
      std::memset(dstBase + (y * dstStride), 0,
                  width * sizeof(LvImage::Pixel_ColorU64));
    }
    return;
  }

  // BROADCAST MASK: Zero alpha channel in each 64-bit pixel (keep words 0,1,2)
  __m256i alphaMask = _mm256_set_epi16(
      0, -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1);

  for (int32_t y = 0; y < height; ++y) {
    const LvImage::Pixel_ColorU32 *srcRow =
        reinterpret_cast<const LvImage::Pixel_ColorU32 *>(srcBase +
                                                          (y * srcStride));
    LvImage::Pixel_ColorU64 *dstRow =
        reinterpret_cast<LvImage::Pixel_ColorU64 *>(dstBase + (y * dstStride));

    int32_t x = 0;
    for (; x <= width - 8; x += 8) {
      // READ MEMORY: Load 8 ColorU32 pixels (32 bytes)
      __m256i v_in =
          _mm256_loadu_si256(reinterpret_cast<const __m256i *>(srcRow + x));

      // EXTRACT MEMORY BANKS: Split into lower and upper 128-bit halves
      __m128i in_lo = _mm256_castsi256_si128(v_in);
      __m128i in_hi = _mm256_extracti128_si256(v_in, 1);

      // HARDWARE EXPAND: Expand 8-bit channels to 16-bit
      __m256i v_ext_lo = _mm256_cvtepu8_epi16(in_lo);
      __m256i v_ext_hi = _mm256_cvtepu8_epi16(in_hi);

      // BITWISE SHIFT: Shift 16-bit channel values right
      v_ext_lo = _mm256_srli_epi16(v_ext_lo, shifts);
      v_ext_hi = _mm256_srli_epi16(v_ext_hi, shifts);

      // APPLY MASK: Zero alpha channel in each pixel
      v_ext_lo = _mm256_and_si256(v_ext_lo, alphaMask);
      v_ext_hi = _mm256_and_si256(v_ext_hi, alphaMask);

      // STORE RESULT: Store 4+4 ColorU64 pixels to memory
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(dstRow + x), v_ext_lo);
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(dstRow + x + 4),
                          v_ext_hi);
    }

    // Cleanup remaining
    for (; x < width; ++x) {
      uint16_t b = static_cast<uint16_t>(srcRow[x].b) >> shifts;
      uint16_t g = static_cast<uint16_t>(srcRow[x].g) >> shifts;
      uint16_t r = static_cast<uint16_t>(srcRow[x].r) >> shifts;
      dstRow[x] = {b, g, r, 0};
    }
  }
}

void Cast_Floating_ColorU32toComplex(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride)
{

  // Shuffle mask to isolate B channel (byte 0) zero-extended to 32-bit
  __m256i shufB = _mm256_setr_epi8(
      0, 0x80, 0x80, 0x80, 4, 0x80, 0x80, 0x80, 8, 0x80, 0x80, 0x80, 12, 0x80,
      0x80, 0x80, 0, 0x80, 0x80, 0x80, 4, 0x80, 0x80, 0x80, 8, 0x80, 0x80,
      0x80, 12, 0x80, 0x80, 0x80);

  // Shuffle mask to isolate G channel (byte 1) zero-extended to 32-bit
  __m256i shufG = _mm256_setr_epi8(
      1, 0x80, 0x80, 0x80, 5, 0x80, 0x80, 0x80, 9, 0x80, 0x80, 0x80, 13, 0x80,
      0x80, 0x80, 1, 0x80, 0x80, 0x80, 5, 0x80, 0x80, 0x80, 9, 0x80, 0x80,
      0x80, 13, 0x80, 0x80, 0x80);

  // Shuffle mask to isolate R channel (byte 2) zero-extended to 32-bit
  __m256i shufR = _mm256_setr_epi8(
      2, 0x80, 0x80, 0x80, 6, 0x80, 0x80, 0x80, 10, 0x80, 0x80, 0x80, 14,
      0x80, 0x80, 0x80, 2, 0x80, 0x80, 0x80, 6, 0x80, 0x80, 0x80, 10, 0x80,
      0x80, 0x80, 14, 0x80, 0x80, 0x80);

  // BROADCAST DIVISOR: Floating-point reciprocal of 3
  __m256 v_div3 = _mm256_set1_ps(1.0f / 3.0f);

  for (int32_t y = 0; y < height; ++y)
  {
    const LvImage::Pixel_ColorU32 *srcRow =
        reinterpret_cast<const LvImage::Pixel_ColorU32 *>(srcBase +
                                                          (y * srcStride));
    LvImage::Pixel_ComplexSGL *dstRow =
        reinterpret_cast<LvImage::Pixel_ComplexSGL *>(dstBase +
                                                      (y * dstStride));
    int32_t x = 0;
    for (; x <= width - 8; x += 8) {
      // READ MEMORY: Load 8 ColorU32 pixels (32 bytes)
      __m256i v_in =
          _mm256_loadu_si256(reinterpret_cast<const __m256i *>(srcRow + x));

      // MEMORY SHUFFLE: Isolate B, G, R channels to 32-bit lanes
      __m256i v_b = _mm256_shuffle_epi8(v_in, shufB);
      __m256i v_g = _mm256_shuffle_epi8(v_in, shufG);
      __m256i v_r = _mm256_shuffle_epi8(v_in, shufR);

      // ACCUMULATE SUM: Add R + G + B in 32-bit integer
      __m256i v_sum = _mm256_add_epi32(v_b, _mm256_add_epi32(v_g, v_r));

      // CAST: Convert 32-bit integer sum to single-precision float
      __m256 v_float = _mm256_cvtepi32_ps(v_sum);

      // DIVIDE BY 3: Multiply by floating-point reciprocal
      __m256 v_avg = _mm256_mul_ps(v_float, v_div3);

      // COMPLEX ZERO: Zero array for imaginary components
      __m256 v_zero = _mm256_setzero_ps();

      // UNPACK AND INTERLEAVE: Interleave real floats with imaginary zeros
      __m256 v_lo = _mm256_unpacklo_ps(v_avg, v_zero);
      __m256 v_hi = _mm256_unpackhi_ps(v_avg, v_zero);

      // EXTRACT MEMORY BANKS: Extract portions for alignment serialization
      __m128 lo_0 = _mm256_castps256_ps128(v_lo);
      __m128 lo_1 = _mm256_extractf128_ps(v_lo, 1);
      __m128 hi_0 = _mm256_castps256_ps128(v_hi);
      __m128 hi_1 = _mm256_extractf128_ps(v_hi, 1);

      // STORE RESULT: Store 8 complex values to memory
      _mm_storeu_ps(reinterpret_cast<float *>(dstRow + x), lo_0);
      _mm_storeu_ps(reinterpret_cast<float *>(dstRow + x + 2), hi_0);
      _mm_storeu_ps(reinterpret_cast<float *>(dstRow + x + 4), lo_1);
      _mm_storeu_ps(reinterpret_cast<float *>(dstRow + x + 6), hi_1);
    }

    // Cleanup remaining
    for (; x < width; ++x) {
      uint32_t sum = srcRow[x].r + srcRow[x].g + srcRow[x].b;
      dstRow[x] = {static_cast<float>(sum / 3), 0.0f};
    }
  }
}

#pragma endregion

#pragma region ColorU64

void Cast_Shift_Color_U64toU8(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride,
    int32_t shifts)
{

  // BROADCAST MASK: 16-bit channel mask within 64-bit pixel
  __m256i v_chMask = _mm256_set1_epi64x(0x000000000000FFFF);

  // BROADCAST DIVISOR: Floating-point reciprocal of 3
  __m256 v_div3 = _mm256_set1_ps(1.0f / 3.0f);

  for (int32_t y = 0; y < height; ++y) {
    const LvImage::Pixel_ColorU64 *srcRow =
        reinterpret_cast<const LvImage::Pixel_ColorU64 *>(srcBase +
                                                          (y * srcStride));
    uint8_t *dstRow = dstBase + (y * dstStride);

    int32_t x = 0;
    for (; x <= width - 4; x += 4) {
      // READ MEMORY: Load 4 ColorU64 pixels (32 bytes)
      __m256i v_in =
          _mm256_loadu_si256(reinterpret_cast<const __m256i *>(srcRow + x));

      // CHANNEL ISOLATE: Extract B, G, R channels from 64-bit pixel lanes
      __m256i v_b = _mm256_and_si256(v_in, v_chMask);
      __m256i v_g =
          _mm256_and_si256(_mm256_srli_epi64(v_in, 16), v_chMask);
      __m256i v_r =
          _mm256_and_si256(_mm256_srli_epi64(v_in, 32), v_chMask);

      // ACCUMULATE SUM: Add R + G + B in 32-bit (fits in low 32 of each 64)
      __m256i v_sum = _mm256_add_epi32(v_b, _mm256_add_epi32(v_g, v_r));

      // CAST: Convert 32-bit integer sum to single-precision float
      __m256 v_float = _mm256_cvtepi32_ps(v_sum);

      // DIVIDE BY 3: Multiply by floating-point reciprocal
      __m256 v_avg = _mm256_mul_ps(v_float, v_div3);

      // CAST: Convert float average back to 32-bit integer
      __m256i v_i32 = _mm256_cvttps_epi32(v_avg);

      // BITWISE SHIFT: Shift 32-bit averaged values right
      __m256i v_shifted = _mm256_srli_epi32(v_i32, shifts);

      // CLAMP LIMITS: Clamp to 0..255 range via min with broadcast
      __m256i v_clamped =
          _mm256_min_epu32(v_shifted, _mm256_set1_epi32(255));

      // PACK: Shuffle results into contiguous bytes
      // Results are in positions 0,8,16,24 of the 256-bit register (even 64-bit slots)
      __m256i v_shuf = _mm256_shuffle_epi8(v_clamped,
          _mm256_setr_epi8(
              0, 8, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
              0, 8, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80));

      // EXTRACT MEMORY BANKS: Combine results from both lanes
      __m128i lo = _mm256_castsi256_si128(v_shuf);
      __m128i hi = _mm256_extracti128_si256(v_shuf, 1);
      __m128i v_combined = _mm_unpacklo_epi16(lo, hi);

      // STORE RESULT: Store 4 bytes to memory
      *reinterpret_cast<int32_t *>(dstRow + x) = _mm_cvtsi128_si32(v_combined);
    }

    // Cleanup remaining
    for (; x < width; ++x)
    {
      uint32_t avg = (srcRow[x].r + srcRow[x].g + srcRow[x].b) / 3;
      uint32_t val = avg >> shifts;
      dstRow[x] = static_cast<uint8_t>(std::min(val, 255u));
    }
  }
}

void Cast_Shift_Color_U64toU16(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride,
    int32_t shifts) {

  // BROADCAST MASK: 16-bit channel mask within 64-bit pixel
  __m256i v_chMask = _mm256_set1_epi64x(0x000000000000FFFF);

  // BROADCAST DIVISOR: Floating-point reciprocal of 3
  __m256 v_div3 = _mm256_set1_ps(1.0f / 3.0f);

  for (int32_t y = 0; y < height; ++y)
  {
    const LvImage::Pixel_ColorU64 *srcRow =
        reinterpret_cast<const LvImage::Pixel_ColorU64 *>(srcBase +
                                                          (y * srcStride));
    uint16_t *dstRow = reinterpret_cast<uint16_t *>(dstBase + (y * dstStride));
    int32_t x = 0;
    for (; x <= width - 4; x += 4) {
      // READ MEMORY: Load 4 ColorU64 pixels (32 bytes)
      __m256i v_in =
          _mm256_loadu_si256(reinterpret_cast<const __m256i *>(srcRow + x));

      // CHANNEL ISOLATE: Extract B, G, R channels from 64-bit pixel lanes
      __m256i v_b = _mm256_and_si256(v_in, v_chMask);
      __m256i v_g =
          _mm256_and_si256(_mm256_srli_epi64(v_in, 16), v_chMask);
      __m256i v_r =
          _mm256_and_si256(_mm256_srli_epi64(v_in, 32), v_chMask);

      // ACCUMULATE SUM: Add R + G + B in 32-bit
      __m256i v_sum = _mm256_add_epi32(v_b, _mm256_add_epi32(v_g, v_r));

      // CAST: Convert 32-bit integer sum to single-precision float
      __m256 v_float = _mm256_cvtepi32_ps(v_sum);

      // DIVIDE BY 3: Multiply by floating-point reciprocal
      __m256 v_avg = _mm256_mul_ps(v_float, v_div3);

      // CAST: Convert float average back to 32-bit integer
      __m256i v_i32 = _mm256_cvttps_epi32(v_avg);

      // BITWISE SHIFT: Shift 32-bit averaged values right
      __m256i v_shifted = _mm256_srli_epi32(v_i32, shifts);

      // PACK: Shuffle 32-bit results into contiguous 16-bit values
      __m256i v_shuf = _mm256_shuffle_epi8(v_shifted,
          _mm256_setr_epi8(
              0, 1, 8, 9, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
              0, 1, 8, 9, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80));

      // EXTRACT MEMORY BANKS: Combine results from both lanes
      __m128i lo = _mm256_castsi256_si128(v_shuf);
      __m128i hi = _mm256_extracti128_si256(v_shuf, 1);
      __m128i v_combined = _mm_unpacklo_epi32(lo, hi);

      // STORE RESULT: Store 4 16-bit integers to memory
      _mm_storel_epi64(reinterpret_cast<__m128i *>(dstRow + x), v_combined);
    }

    // Cleanup remaining
    for (; x < width; ++x)
    {
      uint32_t sum = srcRow[x].r + srcRow[x].g + srcRow[x].b;
      uint16_t val = static_cast<uint16_t>((sum / 3) >> shifts);
      dstRow[x] = val;
    }
  }
}

void Cast_Shift_Color_U64toI16(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride,
    int32_t shifts) {

  // BROADCAST MASK: 16-bit channel mask within 64-bit pixel
  __m256i v_chMask = _mm256_set1_epi64x(0x000000000000FFFF);

  // BROADCAST DIVISOR: Floating-point reciprocal of 3
  __m256 v_div3 = _mm256_set1_ps(1.0f / 3.0f);

  // BROADCAST LIMIT: Maximum positive I16 value
  __m256i v_max_i16 = _mm256_set1_epi32(32767);

  for (int32_t y = 0; y < height; ++y)
  {
    const LvImage::Pixel_ColorU64 *srcRow =
        reinterpret_cast<const LvImage::Pixel_ColorU64 *>(srcBase +
                                                          (y * srcStride));
    int16_t *dstRow = reinterpret_cast<int16_t *>(dstBase + (y * dstStride));
    int32_t x = 0;
    for (; x <= width - 4; x += 4) {
      // READ MEMORY: Load 4 ColorU64 pixels (32 bytes)
      __m256i v_in =
          _mm256_loadu_si256(reinterpret_cast<const __m256i *>(srcRow + x));

      // CHANNEL ISOLATE: Extract B, G, R channels from 64-bit pixel lanes
      __m256i v_b = _mm256_and_si256(v_in, v_chMask);
      __m256i v_g =
          _mm256_and_si256(_mm256_srli_epi64(v_in, 16), v_chMask);
      __m256i v_r =
          _mm256_and_si256(_mm256_srli_epi64(v_in, 32), v_chMask);

      // ACCUMULATE SUM: Add R + G + B in 32-bit
      __m256i v_sum = _mm256_add_epi32(v_b, _mm256_add_epi32(v_g, v_r));

      // CAST: Convert 32-bit integer sum to single-precision float
      __m256 v_float = _mm256_cvtepi32_ps(v_sum);

      // DIVIDE BY 3: Multiply by floating-point reciprocal
      __m256 v_avg = _mm256_mul_ps(v_float, v_div3);

      // CAST: Convert float average back to 32-bit integer
      __m256i v_i32 = _mm256_cvttps_epi32(v_avg);

      // BITWISE SHIFT: Shift 32-bit averaged values right
      __m256i v_shifted = _mm256_srli_epi32(v_i32, shifts);

      // CLAMP LIMITS: Cap at maximum positive I16 value
      __m256i v_clamped = _mm256_min_epi32(v_shifted, v_max_i16);

      // PACK: Shuffle 32-bit results into contiguous 16-bit values
      __m256i v_shuf = _mm256_shuffle_epi8(v_clamped,
          _mm256_setr_epi8(
              0, 1, 8, 9, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
              0, 1, 8, 9, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80));

      // EXTRACT MEMORY BANKS: Combine results from both lanes
      __m128i lo = _mm256_castsi256_si128(v_shuf);
      __m128i hi = _mm256_extracti128_si256(v_shuf, 1);
      __m128i v_combined = _mm_unpacklo_epi32(lo, hi);

      // STORE RESULT: Store 4 16-bit integers to memory
      _mm_storel_epi64(reinterpret_cast<__m128i *>(dstRow + x), v_combined);
    }

    // Cleanup remaining
    for (; x < width; ++x)
    {
      uint32_t sum = srcRow[x].r + srcRow[x].g + srcRow[x].b;
      int16_t val = static_cast<int16_t>(
          std::min(static_cast<int32_t>((sum / 3) >> shifts), 32767));
      dstRow[x] = val;
    }
  }
}

void Cast_Floating_ColorU64toSGL(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride) {

  // BROADCAST MASK: 16-bit channel mask within 64-bit pixel
  __m256i v_chMask = _mm256_set1_epi64x(0x000000000000FFFF);

  // BROADCAST DIVISOR: Floating-point reciprocal of 3
  __m256 v_div3 = _mm256_set1_ps(1.0f / 3.0f);

  for (int32_t y = 0; y < height; ++y) {
    const LvImage::Pixel_ColorU64 *srcRow =
        reinterpret_cast<const LvImage::Pixel_ColorU64 *>(srcBase +
                                                          (y * srcStride));
    float *dstRow = reinterpret_cast<float *>(dstBase + (y * dstStride));
    int32_t x = 0;
    for (; x <= width - 4; x += 4) {
      // READ MEMORY: Load 4 ColorU64 pixels (32 bytes)
      __m256i v_in =
          _mm256_loadu_si256(reinterpret_cast<const __m256i *>(srcRow + x));

      // CHANNEL ISOLATE: Extract B, G, R channels from 64-bit pixel lanes
      __m256i v_b = _mm256_and_si256(v_in, v_chMask);
      __m256i v_g =
          _mm256_and_si256(_mm256_srli_epi64(v_in, 16), v_chMask);
      __m256i v_r =
          _mm256_and_si256(_mm256_srli_epi64(v_in, 32), v_chMask);

      // ACCUMULATE SUM: Add R + G + B in 32-bit
      __m256i v_sum = _mm256_add_epi32(v_b, _mm256_add_epi32(v_g, v_r));

      // CAST: Convert 32-bit integer sum to single-precision float
      __m256 v_float = _mm256_cvtepi32_ps(v_sum);

      // DIVIDE BY 3: Multiply by floating-point reciprocal
      __m256 v_avg = _mm256_mul_ps(v_float, v_div3);

      // PACK: Extract results from even 32-bit slots (0,2,4,6) to contiguous
      __m256 v_perm = _mm256_permutevar8x32_ps(v_avg,
          _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7));

      // STORE RESULT: Store 4 floats to memory
      _mm_storeu_ps(dstRow + x, _mm256_castps256_ps128(v_perm));
    }

    // Cleanup remaining
    for (; x < width; ++x)
    {
      uint32_t sum = srcRow[x].r + srcRow[x].g + srcRow[x].b;
      dstRow[x] = static_cast<float>(sum / 3);
    }
  }
}

void Cast_Shift_Color_U64toU32(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride,
    int32_t shifts) {

  // Shifts >= 16 zero out all 16-bit channels
  if (shifts >= 16) {
    for (int32_t y = 0; y < height; ++y) {
      std::memset(dstBase + (y * dstStride), 0,
                  width * sizeof(LvImage::Pixel_ColorU32));
    }
    return;
  }

  // BROADCAST MASK: Zero alpha channel in each 32-bit pixel (keep bytes 0,1,2)
  __m256i alphaMask = _mm256_setr_epi8(
      -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1, 0,
      -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1, 0);

  for (int32_t y = 0; y < height; ++y)
  {
    const LvImage::Pixel_ColorU64 *srcRow =
        reinterpret_cast<const LvImage::Pixel_ColorU64 *>(srcBase +
                                                          (y * srcStride));
    LvImage::Pixel_ColorU32 *dstRow =
        reinterpret_cast<LvImage::Pixel_ColorU32 *>(dstBase + (y * dstStride));

    int32_t x = 0;
    for (; x <= width - 8; x += 8) {
      // READ MEMORY: Load 8 ColorU64 pixels (64 bytes, two 256-bit loads)
      __m256i in_lo = _mm256_loadu_si256(
          reinterpret_cast<const __m256i *>(srcRow + x));
      __m256i in_hi = _mm256_loadu_si256(
          reinterpret_cast<const __m256i *>(srcRow + x + 4));

      // BITWISE SHIFT: Shift 16-bit channel values right
      in_lo = _mm256_srli_epi16(in_lo, shifts);
      in_hi = _mm256_srli_epi16(in_hi, shifts);

      // PACK SATURATE: Pack 16-bit to 8-bit with unsigned saturation
      __m256i v_pack = _mm256_packus_epi16(in_lo, in_hi);

      // FIX LANE ORDER: Correct lane-wise pack ordering with permute
      __m256i v_perm =
          _mm256_permute4x64_epi64(v_pack, _MM_SHUFFLE(3, 1, 2, 0));

      // APPLY MASK: Zero alpha channel in each pixel
      v_perm = _mm256_and_si256(v_perm, alphaMask);

      // STORE RESULT: Store 8 ColorU32 pixels to memory
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(dstRow + x), v_perm);
    }

    // Cleanup remaining
    for (; x < width; ++x)
    {
      uint32_t b = static_cast<uint32_t>(srcRow[x].b) >> shifts;
      uint32_t g = static_cast<uint32_t>(srcRow[x].g) >> shifts;
      uint32_t r = static_cast<uint32_t>(srcRow[x].r) >> shifts;

      dstRow[x] = {static_cast<uint8_t>(std::min(b, 255u)),
                   static_cast<uint8_t>(std::min(g, 255u)),
                   static_cast<uint8_t>(std::min(r, 255u)), 0};
    }
  }
}

void Cast_Shift_Color_U64toU64(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride,
    int32_t shifts)
{

  // Shifts >= 16 zero out all 16-bit channels
  if (shifts >= 16) {
    for (int32_t y = 0; y < height; ++y) {
      std::memset(dstBase + (y * dstStride), 0,
                  width * sizeof(LvImage::Pixel_ColorU64));
    }
    return;
  }

  // Calculate mask to prevent crossing word boundaries
  uint16_t maskWord = 0xFFFF >> shifts;

  // BROADCAST MASK: Duplicate combined mask across 16 words (alpha=0)
  __m256i v_mask = _mm256_setr_epi16(
      maskWord, maskWord, maskWord, 0, maskWord, maskWord, maskWord, 0,
      maskWord, maskWord, maskWord, 0, maskWord, maskWord, maskWord, 0);

  for (int32_t y = 0; y < height; ++y) {
    const LvImage::Pixel_ColorU64 *srcRow =
        reinterpret_cast<const LvImage::Pixel_ColorU64 *>(srcBase +
                                                          (y * srcStride));
    LvImage::Pixel_ColorU64 *dstRow =
        reinterpret_cast<LvImage::Pixel_ColorU64 *>(dstBase + (y * dstStride));
    int32_t x = 0;
    for (; x <= width - 4; x += 4) {
      // READ MEMORY: Load 4 ColorU64 pixels (32 bytes)
      __m256i v_in =
          _mm256_loadu_si256(reinterpret_cast<const __m256i *>(srcRow + x));

      // BITWISE SHIFT: Shift 32-bit array values right (shifts all 4 channels)
      __m256i v_shifted = _mm256_srli_epi32(v_in, shifts);

      // APPLY MASK: Clean neighboring words and zero alpha channel
      __m256i v_final = _mm256_and_si256(v_shifted, v_mask);

      // STORE RESULT: Store 4 ColorU64 pixels to memory
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(dstRow + x), v_final);
    }

    // Cleanup remaining
    for (; x < width; ++x)
    {
      dstRow[x] = {
          static_cast<uint16_t>(srcRow[x].b >> shifts),
          static_cast<uint16_t>(srcRow[x].g >> shifts),
          static_cast<uint16_t>(srcRow[x].r >> shifts), 0};
    }
  }
}

void Cast_Floating_ColorU64toComplex(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride) {

  // BROADCAST MASK: 16-bit channel mask within 64-bit pixel
  __m256i v_chMask = _mm256_set1_epi64x(0x000000000000FFFF);

  // BROADCAST DIVISOR: Floating-point reciprocal of 3
  __m256 v_div3 = _mm256_set1_ps(1.0f / 3.0f);

  for (int32_t y = 0; y < height; ++y) {
    const LvImage::Pixel_ColorU64 *srcRow =
        reinterpret_cast<const LvImage::Pixel_ColorU64 *>(srcBase +
                                                          (y * srcStride));
    LvImage::Pixel_ComplexSGL *dstRow =
        reinterpret_cast<LvImage::Pixel_ComplexSGL *>(dstBase +
                                                      (y * dstStride));
    int32_t x = 0;
    for (; x <= width - 4; x += 4)
    {
      // READ MEMORY: Load 4 ColorU64 pixels (32 bytes)
      __m256i v_in =
          _mm256_loadu_si256(reinterpret_cast<const __m256i *>(srcRow + x));

      // CHANNEL ISOLATE: Extract B, G, R channels from 64-bit pixel lanes
      __m256i v_b = _mm256_and_si256(v_in, v_chMask);
      __m256i v_g =
          _mm256_and_si256(_mm256_srli_epi64(v_in, 16), v_chMask);
      __m256i v_r =
          _mm256_and_si256(_mm256_srli_epi64(v_in, 32), v_chMask);

      // ACCUMULATE SUM: Add R + G + B in 32-bit
      __m256i v_sum = _mm256_add_epi32(v_b, _mm256_add_epi32(v_g, v_r));

      // CAST: Convert 32-bit integer sum to single-precision float
      __m256 v_float = _mm256_cvtepi32_ps(v_sum);

      // DIVIDE BY 3: Multiply by floating-point reciprocal
      __m256 v_avg = _mm256_mul_ps(v_float, v_div3);

      // PACK: Extract results from even 32-bit slots to contiguous floats
      __m256 v_perm = _mm256_permutevar8x32_ps(v_avg,
          _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7));
      __m128 v_real = _mm256_castps256_ps128(v_perm);

      // COMPLEX ZERO: Zero array for imaginary components
      __m128 v_zero = _mm_setzero_ps();

      // UNPACK AND INTERLEAVE: Interleave real floats with imaginary zeros
      __m128 v_lo = _mm_unpacklo_ps(v_real, v_zero);
      __m128 v_hi = _mm_unpackhi_ps(v_real, v_zero);

      // STORE RESULT: Store 4 complex values to memory
      _mm_storeu_ps(reinterpret_cast<float *>(dstRow + x), v_lo);
      _mm_storeu_ps(reinterpret_cast<float *>(dstRow + x + 2), v_hi);
    }

    // Cleanup remaining
    for (; x < width; ++x) {
      uint32_t sum = srcRow[x].r + srcRow[x].g + srcRow[x].b;
      dstRow[x] = {static_cast<float>(sum / 3), 0.0f};
    }
  }
}

#pragma endregion

#pragma region ComplexSGL

void Cast_Floating_ComplextoU8(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride)
{
  for (int32_t y = 0; y < height; ++y) {
    const LvImage::Pixel_ComplexSGL *srcRow =
        reinterpret_cast<const LvImage::Pixel_ComplexSGL *>(srcBase +
                                                            (y * srcStride));
    uint8_t *dstRow = dstBase + (y * dstStride);

    int32_t x = 0;
    for (; x <= width - 8; x += 8) {
      // READ MEMORY: Load 8 complex values as 16 interleaved floats
      __m256 v_in1 =
          _mm256_loadu_ps(reinterpret_cast<const float *>(srcRow + x));
      __m256 v_in2 =
          _mm256_loadu_ps(reinterpret_cast<const float *>(srcRow + x + 4));

      // MAGNITUDE SQUARED: Compute re² + im² per complex element
      __m256 v_sq1 = _mm256_mul_ps(v_in1, v_in1);
      __m256 v_sq2 = _mm256_mul_ps(v_in2, v_in2);
      __m256 v_sum1 = _mm256_hadd_ps(v_sq1, v_sq1);
      __m256 v_sum2 = _mm256_hadd_ps(v_sq2, v_sq2);
      __m256 v_shuf1 = _mm256_permute_ps(v_sum1, _MM_SHUFFLE(2, 0, 2, 0));
      __m256 v_shuf2 = _mm256_permute_ps(v_sum2, _MM_SHUFFLE(2, 0, 2, 0));

      // SQUARE ROOT: Compute magnitude from squared sums
      __m128 v_lo1 = _mm256_castps256_ps128(v_shuf1);
      __m128 v_hi1 = _mm256_extractf128_ps(v_shuf1, 1);
      __m128 v_mag1 =
          _mm_sqrt_ps(_mm_shuffle_ps(v_lo1, v_hi1, _MM_SHUFFLE(2, 0, 2, 0)));
      __m128 v_lo2 = _mm256_castps256_ps128(v_shuf2);
      __m128 v_hi2 = _mm256_extractf128_ps(v_shuf2, 1);
      __m128 v_mag2 =
          _mm_sqrt_ps(_mm_shuffle_ps(v_lo2, v_hi2, _MM_SHUFFLE(2, 0, 2, 0)));

      // COMBINE: Merge magnitude vectors into single 256-bit register
      __m256 v_mag =
          _mm256_insertf128_ps(_mm256_castps128_ps256(v_mag1), v_mag2, 1);

      // CLAMP LIMITS: Clamp float values between 0 and 255
      __m256 v_clamped = _mm256_max_ps(_mm256_setzero_ps(),
          _mm256_min_ps(v_mag, _mm256_set1_ps(255.0f)));

      // CAST: Convert single-precision float to 32-bit integer
      __m256i v_i32 = _mm256_cvttps_epi32(v_clamped);

      // PACK SATURATE: Pack 32-bit to 16-bit with saturation
      __m256i v_i16 = _mm256_packus_epi32(v_i32, _mm256_setzero_si256());

      // FIX LANE ORDER: Correct lane-wise pack ordering with permute
      v_i16 = _mm256_permute4x64_epi64(v_i16, _MM_SHUFFLE(3, 1, 2, 0));

      // PACK SATURATE: Pack 16-bit to 8-bit with saturation
      __m128i v_i16_lo = _mm256_castsi256_si128(v_i16);
      __m128i v_i8 = _mm_packus_epi16(v_i16_lo, _mm_setzero_si128());

      // STORE RESULT: Store 8 bytes to memory
      _mm_storel_epi64(reinterpret_cast<__m128i *>(dstRow + x), v_i8);
    }

    // Cleanup remaining
    for (; x < width; ++x) {
      float mag =
          std::sqrt(srcRow[x].re * srcRow[x].re + srcRow[x].im * srcRow[x].im);
      dstRow[x] =
          static_cast<uint8_t>(std::min(std::max(mag, 0.0f), 255.0f));
    }
  }
}

void Cast_Floating_ComplextoU16(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride)
{
  for (int32_t y = 0; y < height; ++y) {
    const LvImage::Pixel_ComplexSGL *srcRow =
        reinterpret_cast<const LvImage::Pixel_ComplexSGL *>(srcBase +
                                                            (y * srcStride));
    uint16_t *dstRow = reinterpret_cast<uint16_t *>(dstBase + (y * dstStride));
    int32_t x = 0;
    for (; x <= width - 8; x += 8) {
      // READ MEMORY: Load 8 complex values as 16 interleaved floats
      __m256 v_in1 =
          _mm256_loadu_ps(reinterpret_cast<const float *>(srcRow + x));
      __m256 v_in2 =
          _mm256_loadu_ps(reinterpret_cast<const float *>(srcRow + x + 4));

      // MAGNITUDE SQUARED: Compute re² + im² per complex element
      __m256 v_sq1 = _mm256_mul_ps(v_in1, v_in1);
      __m256 v_sq2 = _mm256_mul_ps(v_in2, v_in2);
      __m256 v_sum1 = _mm256_hadd_ps(v_sq1, v_sq1);
      __m256 v_sum2 = _mm256_hadd_ps(v_sq2, v_sq2);
      __m256 v_shuf1 = _mm256_permute_ps(v_sum1, _MM_SHUFFLE(2, 0, 2, 0));
      __m256 v_shuf2 = _mm256_permute_ps(v_sum2, _MM_SHUFFLE(2, 0, 2, 0));

      // SQUARE ROOT: Compute magnitude from squared sums
      __m128 v_lo1 = _mm256_castps256_ps128(v_shuf1);
      __m128 v_hi1 = _mm256_extractf128_ps(v_shuf1, 1);
      __m128 v_mag1 =
          _mm_sqrt_ps(_mm_shuffle_ps(v_lo1, v_hi1, _MM_SHUFFLE(2, 0, 2, 0)));
      __m128 v_lo2 = _mm256_castps256_ps128(v_shuf2);
      __m128 v_hi2 = _mm256_extractf128_ps(v_shuf2, 1);
      __m128 v_mag2 =
          _mm_sqrt_ps(_mm_shuffle_ps(v_lo2, v_hi2, _MM_SHUFFLE(2, 0, 2, 0)));

      // COMBINE: Merge magnitude vectors into single 256-bit register
      __m256 v_mag =
          _mm256_insertf128_ps(_mm256_castps128_ps256(v_mag1), v_mag2, 1);

      // CLAMP LIMITS: Clamp float values between 0 and 65535
      __m256 v_clamped = _mm256_max_ps(_mm256_setzero_ps(),
          _mm256_min_ps(v_mag, _mm256_set1_ps(65535.0f)));

      // CAST: Convert single-precision float to 32-bit integer
      __m256i v_i32 = _mm256_cvttps_epi32(v_clamped);

      // PACK SATURATE: Pack 32-bit to 16-bit with saturation
      __m256i v_i16 = _mm256_packus_epi32(v_i32, _mm256_setzero_si256());

      // FIX LANE ORDER: Correct lane-wise pack ordering with permute
      __m256i v_perm =
          _mm256_permute4x64_epi64(v_i16, _MM_SHUFFLE(3, 1, 2, 0));

      // EXTRACT: Extract lower 128-bit lane
      __m128i v_final = _mm256_castsi256_si128(v_perm);

      // STORE RESULT: Store 8 16-bit integers to memory
      _mm_storeu_si128(reinterpret_cast<__m128i *>(dstRow + x), v_final);
    }

    // Cleanup remaining
    for (; x < width; ++x)
    {
      float mag =
          std::sqrt(srcRow[x].re * srcRow[x].re + srcRow[x].im * srcRow[x].im);
      dstRow[x] =
          static_cast<uint16_t>(std::min(std::max(mag, 0.0f), 65535.0f));
    }
  }
}

void Cast_Floating_ComplextoI16(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride) {
  for (int32_t y = 0; y < height; ++y) {
    const LvImage::Pixel_ComplexSGL *srcRow =
        reinterpret_cast<const LvImage::Pixel_ComplexSGL *>(srcBase +
                                                            (y * srcStride));
    int16_t *dstRow = reinterpret_cast<int16_t *>(dstBase + (y * dstStride));
    int32_t x = 0;
    for (; x <= width - 8; x += 8) {
      // READ MEMORY: Load 8 complex values as 16 interleaved floats
      __m256 v_in1 =
          _mm256_loadu_ps(reinterpret_cast<const float *>(srcRow + x));
      __m256 v_in2 =
          _mm256_loadu_ps(reinterpret_cast<const float *>(srcRow + x + 4));

      // MAGNITUDE SQUARED: Compute re² + im² per complex element
      __m256 v_sq1 = _mm256_mul_ps(v_in1, v_in1);
      __m256 v_sq2 = _mm256_mul_ps(v_in2, v_in2);
      __m256 v_sum1 = _mm256_hadd_ps(v_sq1, v_sq1);
      __m256 v_sum2 = _mm256_hadd_ps(v_sq2, v_sq2);
      __m256 v_shuf1 = _mm256_permute_ps(v_sum1, _MM_SHUFFLE(2, 0, 2, 0));
      __m256 v_shuf2 = _mm256_permute_ps(v_sum2, _MM_SHUFFLE(2, 0, 2, 0));

      // SQUARE ROOT: Compute magnitude from squared sums
      __m128 v_lo1 = _mm256_castps256_ps128(v_shuf1);
      __m128 v_hi1 = _mm256_extractf128_ps(v_shuf1, 1);
      __m128 v_mag1 =
          _mm_sqrt_ps(_mm_shuffle_ps(v_lo1, v_hi1, _MM_SHUFFLE(2, 0, 2, 0)));
      __m128 v_lo2 = _mm256_castps256_ps128(v_shuf2);
      __m128 v_hi2 = _mm256_extractf128_ps(v_shuf2, 1);
      __m128 v_mag2 =
          _mm_sqrt_ps(_mm_shuffle_ps(v_lo2, v_hi2, _MM_SHUFFLE(2, 0, 2, 0)));

      // COMBINE: Merge magnitude vectors into single 256-bit register
      __m256 v_mag =
          _mm256_insertf128_ps(_mm256_castps128_ps256(v_mag1), v_mag2, 1);

      // CLAMP LIMITS: Clamp float values between 0 and 32767
      __m256 v_clamped = _mm256_max_ps(_mm256_setzero_ps(),
          _mm256_min_ps(v_mag, _mm256_set1_ps(32767.0f)));

      // CAST: Convert single-precision float to 32-bit integer
      __m256i v_i32 = _mm256_cvttps_epi32(v_clamped);

      // PACK SATURATE: Pack 32-bit to signed 16-bit with saturation
      __m256i v_i16 = _mm256_packs_epi32(v_i32, _mm256_setzero_si256());

      // FIX LANE ORDER: Correct lane-wise pack ordering with permute
      __m256i v_perm =
          _mm256_permute4x64_epi64(v_i16, _MM_SHUFFLE(3, 1, 2, 0));

      // EXTRACT: Extract lower 128-bit lane
      __m128i v_final = _mm256_castsi256_si128(v_perm);

      // STORE RESULT: Store 8 16-bit integers to memory
      _mm_storeu_si128(reinterpret_cast<__m128i *>(dstRow + x), v_final);
    }

    // Cleanup remaining
    for (; x < width; ++x)
    {
      float mag =
          std::sqrt(srcRow[x].re * srcRow[x].re + srcRow[x].im * srcRow[x].im);
      dstRow[x] =
          static_cast<int16_t>(std::min(std::max(mag, 0.0f), 32767.0f));
    }
  }
}

void Cast_Floating_ComplextoSGL(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride) {
  for (int32_t y = 0; y < height; ++y) {
    const LvImage::Pixel_ComplexSGL *srcRow =
        reinterpret_cast<const LvImage::Pixel_ComplexSGL *>(srcBase +
                                                            (y * srcStride));
    float *dstRow = reinterpret_cast<float *>(dstBase + (y * dstStride));
    int32_t x = 0;
    for (; x <= width - 8; x += 8) {
      // READ MEMORY: Load 8 complex values as 16 interleaved floats
      __m256 v_in1 =
          _mm256_loadu_ps(reinterpret_cast<const float *>(srcRow + x));
      __m256 v_in2 =
          _mm256_loadu_ps(reinterpret_cast<const float *>(srcRow + x + 4));

      // MAGNITUDE SQUARED: Compute re² + im² per complex element
      __m256 v_sq1 = _mm256_mul_ps(v_in1, v_in1);
      __m256 v_sq2 = _mm256_mul_ps(v_in2, v_in2);
      __m256 v_sum1 = _mm256_hadd_ps(v_sq1, v_sq1);
      __m256 v_sum2 = _mm256_hadd_ps(v_sq2, v_sq2);
      __m256 v_shuf1 = _mm256_permute_ps(v_sum1, _MM_SHUFFLE(2, 0, 2, 0));
      __m256 v_shuf2 = _mm256_permute_ps(v_sum2, _MM_SHUFFLE(2, 0, 2, 0));

      // SQUARE ROOT: Compute magnitude from squared sums
      __m128 v_lo1 = _mm256_castps256_ps128(v_shuf1);
      __m128 v_hi1 = _mm256_extractf128_ps(v_shuf1, 1);
      __m128 v_mag1 =
          _mm_sqrt_ps(_mm_shuffle_ps(v_lo1, v_hi1, _MM_SHUFFLE(2, 0, 2, 0)));
      __m128 v_lo2 = _mm256_castps256_ps128(v_shuf2);
      __m128 v_hi2 = _mm256_extractf128_ps(v_shuf2, 1);
      __m128 v_mag2 =
          _mm_sqrt_ps(_mm_shuffle_ps(v_lo2, v_hi2, _MM_SHUFFLE(2, 0, 2, 0)));

      // COMBINE: Merge magnitude vectors into single 256-bit register
      __m256 v_mag =
          _mm256_insertf128_ps(_mm256_castps128_ps256(v_mag1), v_mag2, 1);

      // STORE RESULT: Store 8 floats to memory
      _mm256_storeu_ps(dstRow + x, v_mag);
    }

    // Cleanup remaining
    for (; x < width; ++x) {
      dstRow[x] =
          std::sqrt(srcRow[x].re * srcRow[x].re + srcRow[x].im * srcRow[x].im);
    }
  }
}

void Cast_Floating_ComplextoColorU32(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride)
{

  // Shuffle mask to replicate magnitude byte to B, G, R with alpha=0
  __m256i shuffleMask = _mm256_setr_epi8(
      0, 0, 0, 0x80, 4, 4, 4, 0x80, 8, 8, 8, 0x80, 12, 12, 12, 0x80,
      0, 0, 0, 0x80, 4, 4, 4, 0x80, 8, 8, 8, 0x80, 12, 12, 12, 0x80);

  for (int32_t y = 0; y < height; ++y) {
    const LvImage::Pixel_ComplexSGL *srcRow =
        reinterpret_cast<const LvImage::Pixel_ComplexSGL *>(srcBase +
                                                            (y * srcStride));
    LvImage::Pixel_ColorU32 *dstRow =
        reinterpret_cast<LvImage::Pixel_ColorU32 *>(dstBase + (y * dstStride));
    int32_t x = 0;
    for (; x <= width - 8; x += 8) {
      // READ MEMORY: Load 8 complex values as 16 interleaved floats
      __m256 v_in1 =
          _mm256_loadu_ps(reinterpret_cast<const float *>(srcRow + x));
      __m256 v_in2 =
          _mm256_loadu_ps(reinterpret_cast<const float *>(srcRow + x + 4));

      // MAGNITUDE SQUARED: Compute re² + im² per complex element
      __m256 v_sq1 = _mm256_mul_ps(v_in1, v_in1);
      __m256 v_sq2 = _mm256_mul_ps(v_in2, v_in2);
      __m256 v_sum1 = _mm256_hadd_ps(v_sq1, v_sq1);
      __m256 v_sum2 = _mm256_hadd_ps(v_sq2, v_sq2);
      __m256 v_shuf1 = _mm256_permute_ps(v_sum1, _MM_SHUFFLE(2, 0, 2, 0));
      __m256 v_shuf2 = _mm256_permute_ps(v_sum2, _MM_SHUFFLE(2, 0, 2, 0));

      // SQUARE ROOT: Compute magnitude from squared sums
      __m128 v_lo1 = _mm256_castps256_ps128(v_shuf1);
      __m128 v_hi1 = _mm256_extractf128_ps(v_shuf1, 1);
      __m128 v_mag1 =
          _mm_sqrt_ps(_mm_shuffle_ps(v_lo1, v_hi1, _MM_SHUFFLE(2, 0, 2, 0)));
      __m128 v_lo2 = _mm256_castps256_ps128(v_shuf2);
      __m128 v_hi2 = _mm256_extractf128_ps(v_shuf2, 1);
      __m128 v_mag2 =
          _mm_sqrt_ps(_mm_shuffle_ps(v_lo2, v_hi2, _MM_SHUFFLE(2, 0, 2, 0)));

      // COMBINE: Merge magnitude vectors into single 256-bit register
      __m256 v_mag =
          _mm256_insertf128_ps(_mm256_castps128_ps256(v_mag1), v_mag2, 1);

      // CLAMP LIMITS: Clamp float values between 0 and 255
      __m256 v_clamped = _mm256_max_ps(_mm256_setzero_ps(),
          _mm256_min_ps(v_mag, _mm256_set1_ps(255.0f)));

      // CAST: Convert single-precision float to 32-bit integer
      __m256i v_i32 = _mm256_cvttps_epi32(v_clamped);

      // MEMORY SHUFFLE: Replicate magnitude to B, G, R channels with alpha=0
      __m256i v_final = _mm256_shuffle_epi8(v_i32, shuffleMask);

      // STORE RESULT: Store 8 ColorU32 pixels to memory
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(dstRow + x), v_final);
    }

    // Cleanup remaining
    for (; x < width; ++x)
    {
      float mag =
          std::sqrt(srcRow[x].re * srcRow[x].re + srcRow[x].im * srcRow[x].im);
      uint8_t val =
          static_cast<uint8_t>(std::min(std::max(mag, 0.0f), 255.0f));
      dstRow[x] = {val, val, val, 0};
    }
  }
}

void Cast_Floating_ComplextoColorU64(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride) {

  // Shuffle mask to replicate 16-bit magnitude to B, G, R with alpha=0
  __m256i shuffleMask = _mm256_setr_epi8(
      0, 1, 0, 1, 0, 1, 0x80, 0x80, 4, 5, 4, 5, 4, 5, 0x80, 0x80,
      8, 9, 8, 9, 8, 9, 0x80, 0x80, 12, 13, 12, 13, 12, 13, 0x80, 0x80);

  for (int32_t y = 0; y < height; ++y) {
    const LvImage::Pixel_ComplexSGL *srcRow =
        reinterpret_cast<const LvImage::Pixel_ComplexSGL *>(srcBase +
                                                            (y * srcStride));
    LvImage::Pixel_ColorU64 *dstRow =
        reinterpret_cast<LvImage::Pixel_ColorU64 *>(dstBase + (y * dstStride));
    int32_t x = 0;
    for (; x <= width - 4; x += 4) {
      // READ MEMORY: Load 4 complex values as 8 interleaved floats
      __m256 v_in =
          _mm256_loadu_ps(reinterpret_cast<const float *>(srcRow + x));

      // MAGNITUDE SQUARED: Compute re² + im² per complex element
      __m256 v_sq = _mm256_mul_ps(v_in, v_in);
      __m256 v_sum = _mm256_hadd_ps(v_sq, v_sq);
      __m256 v_shuf = _mm256_permute_ps(v_sum, _MM_SHUFFLE(2, 0, 2, 0));

      // SQUARE ROOT: Compute magnitude from squared sums
      __m128 v_lo = _mm256_castps256_ps128(v_shuf);
      __m128 v_hi = _mm256_extractf128_ps(v_shuf, 1);
      __m128 v_mag =
          _mm_sqrt_ps(_mm_shuffle_ps(v_lo, v_hi, _MM_SHUFFLE(2, 0, 2, 0)));

      // CLAMP LIMITS: Clamp float values between 0 and 65535
      __m128 v_clamped = _mm_max_ps(_mm_setzero_ps(),
          _mm_min_ps(v_mag, _mm_set1_ps(65535.0f)));

      // CAST: Convert single-precision float to 32-bit integer
      __m128i v_i32 = _mm_cvttps_epi32(v_clamped);

      // HARDWARE EXPAND: Expand 32-bit integers to 64-bit integers
      __m256i v_i64 = _mm256_cvtepi32_epi64(v_i32);

      // MEMORY SHUFFLE: Replicate magnitude to B, G, R channels with alpha=0
      __m256i v_final = _mm256_shuffle_epi8(v_i64, shuffleMask);

      // STORE RESULT: Store 4 ColorU64 pixels to memory
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(dstRow + x), v_final);
    }

    // Cleanup remaining
    for (; x < width; ++x)
    {
      float mag =
          std::sqrt(srcRow[x].re * srcRow[x].re + srcRow[x].im * srcRow[x].im);
      uint16_t val =
          static_cast<uint16_t>(std::min(std::max(mag, 0.0f), 65535.0f));
      dstRow[x] = {val, val, val, 0};
    }
  }
}

void Cast_Floating_ComplextoComplex(const uint8_t *srcBase, uint8_t *dstBase,
    int32_t width, int32_t height,
    int32_t srcStride, int32_t dstStride)
{
  for (int32_t y = 0; y < height; ++y) {
    const float *srcRow =
        reinterpret_cast<const float *>(srcBase + (y * srcStride));
    float *dstRow = reinterpret_cast<float *>(dstBase + (y * dstStride));
    int32_t x = 0;
    for (; x <= (width * 2) - 8; x += 8) {
      // READ MEMORY: Load 8 floats (4 complex values)
      __m256 v_in = _mm256_loadu_ps(srcRow + x);

      // STORE RESULT: Store 8 floats to memory
      _mm256_storeu_ps(dstRow + x, v_in);
    }

    // Cleanup remaining
    for (; x < width * 2; ++x) {
      dstRow[x] = srcRow[x];
    }
  }
}

#pragma endregion
