#pragma once
#include <functional>
#include <exception>
#include <string>
#include <vector> 
#include "extcode.h" 
#include "LvString.h"

namespace LvError
{
#if defined(_WIN32) && !defined(_WIN64)
#pragma pack(push, 1) // Set packing alignment to 1 byte for x86
#define LV_PACKING_SET 1 // Define a flag to know we need to pop
#endif

    typedef struct ErrorCluster
    {
        LVBoolean status;
        int32 code;
        LStrHandle source;
    };
#pragma pack(pop)

#ifdef LV_PACKING_SET
#pragma pack(pop)   // Restore previous packing alignment
#undef LV_PACKING_SET // Undefine the flag
#endif

    typedef ErrorCluster* ErrorClusterPtr;

    /**
     * @brief Sets the fields of a LabVIEW Error Cluster.
     * @param errorCluster Pointer to the LabVIEW Error Cluster.
     * @param status The boolean error status (TRUE for error).
     * @param code The integer error code.
     * @param message The error message string.
     */
    void setErrorCluster(ErrorClusterPtr errorCluster, LVBoolean status, int32 code, const std::string& message);

    /**
     * @brief Wraps a C++ function call, handling exceptions and MgErr return codes,
     * reporting errors via a LabVIEW Error Cluster.
     * @param error Pointer to the LabVIEW Error Cluster to populate.
     * @param function The C++ lambda or function to execute safely.
     * This function should return MgErr (mgNoErr on success).
     */
    void handleError(ErrorClusterPtr errorCluster, const std::function<MgErr()>& function);

    /**
    * @brief Converts a LabVIEW MgErr code to its symbolic string name.
    * @param errorCode The MgErr code (int32).
    * @return A string containing the symbolic name.
    */
    std::string mgErrToString(int32 errorCode);

}