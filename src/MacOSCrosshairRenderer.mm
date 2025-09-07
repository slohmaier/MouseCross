#include "MacOSCrosshairRenderer.h"
#include <QApplication>
#include <QScreen>
#include <QCursor>
#include <cmath>
#import <Cocoa/Cocoa.h>
#import <QuartzCore/QuartzCore.h>

@interface CrosshairView : NSView
{
    QPoint mousePos;
    QColor crosshairColor;
    int lineWidth;
    int offsetFromCursor;
    double thicknessMultiplier;
    double opacity;
    bool showArrows;
    bool inverted;
}

- (void)updateMousePosition:(QPoint)pos;
- (void)updateSettings:(const CrosshairRenderer::Settings&)settings;
@end

@implementation CrosshairView

- (instancetype)initWithFrame:(NSRect)frameRect
{
    self = [super initWithFrame:frameRect];
    if (self) {
        crosshairColor = Qt::white;
        lineWidth = 4;
        offsetFromCursor = 49;
        thicknessMultiplier = 3.0;
        opacity = 0.8;
        showArrows = true;
        inverted = false;
        
        [self setWantsLayer:YES];
        self.layer.backgroundColor = [[NSColor clearColor] CGColor];
    }
    return self;
}

- (void)updateMousePosition:(QPoint)pos
{
    mousePos = pos;
    [self setNeedsDisplay:YES];
}

- (void)updateSettings:(const CrosshairRenderer::Settings&)settings
{
    crosshairColor = settings.color;
    lineWidth = settings.lineWidth;
    offsetFromCursor = settings.offsetFromCursor;
    thicknessMultiplier = settings.thicknessMultiplier;
    opacity = settings.opacity;
    showArrows = settings.showArrows;
    inverted = settings.inverted;
    [self setNeedsDisplay:YES];
}

- (void)drawRect:(NSRect)dirtyRect
{
    CGContextRef context = [[NSGraphicsContext currentContext] CGContext];
    if (!context) return;
    
    CGContextClearRect(context, dirtyRect);
    
    // Get screen bounds
    NSRect screenBounds = [[NSScreen mainScreen] frame];
    for (NSScreen *screen in [NSScreen screens]) {
        screenBounds = NSUnionRect(screenBounds, [screen frame]);
    }
    
    // Convert Qt coordinates to macOS coordinates
    CGFloat screenHeight = screenBounds.size.height;
    CGFloat x = mousePos.x();
    CGFloat y = screenHeight - mousePos.y(); // Flip Y coordinate
    
    // Set up drawing parameters
    CGContextSetLineCap(context, kCGLineCapButt);
    CGContextSetShouldAntialias(context, YES);
    
    // Draw crosshair lines
    [self drawCrosshairInContext:context atX:x y:y screenBounds:screenBounds];
}

- (void)drawCrosshairInContext:(CGContextRef)context atX:(CGFloat)x y:(CGFloat)y screenBounds:(NSRect)screenBounds
{
    // Calculate distances to edges
    CGFloat distToLeft = x - screenBounds.origin.x;
    CGFloat distToRight = screenBounds.origin.x + screenBounds.size.width - x;
    CGFloat distToTop = y - screenBounds.origin.y;
    CGFloat distToBottom = screenBounds.origin.y + screenBounds.size.height - y;
    
    // Draw gradient lines
    [self drawGradientLineInContext:context 
                            fromX:x - offsetFromCursor fromY:y 
                            toX:screenBounds.origin.x toY:y 
                            distance:distToLeft];
    
    [self drawGradientLineInContext:context 
                            fromX:x + offsetFromCursor fromY:y 
                            toX:screenBounds.origin.x + screenBounds.size.width toY:y 
                            distance:distToRight];
    
    [self drawGradientLineInContext:context 
                            fromX:x fromY:y - offsetFromCursor 
                            toX:x toY:screenBounds.origin.y 
                            distance:distToTop];
    
    [self drawGradientLineInContext:context 
                            fromX:x fromY:y + offsetFromCursor 
                            toX:x toY:screenBounds.origin.y + screenBounds.size.height 
                            distance:distToBottom];
}

- (void)drawGradientLineInContext:(CGContextRef)context 
                         fromX:(CGFloat)startX fromY:(CGFloat)startY 
                         toX:(CGFloat)endX toY:(CGFloat)endY 
                         distance:(CGFloat)totalDistance
{
    const int segments = 50;
    
    // Get screen scale factor
    CGFloat scaleFactor = [[NSScreen mainScreen] backingScaleFactor];
    int baseThickness = MAX(3, lineWidth * scaleFactor);
    
    for (int i = 0; i < segments; ++i) {
        double progress = (double)i / segments;
        double nextProgress = (double)(i + 1) / segments;
        
        // Calculate thickness based on distance from center
        double thickMultiplier = 1.0 + (thicknessMultiplier - 1.0) * progress;
        CGFloat currentThickness = baseThickness * thickMultiplier;
        
        // Calculate segment points
        CGFloat segStartX = startX + (endX - startX) * progress;
        CGFloat segStartY = startY + (endY - startY) * progress;
        CGFloat segEndX = startX + (endX - startX) * nextProgress;
        CGFloat segEndY = startY + (endY - startY) * nextProgress;
        
        // Draw outer line with selected color
        CGContextSetLineWidth(context, currentThickness);
        CGContextSetRGBStrokeColor(context, 
                                  crosshairColor.redF(), 
                                  crosshairColor.greenF(), 
                                  crosshairColor.blueF(), 
                                  opacity);
        
        if (inverted) {
            CGContextSetBlendMode(context, kCGBlendModeDifference);
        } else {
            CGContextSetBlendMode(context, kCGBlendModeNormal);
        }
        
        CGContextMoveToPoint(context, segStartX, segStartY);
        CGContextAddLineToPoint(context, segEndX, segEndY);
        CGContextStrokePath(context);
        
        // Draw inner contrasting line if not inverted
        if (!inverted) {
            CGContextSetLineWidth(context, currentThickness / 2);
            CGContextSetRGBStrokeColor(context, 
                                      1.0 - crosshairColor.redF(), 
                                      1.0 - crosshairColor.greenF(), 
                                      1.0 - crosshairColor.blueF(), 
                                      opacity * 0.7);
            CGContextMoveToPoint(context, segStartX, segStartY);
            CGContextAddLineToPoint(context, segEndX, segEndY);
            CGContextStrokePath(context);
        }
    }
    
    // Draw arrows if enabled
    if (showArrows) {
        [self drawArrowsInContext:context 
                         fromX:startX fromY:startY 
                         toX:endX toY:endY 
                         distance:totalDistance
                         baseThickness:baseThickness];
    }
}

- (void)drawArrowsInContext:(CGContextRef)context 
                   fromX:(CGFloat)startX fromY:(CGFloat)startY 
                   toX:(CGFloat)endX toY:(CGFloat)endY 
                   distance:(CGFloat)totalDistance
                   baseThickness:(int)baseThickness
{
    // Calculate direction vector
    CGFloat deltaX = startX - endX;
    CGFloat deltaY = startY - endY;
    CGFloat length = sqrt(deltaX * deltaX + deltaY * deltaY);
    
    if (length == 0) return;
    
    CGFloat dirX = deltaX / length;
    CGFloat dirY = deltaY / length;
    CGFloat perpX = -dirY;
    CGFloat perpY = dirX;
    
    int arrowSpacing = baseThickness * 4;
    CGFloat arrowPenWidth = MAX(1, baseThickness / 2);
    
    CGContextSetLineWidth(context, arrowPenWidth);
    CGContextSetRGBStrokeColor(context, 
                              crosshairColor.redF(), 
                              crosshairColor.greenF(), 
                              crosshairColor.blueF(), 
                              opacity);
    
    for (int distance = arrowSpacing; distance < totalDistance - arrowSpacing; distance += arrowSpacing) {
        double progress = (double)distance / totalDistance;
        CGFloat arrowX = endX + (startX - endX) * progress;
        CGFloat arrowY = endY + (startY - endY) * progress;
        
        double thickMultiplier = 1.0 + (thicknessMultiplier - 1.0) * progress;
        int currentThickness = baseThickness * thickMultiplier;
        // Arrow size matches the inner line thickness (half of current thickness)
        int currentArrowSize = currentThickness / 4;
        
        // Draw arrow head
        CGFloat tipX = arrowX;
        CGFloat tipY = arrowY;
        CGFloat leftX = arrowX - dirX * currentArrowSize + perpX * currentArrowSize * 0.5;
        CGFloat leftY = arrowY - dirY * currentArrowSize + perpY * currentArrowSize * 0.5;
        CGFloat rightX = arrowX - dirX * currentArrowSize - perpX * currentArrowSize * 0.5;
        CGFloat rightY = arrowY - dirY * currentArrowSize - perpY * currentArrowSize * 0.5;
        
        CGContextMoveToPoint(context, tipX, tipY);
        CGContextAddLineToPoint(context, leftX, leftY);
        CGContextMoveToPoint(context, tipX, tipY);
        CGContextAddLineToPoint(context, rightX, rightY);
        CGContextStrokePath(context);
    }
}

@end

MacOSCrosshairRenderer::MacOSCrosshairRenderer()
    : m_view(nil)
    , m_updateTimer(nullptr)
    , m_window(nil)
{
}

MacOSCrosshairRenderer::~MacOSCrosshairRenderer()
{
    cleanup();
}

bool MacOSCrosshairRenderer::initialize()
{
    @autoreleasepool {
        // Get combined screen geometry
        NSRect combinedFrame = NSZeroRect;
        for (NSScreen *screen in [NSScreen screens]) {
            combinedFrame = NSUnionRect(combinedFrame, [screen frame]);
        }
        
        // Create a borderless, transparent overlay window
        NSWindow *window = [[NSWindow alloc] initWithContentRect:combinedFrame
                                                       styleMask:NSWindowStyleMaskBorderless
                                                         backing:NSBackingStoreBuffered
                                                           defer:NO];
        
        [window setBackgroundColor:[NSColor clearColor]];
        [window setOpaque:NO];
        [window setHasShadow:NO];
        [window setIgnoresMouseEvents:YES]; // Critical: pass through all mouse events
        [window setLevel:kCGOverlayWindowLevel]; // Above normal windows
        [window setCollectionBehavior:NSWindowCollectionBehaviorCanJoinAllSpaces | 
                                     NSWindowCollectionBehaviorStationary |
                                     NSWindowCollectionBehaviorIgnoresCycle];
        
        // Create the crosshair view
        m_view = [[CrosshairView alloc] initWithFrame:combinedFrame];
        [window setContentView:m_view];
        
        // Store window reference
        m_window = (__bridge void*)window;
        [window retain];
        
        // Setup update timer
        m_updateTimer = new QTimer(this);
        connect(m_updateTimer, &QTimer::timeout, this, &MacOSCrosshairRenderer::updateCrosshair);
        
        return true;
    }
}

void MacOSCrosshairRenderer::cleanup()
{
    stopRendering();
    
    if (m_updateTimer) {
        delete m_updateTimer;
        m_updateTimer = nullptr;
    }
    
    @autoreleasepool {
        if (m_window) {
            NSWindow *window = (__bridge NSWindow*)m_window;
            [window close];
            [window release];
            m_window = nil;
        }
        
        if (m_view) {
            [m_view release];
            m_view = nil;
        }
    }
}

void MacOSCrosshairRenderer::startRendering()
{
    if (m_isRendering) return;
    
    @autoreleasepool {
        NSWindow *window = (__bridge NSWindow*)m_window;
        [window orderFront:nil];
        [window makeKeyAndOrderFront:nil];
    }
    
    if (m_updateTimer) {
        m_updateTimer->start(16); // ~60 FPS
    }
    
    m_isRendering = true;
}

void MacOSCrosshairRenderer::stopRendering()
{
    if (!m_isRendering) return;
    
    if (m_updateTimer) {
        m_updateTimer->stop();
    }
    
    @autoreleasepool {
        NSWindow *window = (__bridge NSWindow*)m_window;
        [window orderOut:nil];
    }
    
    m_isRendering = false;
}

void MacOSCrosshairRenderer::updateSettings(const Settings& settings)
{
    m_settings = settings;
    
    @autoreleasepool {
        if (m_view) {
            [m_view updateSettings:settings];
        }
        
        if (m_window) {
            NSWindow *window = (__bridge NSWindow*)m_window;
            [window setAlphaValue:1.0]; // Let the view handle opacity
        }
    }
}

void MacOSCrosshairRenderer::updateMousePosition(const QPoint& pos)
{
    m_mousePos = pos;
    
    @autoreleasepool {
        if (m_view) {
            [m_view updateMousePosition:pos];
        }
    }
}

void MacOSCrosshairRenderer::updateCrosshair()
{
    QPoint newPos = QCursor::pos();
    if (newPos != m_mousePos) {
        updateMousePosition(newPos);
    }
}