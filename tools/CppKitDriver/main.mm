#include <Cocoa/Cocoa.h>

int main(int argc, const char** argv) {
  [NSAutoreleasePool new];
  [NSApplication sharedApplication];
  [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

  id window = [[[NSWindow alloc]
      initWithContentRect: NSMakeRect(0, 0, 800, 600)
      styleMask: NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable 
      backing: NSBackingStoreBuffered
      defer: NO
    ]
    autorelease
  ];

  [window cascadeTopLeftFromPoint:NSMakePoint(20, 20)];
  [window setTitle: @"Hello, World!"];
  [window makeKeyAndOrderFront:nil];

  [NSApp activateIgnoringOtherApps:YES];
  [NSApp run];

  return 0;
}