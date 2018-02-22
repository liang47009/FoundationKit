//
//  main.m
//  FoundationKitTest_Mac
//
//  Created by libo on 2017/5/11.
//  Copyright © 2017年 libo. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#include "FoundationKit/forward.hpp"
#include "FoundationKit/FoundationMacros.hpp"
#include "FoundationKit/GenericPlatformMacros.hpp"
#include "FoundationKit/LanguageFeatures.hpp"

#include "FoundationKit/Crypto/aes.hpp"
#include "FoundationKit/Crypto/Base64.hpp"
#include "FoundationKit/Crypto/des.hpp"
#include "FoundationKit/Crypto/md5.hpp"
#include "FoundationKit/Crypto/sha1.hpp"
#include "FoundationKit/Crypto/urlencode.hpp"

#include "FoundationKit/Foundation/Compression.hpp"
#include "FoundationKit/Foundation/DateTime.hpp"
#include "FoundationKit/Foundation/NotificationCenter.hpp"
#include "FoundationKit/Foundation/Dictionary.hpp"
#include "FoundationKit/Foundation/Directory.hpp"
#include "FoundationKit/Foundation/ElapsedTimer.hpp"
#include "FoundationKit/Foundation/Exception.hpp"
#include "FoundationKit/Foundation/File.hpp"
#include "FoundationKit/Foundation/FunctionHandler.hpp"
#include "FoundationKit/Foundation/Path.hpp"
#include "FoundationKit/Foundation/Singleton.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "FoundationKit/Foundation/Time.hpp"
#include "FoundationKit/Foundation/TimeSpan.hpp"
#include "FoundationKit/Foundation/Value.hpp"
#include "FoundationKit/Foundation/Version.hpp"

#include "FoundationKit/Math/Math.hpp"
#include "FoundationKit/Math/MathContent.hpp"
#include "FoundationKit/Math/MathUtil.hpp"
#include "FoundationKit/Math/Matrix.hpp"
#include "FoundationKit/Math/Quaternion.hpp"
#include "FoundationKit/Math/Rect.hpp"
#include "FoundationKit/Math/Size.hpp"
#include "FoundationKit/Math/Vector2.hpp"
#include "FoundationKit/Math/Vector3.hpp"
#include "FoundationKit/Math/Vector4.hpp"


#include "FoundationKit/Platform/Directory.hpp"
#include "FoundationKit/Platform/Environment.hpp"
#include "FoundationKit/Platform/File.hpp"
#include "FoundationKit/Platform/OpenGL.hpp"
#include "FoundationKit/Platform/Path.hpp"
#include "FoundationKit/Platform/PlatformDevice.hpp"
#include "FoundationKit/Platform/PlatformMemory.hpp"
#include "FoundationKit/Platform/PlatformTLS.hpp"

int main(int argc, const char * argv[]) {
    return NSApplicationMain(argc, argv);
}
