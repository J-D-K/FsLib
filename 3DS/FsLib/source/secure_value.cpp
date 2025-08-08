#include "fslib.hpp"
#include "string.hpp"

#include <3ds.h>

bool fslib::get_secure_value_for_title(uint32_t uniqueID, uint64_t &secureValue)
{
    bool exists{};
    const bool getError =
        error::libctru(FSUSER_GetSaveDataSecureValue(&exists, &secureValue, SECUREVALUE_SLOT_SD, uniqueID, 0));
    if (getError) { return false; }

    return true;
}

bool fslib::set_secure_value_for_title(uint32_t uniqueID, uint64_t secureValue)
{
    const bool setError = error::libctru(FSUSER_SetSaveDataSecureValue(secureValue, SECUREVALUE_SLOT_SD, uniqueID, 0));
    if (setError) { return false; }

    return true;
}
