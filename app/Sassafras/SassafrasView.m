// Functions that override NSView

#import "SassafrasView.h"

extern void draw_display(float, float);
extern void draw_selection_rect(float, float, float, float);

CGContextRef gcontext;

@implementation SassafrasView

-(void)drawRect:(NSRect)rect
{
#if 0
	float y1, y2;

	gcontext = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];

    CGContextSetTextMatrix(gcontext, CGAffineTransformIdentity);

    y1 = rect.origin.y;
	y2 = rect.origin.y + rect.size.height;

	draw_display(y1, y2);

	draw_selection_rect(selectionRect.origin.x, selectionRect.origin.y, selectionRect.size.width, selectionRect.size.height);
#endif
}

-(void)mouseDown:(NSEvent *)event
{
	startingPoint = [self convertPoint:[event locationInWindow] fromView:nil]; // from Cocoa Drawing Guide p. 54
}

-(void)mouseUp:(NSEvent *)event
{
	NSRect r = selectionRect;

	// clear selection rectangle so it is not drawn

	selectionRect.size.width = 0;
	selectionRect.size.height = 0;

	// copy contents of rectangle to pasteboard

	NSPasteboard *p = [NSPasteboard generalPasteboard];
	[p declareTypes:[NSArray arrayWithObjects:NSPDFPboardType, nil] owner:self];
	[self writePDFInsideRect:r toPasteboard:p];

	// clear selection rectangle on screen

	[self display];
}

-(void)mouseDragged:(NSEvent *)event
{
	NSPoint p = [self convertPoint:[event locationInWindow] fromView:nil]; // from Cocoa Drawing Guide p. 54

	// update the selection rectangle

	selectionRect.origin.x = (startingPoint.x < p.x) ? startingPoint.x : p.x;
	selectionRect.origin.y = (startingPoint.y < p.y) ? startingPoint.y : p.y;
	
	selectionRect.size.width = fabs(startingPoint.x - p.x);
	selectionRect.size.height = fabs(startingPoint.y - p.y);

	[self display];
}

@end
