#include <unistd.h>
#include <sched.h>
#include <sys/resource.h>
#include <iostream>
#include <string.h>

namespace Util
{
bool 
parseCpuIndex(char* str, uint32_t& index)
{
    size_t i = 0;
    size_t size = strlen(str);
    while(i < size && str[i] == '0') i++;
	for(; i < size; ++i)
	{
		if(!isdigit(str[i])) return false;
		index = index * 10 + (str[i] - '0');
	}
    return true;
}

void
setCpuAffinity(uint32_t index)
{
	// set cpu affinity
	const int NPROCESSORS = sysconf( _SC_NPROCESSORS_ONLN );
    cpu_set_t set;
    CPU_ZERO(&set);
	int processor_index = index % NPROCESSORS;
    CPU_SET(processor_index, &set);
    sched_setaffinity(getpid(), sizeof(set), &set);
}

void
print_cpu_time()
{
	constexpr double BASE = 1000000.0;
    double user, sys;
    struct rusage myusage, childusage;
    if(getrusage(RUSAGE_SELF, &myusage) < 0) return;
    if(getrusage(RUSAGE_CHILDREN, &childusage) < 0) return;

    user = (double) myusage.ru_utime.tv_sec + myusage.ru_utime.tv_usec/ BASE;
    user += (double) childusage.ru_utime.tv_sec + childusage.ru_utime.tv_usec / BASE;
    sys = (double) myusage.ru_stime.tv_sec + myusage.ru_stime.tv_usec/ BASE;
    sys += (double) childusage.ru_stime.tv_sec + childusage.ru_stime.tv_usec/ BASE;

    std::cout << "\nuser time = " << user << ", sys time = " << sys << std::endl;
}
}