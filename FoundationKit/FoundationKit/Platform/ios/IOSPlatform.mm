/****************************************************************************
 Copyright (c) 2015 libo All rights reserved.
 
 losemymind.libo@gmail.com
 
 ****************************************************************************/
 #include "FoundationKit/GenericPlatformMacros.hpp"
 #if (PLATFORM_IOS)

#include <unistd.h>
#include <malloc/malloc.h>
#include <sys/sysctl.h>
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#include "FoundationKit/Platform/Platform.hpp"
#include "FoundationKit/Foundation/DateTime.hpp"

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

std::string Platform::ExecuteSystemCommand(const std::string& command)
{
    char buffer[256+1]={0};
    std::string result = "";
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (!feof(pipe))
        {
            if (fgets(buffer, 256, pipe) != NULL)
                result += buffer;
        }
    }
    catch (...)
    {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
}


bool Platform::ScreenShot(std::string& outSavePath)
{
    NSMutableArray *windows = [UIApplication sharedApplication].windows.mutableCopy;
    UIWindow *keyWindow = [UIApplication sharedApplication].keyWindow;
    if (![windows containsObject:keyWindow])
    {
        [windows addObject:keyWindow];
    }
    if (windows.count == 0)
    {
        
        NSLog(@"***** Could not take screenshot.  No windows were available.");
        [windows release];
        return false;
    }
    
    UIGraphicsBeginImageContextWithOptions([[windows objectAtIndex:0] bounds].size, YES, 0);
    for (UIWindow *window in windows)
    {
        if (window.hidden)
        {
            continue;
        }
        
        if ([window respondsToSelector:@selector(drawViewHierarchyInRect:afterScreenUpdates:)])
        {
            [window drawViewHierarchyInRect:window.bounds afterScreenUpdates:YES];
        }
        else
        {
            [window.layer renderInContext:UIGraphicsGetCurrentContext()];
        }
    }
    UIImage *image = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    [windows release];
    std::string filename = "ScreenShot_";
    DateTime  dateNow = DateTime::Now();
    filename +=  dateNow.ToString("%Y%m%d%H%M%S");
    filename += ".jpg";
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString *savePath = [paths objectAtIndex : 0];
    NSString *imageName = [NSString stringWithUTF8String:filename.c_str()];
    savePath = [savePath stringByAppendingPathComponent:imageName];
    NSData * imageData = UIImageJPEGRepresentation(image,1.0);
    [imageData writeToFile:savePath atomically:YES];
    outSavePath = [savePath UTF8String];
    NSLog(@"***** ScreenShot completed, path:%@", savePath);
    return true;
}


NS_FK_END

#endif // OF #if (PLATFORM_IOS)












