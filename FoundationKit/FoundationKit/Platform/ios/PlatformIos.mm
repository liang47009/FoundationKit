#ifdef FK_TARGET_OS_IPHONE
#include "Platform.h"
#include <sys/types.h>
#include <sys/sysctl.h>
#include <mach/vm_statistics.h>
#include <mach/mach_types.h> 
#include <mach/mach_init.h>
#include <mach/mach_host.h>
#include <mach/mach.h>
NS_FK_BEGIN
int Platform::getTotalMemory()
{
	int64_t physical_memory;
	int mib[2];
	mib[0] = CTL_HW;
	mib[1] = HW_MEMSIZE;
	size_t length = sizeof(int64_t);
	sysctl(mib, 2, &physical_memory, &length, NULL, 0);
	return static_cast<int>((physical_memory/1048576)*1024);
	//GB = physical_memory/1073741824;
}

int Platform::getUsedMemory()
{
	vm_size_t page_size;
	mach_port_t mach_port;
	mach_msg_type_number_t count;
	vm_statistics_data_t vm_stats;
	int64_t used_memory = 0;
	//int64_t myFreeMemory = 0;
	int mb = 0;
	mach_port = mach_host_self();
	count = sizeof(vm_stats) / sizeof(natural_t);
	if (KERN_SUCCESS == host_page_size(mach_port, &page_size) &&
		KERN_SUCCESS == host_statistics(mach_port, HOST_VM_INFO, 
										(host_info_t)&vm_stats, &count))
	{
		//myFreeMemory = (int64_t)vm_stats.free_count * (int64_t)page_size;
		
		used_memory = ((int64_t)vm_stats.active_count + 
					   (int64_t)vm_stats.inactive_count + 
					   (int64_t)vm_stats.wire_count) *  (int64_t)page_size;
		mb = static_cast<int>((used_memory/1048576)*1024);
	}
	return mb;
}

int Platform::getProcessMemory()
{
	struct task_basic_info t_info;
	mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;
	
	if (KERN_SUCCESS != task_info(mach_task_self(),
								  TASK_BASIC_INFO, (task_info_t)&t_info, 
								  &t_info_count))
	{
		return -1;
	}
	return static_cast<int>((t_info.resident_size/1048576)*1024);
}
NS_FK_END
#endif





