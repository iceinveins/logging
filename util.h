#pragma once
namespace Util
{
bool parseCpuIndex(char* str, uint32_t& index);
void setCpuAffinity(uint32_t index);
void print_cpu_time();
}
