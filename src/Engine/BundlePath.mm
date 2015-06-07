#import <Cocoa/Cocoa.h>
#import <objc/runtime.h>
#include <string>

std::string
getBundlePath()
{
    CFURLRef appUrlRef = CFBundleCopyBundleURL( CFBundleGetMainBundle() );
    CFStringRef macPath = CFURLCopyFileSystemPath( appUrlRef, kCFURLPOSIXPathStyle );
    char buffer[1024];
    if (!CFStringGetCString(macPath, buffer, 1024, kCFStringEncodingUTF8))
         return std::string();
    CFRelease(appUrlRef);
    CFRelease(macPath);
    return buffer;
}
