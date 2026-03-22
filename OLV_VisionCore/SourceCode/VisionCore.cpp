#include "../Include/VisionCore.h"

using namespace LvError;

_declspec(dllexport) void OLV_COPY(ErrorClusterPtr errorCluster, uint64_t srcPtr, 
    uint64_t dstPtr, int32_t length)
{
    handleError(errorCluster, [&]() -> MgErr {

		//Safety checks
		if (srcPtr == 0 || dstPtr == 0 || length < 0)
        {
            return mgArgErr;
        }

        std::copy_n(reinterpret_cast<const uint8_t*>(srcPtr), length, reinterpret_cast<uint8_t*>(dstPtr));
        return mgNoErr;
   });
}

_declspec(dllexport) void OLV_CAST(LvError::ErrorClusterPtr errorCluster,
    LvImage::ImageClusterPtr src, LvImage::ImageClusterPtr dst,
    CastMethode methode, int32_t numberOfShifts,
    uint8_t bitdepthSrc, uint8_t bitdepthDst,
    float* LookupTable, int32_t LookupTableSize)
{
    handleError(errorCluster, [&]() -> MgErr {

        //Safety checks
        if (!src || !dst || src->imagePointer == 0 || dst->imagePointer == 0) 
        {
            return mgArgErr; 
        }

        //Size check
        if (src->width != dst->width || src->height != dst->height)
        {
            return mgArgErr;
        }		

        switch (methode)
        {
            case UseBitDepth:
                return Cast_BitDepth(src, dst, bitdepthSrc, bitdepthDst);

            case Shift:
                return Cast_Shift(src, dst, numberOfShifts);

            case UseLookUpTable:
                return Cast_LUT(src, dst, LookupTable, LookupTableSize);

            default:
                return mgArgErr;
            }

        return mgNoErr;
    });
}