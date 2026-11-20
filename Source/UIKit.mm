// © 2021 NVIDIA Corporation

// iOS: the OS owns the process entry point and the frame loop, so the sample runs from a display link.
// The window is a UIView backed by a CAMetalLayer, which is handed to "SampleBase" before "Create".

#include "NRIFramework.h"

#if (NRIF_PLATFORM == NRIF_UIKIT)

#import <QuartzCore/QuartzCore.h>
#import <UIKit/UIKit.h>

static SampleBase* (*g_CreateSample)() = nullptr;
static const char* g_SampleName = "";

//==============================================================================================================================
// View
//==============================================================================================================================

@interface NRIFMetalView : UIView
@property(nonatomic) SampleBase* sample;
@end

@implementation NRIFMetalView
+ (Class)layerClass {
    return [CAMetalLayer class];
}

- (void)sendTouches:(NSSet<UITouch*>*)touches pressed:(BOOL)pressed {
    if (!self.sample)
        return;

    UITouch* touch = [touches anyObject];
    CGPoint point = [touch locationInView:self];
    CGFloat scale = self.traitCollection.displayScale;

    self.sample->SetTouch((float)(point.x * scale), (float)(point.y * scale), pressed);
}

- (void)touchesBegan:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event {
    [self sendTouches:touches pressed:YES];
}

- (void)touchesMoved:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event {
    [self sendTouches:touches pressed:YES];
}

- (void)touchesEnded:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event {
    [self sendTouches:touches pressed:NO];
}

- (void)touchesCancelled:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event {
    [self sendTouches:touches pressed:NO];
}
@end

//==============================================================================================================================
// View controller: creates the sample once the view has a size, then renders one frame per display link tick
//==============================================================================================================================

@interface NRIFViewController : UIViewController {
    SampleBase* _sample;
    CADisplayLink* _displayLink;
}
@end

@implementation NRIFViewController

- (void)loadView {
    self.view = [[NRIFMetalView alloc] init];
}

- (BOOL)prefersStatusBarHidden {
    return YES;
}

- (BOOL)prefersHomeIndicatorAutoHidden {
    return YES;
}

- (UIInterfaceOrientationMask)supportedInterfaceOrientations {
    return UIInterfaceOrientationMaskPortrait; // the swap chain doesn't handle resizing yet
}

- (void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];

    if (_sample)
        return;

    CAMetalLayer* layer = (CAMetalLayer*)self.view.layer;
    CGFloat scale = self.traitCollection.displayScale;
    CGSize size = self.view.bounds.size;
    uint2 outputResolution((uint32_t)(size.width * scale), (uint32_t)(size.height * scale));

    layer.contentsScale = scale;
    layer.drawableSize = CGSizeMake(outputResolution.x, outputResolution.y);

    _sample = g_CreateSample();
    _sample->SetNativeWindow((__bridge void*)layer, outputResolution, (float)scale);
    ((NRIFMetalView*)self.view).sample = _sample;

    // No command line on iOS: defaults only
    char* argv[] = {(char*)g_SampleName, nullptr};
    if (!_sample->Create(1, argv, g_SampleName))
        exit(NRIF_FAIL);

    _displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(tick:)];
    [_displayLink addToRunLoop:[NSRunLoop mainRunLoop] forMode:NSRunLoopCommonModes];
}

- (void)tick:(CADisplayLink*)link {
    if (_sample->RenderLoopStep())
        return;

    [link invalidate];

    _sample->EndRenderLoop();
    ((NRIFMetalView*)self.view).sample = nullptr;
    delete _sample;
    _sample = nullptr;

    exit(NRIF_OK);
}

@end

//==============================================================================================================================
// Application
//==============================================================================================================================

@interface NRIFSceneDelegate : UIResponder <UIWindowSceneDelegate>
@property(strong, nonatomic) UIWindow* window;
@end

@implementation NRIFSceneDelegate
- (void)scene:(UIScene*)scene willConnectToSession:(UISceneSession*)session options:(UISceneConnectionOptions*)connectionOptions {
    self.window = [[UIWindow alloc] initWithWindowScene:(UIWindowScene*)scene];
    self.window.rootViewController = [[NRIFViewController alloc] init];
    [self.window makeKeyAndVisible];
}
@end

@interface NRIFAppDelegate : UIResponder <UIApplicationDelegate>
@end

@implementation NRIFAppDelegate
@end

int NRIF_UIKitMain(int argc, char** argv, SampleBase* (*createSample)(), const char* sampleName) {
    g_CreateSample = createSample;
    g_SampleName = sampleName;

    // Shaders and textures are looked up relative to the working directory ("_Shaders", "_Data"): use the app bundle
    [[NSFileManager defaultManager] changeCurrentDirectoryPath:[[NSBundle mainBundle] bundlePath]];

    @autoreleasepool {
        return UIApplicationMain(argc, argv, nil, NSStringFromClass([NRIFAppDelegate class]));
    }
}

#endif
