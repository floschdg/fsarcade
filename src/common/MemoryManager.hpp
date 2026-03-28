#pragma once

#include <string>
#include <vector>
#include <cstdint>


using String32Id = uint32_t;


// temporary helper function
std::u32string int32_to_u32string(int32_t value);


class MemoryManager {
public:
    static std::u32string& GetString32(String32Id id);

    static String32Id EmplaceString32_Frame(std::u32string&& str);
    static void Clear_Frame();


private:
    static std::vector<std::u32string> s_frame_string32s;
};

