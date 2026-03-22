#pragma once
#include "extcode.h"
#include <cstdint>

namespace LvImage
{
	enum ImageFormat : uint16_t
    {
		MonoU8 = 0,
		MonoU16 = 1,
		MonoI16 = 2,
		MonoSGL = 3,
		ColorU32 = 4,
		ColorU64 = 5,
		ComplexCSGL = 6,
    };

    #if defined(_WIN32) && !defined(_WIN64)
    #pragma pack(push, 1) // Set packing alignment to 1 byte for x86
    #define LV_PACKING_SET 1 // Define a flag to know we need to pop
    #endif

        struct ImageCluster {
            uint64_t imagePointer;
            int32_t borderSize;
            int32_t lineWidth;
            int32_t width;
            int32_t height;
            ImageFormat imageFormat;
        };
    #pragma pack(pop)

    #ifdef LV_PACKING_SET
    #pragma pack(pop)   // Restore previous packing alignment
    #undef LV_PACKING_SET // Undefine the flag
    #endif

    struct Pixel_ColorU32 { uint8_t b, g, r, a; };
    struct Pixel_ColorU64 { uint16_t b, g, r, a; };
    struct Pixel_ComplexSGL { float re, im; };

    typedef ImageCluster* ImageClusterPtr;
}