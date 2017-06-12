/****************************************************************************
 Copyright (c) 2015 libo All rights reserved.
 
 losemymind.libo@gmail.com
 
 ****************************************************************************/
 #include "FoundationKit/GenericPlatformMacros.hpp"
 #if TARGET_PLATFORM == PLATFORM_IOS
#include <unistd.h>
#include <malloc/malloc.h>
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

size_t Platform::MallocUsableSize(void* ptr)
{
    return malloc_size(ptr);
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


std::string Platform::ScreenShot()
{
NSArray *windows = [UIEventHelper windowsWithKeyWindow];
            if (windows.count == 0)
            {
    
                NSLog(@"***** Could not take screenshot.  No windows were available.");
                return false;
            }
            
            UIGraphicsBeginImageContextWithOptions([[windows objectAtIndex:0] bounds].size, YES, 0);
            for (UIWindow *window in windows)
            {
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
            
            
            auto commandData = StringUtils::split(params, ',');
            std::string filename = commandData[0];
            std::string strBehaviorID = commandData[1];
            NSString* nsBehaviorID = [NSString stringWithUTF8String:strBehaviorID.c_str()];
            if(filename.empty())
            {
                DateTime  dateNow = DateTime::now();
                filename =  dateNow.toString("%Y%m%d%H%M%S");
                filename += ".jpg";
            }
            
            NSString *path = [NSHomeDirectory() stringByAppendingPathComponent:@"Documents/GameAutomation/scripts/"];
            FileUtils::getInstance()->createDirectory([path UTF8String]);
            NSString *imageName = [NSString stringWithUTF8String:filename.c_str()];
            path = [path stringByAppendingPathComponent:imageName];
            //[UIImageJPEGRepresentation(image,1.0) writeToFile:path atomically:YES];
            NSData * imageData = UIImageJPEGRepresentation(image,1.0);
            [imageData writeToFile:path atomically:YES];
            NSLog(@"***** ScreenShot completed, path:%@", path);
            
            NSString* nsRid = [[GameAutomation sharedGameAutomation] getCommandRid];
            [msendImage PostSendImage:path picName:imageName BehaviorID:nsBehaviorID rid:nsRid];
            
            this->setState(ScriptCommandState::COMPLETED);
            return true;
}


NS_FK_END

#endif //TARGET_PLATFORM == PLATFORM_IOS












