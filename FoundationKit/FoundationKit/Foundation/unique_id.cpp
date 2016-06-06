
#include "unique_id.hpp"
NS_FK_BEGIN
static const  uint64_t sequence_max = (std::numeric_limits<uint64_t>::max)();
uint64_t unique_id::sequence = 0;
uint64_t unique_id::machineid = 0;
uint64 FoundationKit::unique_id::create()
{
    int64 uniqueid = 0;
    int64 timestamp = std::chrono::high_resolution_clock::now().time_since_epoch().count();

    if (machineid == 0)
    {
        std::vector<uint8> machineid_arr = Platform::getMacAddressRaw();
        std::stringstream ss;
        ss << std::hex << std::setfill('0');

        for (unsigned i = 0; i < 6; ++i)
            ss << std::setw(2) << int(machineid_arr[i]);

        ss >> machineid;

        if (machineid == 0)
            return INVALID_UNIQUE_ID;
    }

    uniqueid = timestamp << 22;
    uniqueid |= (machineid & 0x3FF) << 12;
    uniqueid |= sequence++ & 0xFFF;
    if (sequence == sequence_max)
    {
        sequence = 0;
    }
    return uniqueid;
}

NS_FK_END


