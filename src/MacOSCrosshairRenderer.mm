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
    double circleSpacingIncrease;
    CrosshairRenderer::DirectionShape directionShape;
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
        circleSpacingIncrease = 5.0;
        directionShape = CrosshairRenderer::DirectionShape::Circle;
        
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
    circleSpacingIncrease = settings.circleSpacingIncrease;
    directionShape = settings.directionShape;
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
    
    // Draw closing lines at crosshair start to close the gap
    CGFloat baseThickness = lineWidth;
    CGContextSetLineWidth(context, baseThickness);
    CGContextSetRGBStrokeColor(context, 
                              crosshairColor.redF(), 
                              crosshairColor.greenF(), 
                              crosshairColor.blueF(), 
                              opacity);
    CGContextSetBlendMode(context, inverted ? kCGBlendModeDifference : kCGBlendModeNormal);
    
    // Vertical closing line
    CGFloat closingLineLength = offsetFromCursor;
    CGContextMoveToPoint(context, x, y - closingLineLength/2);
    CGContextAddLineToPoint(context, x, y + closingLineLength/2);
    CGContextStrokePath(context);
    
    // Horizontal closing line
    CGContextMoveToPoint(context, x - closingLineLength/2, y);
    CGContextAddLineToPoint(context, x + closingLineLength/2, y);
    CGContextStrokePath(context);
    
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
    
    // Draw thin center lines in each crosshair arm with main color
    CGContextSetLineWidth(context, 1);
    CGContextSetRGBStrokeColor(context, 
                              crosshairColor.redF(), 
                              crosshairColor.greenF(), 
                              crosshairColor.blueF(), 
                              opacity);
    CGContextSetBlendMode(context, kCGBlendModeNormal);
    
    // Left arm center line
    CGContextMoveToPoint(context, x - offsetFromCursor, y);
    CGContextAddLineToPoint(context, screenBounds.origin.x, y);
    CGContextStrokePath(context);
    
    // Right arm center line
    CGContextMoveToPoint(context, x + offsetFromCursor, y);
    CGContextAddLineToPoint(context, screenBounds.origin.x + screenBounds.size.width, y);
    CGContextStrokePath(context);
    
    // Top arm center line
    CGContextMoveToPoint(context, x, y - offsetFromCursor);
    CGContextAddLineToPoint(context, x, screenBounds.origin.y);
    CGContextStrokePath(context);
    
    // Bottom arm center line
    CGContextMoveToPoint(context, x, y + offsetFromCursor);
    CGContextAddLineToPoint(context, x, screenBounds.origin.y + screenBounds.size.height);
    CGContextStrokePath(context);
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
    
    // Draw direction shapes if enabled
    if (showArrows) {
        [self drawDirectionShapesInContext:context 
                                   fromX:startX fromY:startY 
                                   toX:endX toY:endY 
                                   distance:totalDistance
                                   baseThickness:baseThickness];
    }
}

- (void)drawDirectionShapesInContext:(CGContextRef)context 
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
    
    CGContextSetRGBFillColor(context, 
                              crosshairColor.redF(), 
                              crosshairColor.greenF(), 
                              crosshairColor.blueF(), 
                              opacity);
    
    // Get actual screen bounds for clipping
    NSScreen *mainScreen = [NSScreen mainScreen];
    CGRect screenBounds = [mainScreen frame];
    
    // Generate circle positions with fixed diameter-based spacing
    NSMutableArray *circlePositions = [NSMutableArray array];
    
    // Calculate initial circle diameter at center (smallest size)
    // Circle radius = currentThickness / 4, so diameter = currentThickness / 2
    CGFloat baseDiameter = baseThickness / 2.0;          // Base circle diameter
    CGFloat initialSpacing = baseDiameter * 2.0;         // Start with 2x circle diameter (half of 4x)
    CGFloat spacingMultiplier = 1.0 + (circleSpacingIncrease / 100.0);  // Convert percentage to multiplier
    
    CGFloat currentDistance = initialSpacing;            // First circle position
    CGFloat currentSpacing = initialSpacing;
    
    // Generate positions from center toward edge with fixed progressive spacing
    while (currentDistance < totalDistance * 1.2) {  // Go slightly beyond line end for edge cases
        [circlePositions addObject:@(currentDistance)];
        
        // Calculate circle diameter at this position for next spacing
        double progress = currentDistance / totalDistance;
        double thickMultiplier = 1.0 + (thicknessMultiplier - 1.0) * progress;
        CGFloat currentThickness = baseThickness * thickMultiplier;
        CGFloat circleDiameter = currentThickness / 2.0;  // Circle diameter = thickness / 2
        
        // Next spacing is 2x the current circle diameter, increased by setting percentage
        currentSpacing = (circleDiameter * 2.0) * spacingMultiplier;
        spacingMultiplier *= (1.0 + (circleSpacingIncrease / 100.0));  // Compound the percentage increase
        currentDistance += currentSpacing;
    }
    
    // Draw circles from center outward
    for (NSNumber *distanceNum in circlePositions) {
        CGFloat dist = [distanceNum floatValue];
        
        // Skip if beyond the crosshair line
        if (dist > totalDistance) continue;
        
        // Calculate position along the line (0.0 at center, 1.0 at edge)
        double progress = dist / totalDistance;
        CGFloat circleX = startX + (endX - startX) * progress;
        CGFloat circleY = startY + (endY - startY) * progress;
        
        // Calculate circle size: small at center, large at edge
        double thickMultiplier = 1.0 + (thicknessMultiplier - 1.0) * progress;
        int currentThickness = baseThickness * thickMultiplier;
        CGFloat circleRadius = currentThickness / 4.0;
        
        // Create shape bounds
        CGRect shapeBounds = CGRectMake(circleX - circleRadius, 
                                       circleY - circleRadius, 
                                       circleRadius * 2, 
                                       circleRadius * 2);
        
        // Check if shape intersects screen bounds
        if (CGRectIntersectsRect(shapeBounds, screenBounds)) {
            // Save graphics state for clipping
            CGContextSaveGState(context);
            
            // Clip to screen bounds for partial shapes
            CGContextClipToRect(context, screenBounds);
            
            // Draw the appropriate shape
            switch (directionShape) {
                case CrosshairRenderer::DirectionShape::Circle:
                    CGContextFillEllipseInRect(context, shapeBounds);
                    break;
                    
                case CrosshairRenderer::DirectionShape::Arrow:
                {
                    // Calculate arrow pointing toward center - scale clearly with thickness
                    CGFloat arrowSize = circleRadius * 2.0; // Make arrow much larger for visible scaling
                    CGFloat normalizedDeltaX = deltaX / length;
                    CGFloat normalizedDeltaY = deltaY / length;
                    
                    // Arrow tip points toward center (closer to center for better visibility)
                    CGFloat tipX = circleX + normalizedDeltaX * arrowSize * 0.3;
                    CGFloat tipY = circleY + normalizedDeltaY * arrowSize * 0.3;
                    
                    // Arrow base perpendicular to direction (wider base for better scaling visibility)
                    CGFloat perpX = -normalizedDeltaY;
                    CGFloat perpY = normalizedDeltaX;
                    
                    CGFloat baseX1 = circleX - normalizedDeltaX * arrowSize * 0.5 + perpX * arrowSize * 0.6;
                    CGFloat baseY1 = circleY - normalizedDeltaY * arrowSize * 0.5 + perpY * arrowSize * 0.6;
                    CGFloat baseX2 = circleX - normalizedDeltaX * arrowSize * 0.5 - perpX * arrowSize * 0.6;
                    CGFloat baseY2 = circleY - normalizedDeltaY * arrowSize * 0.5 - perpY * arrowSize * 0.6;
                    
                    CGContextBeginPath(context);
                    CGContextMoveToPoint(context, tipX, tipY);
                    CGContextAddLineToPoint(context, baseX1, baseY1);
                    CGContextAddLineToPoint(context, baseX2, baseY2);
                    CGContextClosePath(context);
                    CGContextFillPath(context);
                    break;
                }
                    
                case CrosshairRenderer::DirectionShape::Cross:
                {
                    CGFloat crossSize = circleRadius * 1.2; // Make cross larger and more visible
                    CGFloat crossLineWidth = circleRadius * 0.6; // Keep thickness proportional
                    
                    // Horizontal line
                    CGRect hLine = CGRectMake(circleX - crossSize, circleY - crossLineWidth/2, 
                                             crossSize * 2, crossLineWidth);
                    CGContextFillRect(context, hLine);
                    
                    // Vertical line  
                    CGRect vLine = CGRectMake(circleX - crossLineWidth/2, circleY - crossSize, 
                                             crossLineWidth, crossSize * 2);
                    CGContextFillRect(context, vLine);
                    break;
                }
                    
                case CrosshairRenderer::DirectionShape::Raute:
                {
                    CGFloat rauteSize = circleRadius * 1.3; // Make Raute larger and more visible
                    
                    // Create diamond (Raute) shape
                    CGContextBeginPath(context);
                    CGContextMoveToPoint(context, circleX, circleY - rauteSize);      // Top
                    CGContextAddLineToPoint(context, circleX + rauteSize, circleY);   // Right
                    CGContextAddLineToPoint(context, circleX, circleY + rauteSize);   // Bottom
                    CGContextAddLineToPoint(context, circleX - rauteSize, circleY);   // Left
                    CGContextClosePath(context);
                    CGContextFillPath(context);
                    break;
                }
            }
            
            // Restore graphics state
            CGContextRestoreGState(context);
        }
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