/****************************************************************************
 Copyright (c) 2015 libo All rights reserved.
 
 losemymind.libo@gmail.com
 
 ****************************************************************************/
 #include "FoundationKit/GenericPlatformMacros.hpp"
 #if TARGET_PLATFORM == PLATFORM_IOS
#include <unistd.h>
#include <sys/sysctl.h>
#include "FoundationKit/Platform/Platform.hpp"

NS_FK_BEGIN
bool Platform::IsDebuggerPresent()
{
    // Based on http://developer.apple.com/library/mac/#qa/qa1361/_index.html
	struct kinfo_proc Info;
	int32 Mib[] = { CTL_KERN, KERN_PROC, KERN_PROC_PID, getpid() };
	size_t Size = sizeof(Info);
	sysctl( Mib, sizeof( Mib ) / sizeof( *Mib ), &Info, &Size, NULL, 0 );
	return ( Info.kp_proc.p_flag & P_TRACED ) != 0;
}
NS_FK_END

#endif //TARGET_PLATFORM == PLATFORM_IOS












