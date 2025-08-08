#include "secureValue.hpp"
#include "string.hpp"
#include <3ds.h>

extern std::string g_fslibErrorString;

bool fslib::getSecureValueForTitle(uint32_t uniqueID, uint64_t &secureValueOut)
{
    // This is more or less how the function returns if the secure exists.
    bool valueExists = false;
    Result fsError = FSUSER_GetSaveDataSecureValue(&valueExists, &secureValueOut, SECUREVALUE_SLOT_SD, uniqueID, 0);
    if (!valueExists || R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error retrieving secure value: 0x%08X.", fsError);
        return false;
    }
    return true;
}

bool fslib::setSecureValueForTitle(uint32_t uniqueID, uint64_t secureValue)
{
    Result fsError = FSUSER_SetSaveDataSecureValue(secureValue, SECUREVALUE_SLOT_SD, uniqueID, 0);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error setting secure value: 0x%08X.", fsError);
        return false;
    }

    return true;
}
