#include "../Include/LvString.h"

/**
 * @brief Namespace for LabVIEW string handling utilities
 */
namespace LvString
{
    MgErr setLvStringHandle(LStrHandle lvHandle, const std::string& cppString)
    {
        // Handle null input
        if (!lvHandle)
        {
            return mgArgErr;
        }           

        // Add header
        size_t neededSize = sizeof(int32_t) + cppString.length();            

        // Resize the LabVIEW handle using the Memory Manager
        MgErr resizeError = DSSetHandleSize(lvHandle, neededSize);

        if (resizeError != mgNoErr)
        {
            return resizeError;
        }         

        LStrPtr lvStringPtr = *lvHandle;

        // Check if dereferencing failed after resize
        if (!lvStringPtr) {
            return mFullErr; 
        }

        lvStringPtr->cnt = static_cast<int32_t>(cppString.length());


        if (lvStringPtr->cnt > 0)
        {
            std::memcpy(LStrBuf(lvStringPtr), cppString.data(), lvStringPtr->cnt);
        }

        return mgNoErr;
    }

    MgErr setLvStringHandle(LStrHandle lvHandle, const char* cString)
    {
        // Handle null input
        if (!lvHandle)
        {
            return mgArgErr;
        }
        if (!cString)
        {
            cString = ""; // Treat null C string pointer as empty
        }

        // Calculate length of the C string
        size_t cStringLength = std::strlen(cString);

        // Add header
        size_t neededSize = sizeof(int32_t) + cStringLength;

        // Resize the LabVIEW handle
        MgErr resizeError = DSSetHandleSize(lvHandle, neededSize);
        if (resizeError != mgNoErr)
        {
            return resizeError;
        }

        LStrPtr lvStringPtr = *lvHandle;
        if (!lvStringPtr) 
        {
            return mFullErr;
        }

        lvStringPtr->cnt = static_cast<int32_t>(cStringLength);

        // Copy the C string characters
        if (lvStringPtr->cnt > 0)
        {
            std::memcpy(LStrBuf(lvStringPtr), cString, lvStringPtr->cnt);
        }

        return mgNoErr;
    }

    MgErr getLvStringHandle(LStrHandle lvHandle, std::string& cppString)
    {
        cppString.clear(); // Clear output first

        if (!lvHandle) 
        { 
            return mgArgErr; 
        }

        LStrPtr lvStringPtr = *lvHandle;

        // Check LStr pointer and length
        if (!lvStringPtr || lvStringPtr->cnt <= 0)
        {
            return mgNoErr; // Empty or invalid LStr, but not an error
        }

        try
        {
            // Assign data directly from LStr buffer
            cppString.assign(reinterpret_cast<const char*>(LStrBuf(lvStringPtr)),static_cast<size_t>(lvStringPtr->cnt));
        }
        catch (const std::bad_alloc&) { return mFullErr; } // Handle memory allocation failure
        catch (...) { return bogusError; } // Handle other exceptions

        return mgNoErr;
    }

    MgErr getLvStringHandle(LStrHandle lvHandle, char* buffer, size_t bufferSize)
    {
        // Check buffer validity
        if (!buffer || bufferSize == 0) { return mgArgErr; }

        // Ensure null termination initially
        std::memset(buffer, 0, bufferSize);

        if (!lvHandle) { return mgArgErr; } // Check handle pointer

        LStrPtr lvStringPtr = *lvHandle;

        // Check LStr pointer and length
        if (!lvStringPtr || lvStringPtr->cnt <= 0)
        {
            return mgNoErr; // Empty or invalid LStr, buffer is null-terminated
        }

        // Determine copy length, leaving space for null terminator
        size_t lenToCopy = static_cast<size_t>(lvStringPtr->cnt);
        size_t maxCopyLen = bufferSize - 1;

        bool truncated = false;
        if (lenToCopy > maxCopyLen)
        {
            lenToCopy = maxCopyLen;
            truncated = true;
        }

        if (lenToCopy > 0)
        {
            std::memcpy(buffer, LStrBuf(lvStringPtr), lenToCopy);
        }

        // Final null termination
        buffer[lenToCopy] = '\0';

        return mgNoErr;
    }
}