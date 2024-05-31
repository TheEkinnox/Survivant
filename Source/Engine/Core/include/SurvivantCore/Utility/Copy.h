#pragma once
#include <cstring>

namespace SvCore::Utility
{
#if __STDC_WANT_SECURE_LIB__

    inline bool MemCopy(void* p_destination, const size_t p_destinationSize, const void* p_source, const size_t p_sourceSize)
    {
        return memcpy_s(p_destination, p_destinationSize, p_source, p_sourceSize) == 0;
    }

    inline bool StrCopy(char* p_destination, const size_t p_destinationSize, const char* p_source)
    {
        return strcpy_s(p_destination, p_destinationSize, p_source) == 0;
    }

#else // __STDC_WANT_SECURE_LIB__

    inline bool MemCopy(void* p_destination, const size_t p_destinationSize, const void* p_source, const size_t p_sourceSize)
    {
        return p_destinationSize >= p_sourceSize && memcpy(p_destination, p_source, p_sourceSize) == p_destination;
    }

    inline bool StrCopy(char* p_destination, const size_t p_destinationSize, const char* p_source)
    {
        return p_destinationSize >= strlen(p_source) && strcpy(p_destination, p_source) == p_destination;
    }

#endif // __STDC_WANT_SECURE_LIB__
}
