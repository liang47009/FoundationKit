/****************************************************************************
 Copyright (c) 2015 libo All rights reserved.
 
 losemymind.libo@gmail.com
 
 ****************************************************************************/
 #include "FoundationKit/GenericPlatformMacros.h"
 #if TARGET_PLATFORM == PLATFORM_IOS
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <net/if.h>
#include <net/if_dl.h>
#include <mach/vm_statistics.h>
#include <mach/machine.h>
#include <mach/mach_types.h> 
#include <mach/mach_init.h>
#include <mach/mach_host.h>
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <sys/time.h>
#include <sched.h>  // maybe the BSD time and sched.h need to be independent, but they are normally available together

#include <unordered_map>
#import <UIKit/UIKit.h>
#import <AdSupport/AdSupport.h>
#include "Platform.h"
#include "FoundationKit/Foundation/StringUtils.h"
#include "FoundationKit/Crypto/md5.hpp"
#include "FoundationKit/Platform/OpenGL.h"
#include "FoundationKit/Platform/FileUtils.h"
#include "FoundationKit/Platform/Environment.h"


NS_FK_BEGIN


float Platform::getTotalMemory()
{
    long megabyte = 1024*1024;
	int64_t physical_memory;
	int mib[2];
	mib[0] = CTL_HW;
	mib[1] = HW_MEMSIZE;
	size_t length = sizeof(int64_t);
	sysctl(mib, 2, &physical_memory, &length, NULL, 0);
	return static_cast<int>((physical_memory/megabyte));
}

float Platform::getFreeMemory()
{
    long megabyte = 1024*1024;
	vm_size_t page_size;
	mach_port_t mach_port;
	mach_msg_type_number_t count;
	vm_statistics_data_t vm_stats;
	int64_t free_memory = 0;
	float mb = 0;
	mach_port = mach_host_self();
	count = sizeof(vm_stats) / sizeof(natural_t);
	if (KERN_SUCCESS == host_page_size(mach_port, &page_size) &&
		KERN_SUCCESS == host_statistics(mach_port, HOST_VM_INFO, 
										(host_info_t)&vm_stats, &count))
	{
		free_memory = (int64_t)vm_stats.free_count * (int64_t)page_size;

		mb = static_cast<int>((free_memory/megabyte));
	}
	return mb;
}

float Platform::getProcessMemory()
{
    long megabyte = 1024*1024;
	struct task_basic_info t_info;
	mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;
	
	if (KERN_SUCCESS != task_info(mach_task_self(),
								  TASK_BASIC_INFO, (task_info_t)&t_info, 
								  &t_info_count))
	{
		return -1;
	}
	return static_cast<int>((t_info.resident_size/megabyte));
}

std::vector<uint8> Platform::getMacAddressRaw()
{
    std::vector<uint8>  result;
	result.resize(6);
    int                 mgmtInfoBase[6];
    char                *msgBuffer = NULL;
    NSString            *errorFlag = NULL;
    size_t              length;
    
    // Setup the management Information Base (mib)
    mgmtInfoBase[0] = CTL_NET;        // Request network subsystem
    mgmtInfoBase[1] = AF_ROUTE;       // Routing table info
    mgmtInfoBase[2] = 0;
    mgmtInfoBase[3] = AF_LINK;        // Request link layer information
    mgmtInfoBase[4] = NET_RT_IFLIST;  // Request all configured interfaces
    
    // With all configured interfaces requested, get handle index
    if ((mgmtInfoBase[5] = if_nametoindex("en0")) == 0)
        errorFlag = @"if_nametoindex failure";
    // Get the size of the data available (store in len)
    else if (sysctl(mgmtInfoBase, 6, NULL, &length, NULL, 0) < 0)
        errorFlag = @"sysctl mgmtInfoBase failure";
    // Alloc memory based on above call
    else if ((msgBuffer = static_cast<char*>(malloc(length))) == NULL)
        errorFlag = @"buffer allocation failure";
    // Get system information, store in buffer
    else if (sysctl(mgmtInfoBase, 6, msgBuffer, &length, NULL, 0) < 0)
    {
        free(msgBuffer);
        errorFlag = @"sysctl msgBuffer failure";
    }
    else
    {
        // Map msgbuffer to interface message structure
        struct if_msghdr *interfaceMsgStruct = (struct if_msghdr *) msgBuffer;
        
        // Map to link-level socket structure
        struct sockaddr_dl *socketStruct = (struct sockaddr_dl *) (interfaceMsgStruct + 1);
        
        // Copy link layer address data in socket structure to an array
        unsigned char macAddress[6];
        memcpy(&macAddress, socketStruct->sdl_data + socketStruct->sdl_nlen, 6);
        std::memcpy(result.data(), socketStruct->sdl_data + socketStruct->sdl_nlen, 6);
        // Release the buffer memory
        free(msgBuffer);
    }
    // Error...
    NSLog(@"Error: %@", errorFlag);
    return result;
}

std::string Platform::getMacAddress()
{
	std::vector<uint8> result = getMacAddressRaw();
    // Read from char array into a string object, into traditional Mac address format
    NSString *macAddressString = [NSString stringWithFormat:@"%02X%02X%02X%02X%02X%02X",
                                    result[0], result[1], result[2], result[3], result[4], result[5]];
    return [macAddressString UTF8String];
}

std::string Platform::getDeviceId()
{
    float systemVersion = [[[UIDevice currentDevice] systemVersion] floatValue];
    if(systemVersion < 7.0f)
    {
        return MD5::md5_hash_hex(getMacAddress().c_str());
    }
    else
    {
        NSString* advertisingId = [[[ASIdentifierManager sharedManager] advertisingIdentifier] UUIDString];
        return [advertisingId UTF8String];
    }
    return "";
}

std::string Platform::getDeviceName()
{
    return Environment::GetMachineName();
}

std::string Platform::getOperatingSystemVersion()
{
    return Environment::GetOSVersion();
}

std::string Platform::getCPUArchitecture()
{
    cpu_type_t cpuType;
    cpu_subtype_t cpuSubType;
    size_t size = sizeof(cpuType);
    sysctlbyname("hw.cputype", &cpuType, &size, nullptr, 0);
    
    size = sizeof(cpuSubType);
    sysctlbyname("hw.cputype", &cpuSubType, &size, nullptr, 0);
    std::string cpuTypeString;
    switch (cpuType)
    {
        case CPU_TYPE_ARM:
            cpuTypeString.append("arm");
            if(cpuSubType == CPU_SUBTYPE_ARM_V6) cpuTypeString.append("v6");
            else if(cpuSubType == CPU_SUBTYPE_ARM_V7) cpuTypeString.append("v7");
            else if(cpuSubType == CPU_SUBTYPE_ARM_V7F) cpuTypeString.append("v7f");
            else if(cpuSubType == CPU_SUBTYPE_ARM_V7S) cpuTypeString.append("v7s");
            else if(cpuSubType == CPU_SUBTYPE_ARM_V7K) cpuTypeString.append("v7k");
            else if(cpuSubType == CPU_SUBTYPE_ARM_V8) cpuTypeString.append("v8");
            else cpuTypeString.append(StringUtils::to_string(cpuSubType));
            break;
        case CPU_TYPE_ARM64:
            cpuTypeString.append("arm64");
            break;
        case CPU_TYPE_X86:
            cpuTypeString.append("x86");
            break;
        case CPU_TYPE_X86_64:
            cpuTypeString.append("x86_64");
            break;
        default:
            cpuTypeString.append(StringUtils::to_string(cpuType) + StringUtils::to_string(cpuSubType));
            break;
    }
    return cpuTypeString;
}

void Platform::systemTime(int32& year, int32& month, int32& dayOfWeek, int32& day, int32& hour, int32& min, int32& sec, int32& msec)
{
    // query for calendar time
    struct timeval tmVal;
    gettimeofday(&tmVal, NULL);

    // convert it to local time
    struct tm localTime;
    localtime_r(&tmVal.tv_sec, &localTime);

    // pull out data/time
    year = localTime.tm_year + 1900;
    month = localTime.tm_mon + 1;
    dayOfWeek = localTime.tm_wday;
    day = localTime.tm_mday;
    hour = localTime.tm_hour;
    min = localTime.tm_min;
    sec = localTime.tm_sec;
    msec = tmVal.tv_usec / 1000;
}

void Platform::utcTime(int32& year, int32& month, int32& dayOfWeek, int32& day, int32& hour, int32& min, int32& sec, int32& msec)
{
    // query for calendar time
    struct timeval tmVal;
    gettimeofday(&tmVal, NULL);

    // convert it to UTC
    struct tm localTime;
    gmtime_r(&tmVal.tv_sec, &localTime);

    // pull out data/time
    year = localTime.tm_year + 1900;
    month = localTime.tm_mon + 1;
    dayOfWeek = localTime.tm_wday;
    day = localTime.tm_mday;
    hour = localTime.tm_hour;
    min = localTime.tm_min;
    sec = localTime.tm_sec;
    msec = tmVal.tv_usec / 1000;
}

int64 Platform::getTickCount()
{
    return (int64)mach_absolute_time();
}

#define USE_OPENGL_CAPTURE 0
void Platform::captureScreen(const Rect& rect, const std::string& filename, const std::function<void(bool, const std::string&)>& afterCaptured)
{
    do
    {
#if(USE_OPENGL_CAPTURE)
        int posx = static_cast<int>(rect.origin.x);
        int posy = static_cast<int>(rect.origin.y);
        int width  = static_cast<int>(rect.size.width);
        int height = static_cast<int>(rect.size.height);
        int buff_width = width - posx;
        int buff_height = height - posy;
        int data_size = buff_width * buff_height * 4;
        
        std::shared_ptr<GLubyte> buffer(new GLubyte[data_size], [](GLubyte* p){ SAFE_DELETE_ARRAY(p); });
        BREAK_IF(!buffer);
        
        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glReadPixels(posx, posy, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer.get());
        
        std::shared_ptr<GLubyte> flippedBuffer(new GLubyte[data_size], [](GLubyte* p) { SAFE_DELETE_ARRAY(p); });
        BREAK_IF(!flippedBuffer);
        
        for (int row = 0; row < buff_height; ++row)
        {
            memcpy(flippedBuffer.get() + (buff_height - row - 1) * buff_width * 4, buffer.get() + row * buff_width * 4, buff_width * 4);
        }
        
        CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, flippedBuffer.get(), data_size, NULL);
        CGImageRef imageRef = CGImageCreate(buff_width
                                            , buff_height
                                            , 8
                                            , 32
                                            , 4*buff_width
                                            , CGColorSpaceCreateDeviceRGB()
                                            , kCGBitmapByteOrderDefault
                                            , provider
                                            , NULL
                                            , NO
                                            , kCGRenderingIntentDefault);
        UIImage* image = [UIImage imageWithCGImage:imageRef];
#else
        NSMutableArray *windows = [UIApplication sharedApplication].windows.mutableCopy;
        UIWindow *keyWindow = [UIApplication sharedApplication].keyWindow;
        if (![windows containsObject:keyWindow]) {
            [windows addObject:keyWindow];
        }
        UIGraphicsBeginImageContextWithOptions([[windows objectAtIndex:0] bounds].size, YES, 0);
        for (UIWindow *window in windows) {
            //avoid https://github.com/kif-framework/KIF/issues/679
            if (window.hidden) {
                continue;
            }
            
            if ([window respondsToSelector:@selector(drawViewHierarchyInRect:afterScreenUpdates:)]) {
                [window drawViewHierarchyInRect:window.bounds afterScreenUpdates:YES];
            } else {
                [window.layer renderInContext:UIGraphicsGetCurrentContext()];
            }
        }
        UIImage *image = UIGraphicsGetImageFromCurrentImageContext();
        UIGraphicsEndImageContext();
#endif
        
        NSString *path = [NSHomeDirectory() stringByAppendingPathComponent:@"Documents/GameAutomation/scripts/"];
        FileUtils::getInstance()->createDirectory([path UTF8String]);
        NSString *imageName = [NSString stringWithUTF8String:filename.c_str()];
        path = [path stringByAppendingPathComponent:imageName];
        //[UIImageJPEGRepresentation(image,1.0) writeToFile:path atomically:YES];
        NSData * imageData = UIImageJPEGRepresentation(image,1.0);
        if (imageData && [imageData writeToFile:path atomically:YES])
        {
            if (afterCaptured)
                afterCaptured(true, [path UTF8String]);
        }
        else
        {
            if (afterCaptured)
                afterCaptured(false, [path UTF8String]);
        }

    } while (false);
}

NS_FK_END

#endif //TARGET_PLATFORM == PLATFORM_IOS












