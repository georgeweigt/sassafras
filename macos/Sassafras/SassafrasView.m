// Functions that override NSView

#import "SassafrasView.h"

extern void draw_display(float, float);
extern void draw_selection_rect(float, float, float, float);

CGContextRef gcontext;

@implementation SassafrasView

-(void)drawRect:(NSRect)rect
{
}

@end
