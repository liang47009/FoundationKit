//
//  AppDelegate.m
//  FoundationKitTest_Mac
//
//  Created by libo on 2017/5/11.
//  Copyright © 2017年 libo. All rights reserved.
//

#import "AppDelegate.h"

@interface AppDelegate ()

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
    
    NSScreen* mainScreen = [NSScreen mainScreen];
    NSDictionary* description = [mainScreen deviceDescription];
    NSSize resoultion = [[description objectForKey:NSDeviceResolution] sizeValue];
    
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}


@end
