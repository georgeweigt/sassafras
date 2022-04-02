#ifndef __SassafrasView__
#define __SassafrasView__

#import <Cocoa/Cocoa.h>

@interface SassafrasView : NSText
{
	NSPoint startingPoint;
	NSRect selectionRect;
}

@end

#endif	// __SassafrasView__
