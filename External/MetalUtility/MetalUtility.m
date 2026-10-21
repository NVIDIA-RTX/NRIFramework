#include "MetalUtility.h"

#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>

// Vulkan and Metal swap-chain creation requires the metal layer. GLFW isn't used here: a shared library would have its own
// static GLFW copy, which isn't initialized
void* GetMetalLayer(void* window)
{
    NSWindow* nsWindow = (__bridge NSWindow*)window;
    if (!nsWindow)
        return NULL;

    NSView* contentView = [nsWindow contentView];
    if (![contentView.layer isKindOfClass:[CAMetalLayer class]])
    {
        CAMetalLayer* layer = [CAMetalLayer layer];
        layer.contentsScale = nsWindow.backingScaleFactor;
        layer.frame = contentView.bounds;
        layer.autoresizingMask = kCALayerWidthSizable | kCALayerHeightSizable;
        [contentView setLayer:layer];
        [contentView setWantsLayer:YES];
    }

    return (__bridge void*)[contentView layer];
}
