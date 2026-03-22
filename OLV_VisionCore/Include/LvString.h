#pragma once
#include "extcode.h"
#include <string> 


namespace LvString
{
    /**
     * @brief Resizes a LabVIEW string handle and copies C++ std::string data into it.
     * @param lvHandle The LabVIEW string handle to modify.
     * @param cppString The C++ string containing the data to copy.
     * @return MgErr (LabVIEW error code, mgNoErr on success).
     */
    MgErr setLvStringHandle(LStrHandle lvHandle, const std::string& cppString);

    /**
     * @brief Resizes a LabVIEW string handle and copies C-style string data into it.
     * @param lvHandle The LabVIEW string handle to modify.
     * @param cString A null-terminated C-style string containing the data to copy.
     * @return MgErr (LabVIEW error code, mgNoErr on success).
     */
    MgErr setLvStringHandle(LStrHandle lvHandle, const char* cString);

    /**
     * @brief Reads data from a LabVIEW string handle into a C++ std::string.
     * @param lvHandle The LabVIEW string handle to read from.
     * @param cppString [Output] The std::string to copy the data into. Cleared if handle is invalid or empty.
     * @return MgErr (LabVIEW error code, mgNoErr on success).
     */
    MgErr getLvStringHandle(LStrHandle lvHandle, std::string& cppString);

    /**
     * @brief Reads data from a LabVIEW string handle into a C-style character buffer.
     * Ensures null termination and prevents buffer overflows.
     * @param lvHandle The LabVIEW string handle to read from.
     * @param buffer [Output] The pre-allocated character buffer to copy data into.
     * @param bufferSize The total size (in bytes) of the 'buffer'.
     * @return MgErr (LabVIEW error code, mgNoErr on success, mgArgErr if buffer too small/invalid).
     */
    MgErr getLvStringHandle(LStrHandle lvHandle, char* buffer, size_t bufferSize);

} // namespace string 