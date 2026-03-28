#include "common/MemoryManager.hpp"

std::vector<std::u32string> MemoryManager::s_frame_string32s;


std::u32string
int32_to_u32string(int32_t value)
{
    std::string str = std::to_string(value);
    return std::u32string(str.begin(), str.end());
}


std::u32string&
MemoryManager::GetString32(String32Id id)
{
    return s_frame_string32s[id];
}

String32Id
MemoryManager::EmplaceString32_Frame(std::u32string&& str)
{
    String32Id id = static_cast<String32Id>(s_frame_string32s.size());
    s_frame_string32s.emplace_back(str);
    return id;
}

void
MemoryManager::Clear_Frame()
{
    s_frame_string32s.clear();
}

