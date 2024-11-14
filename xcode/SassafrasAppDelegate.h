//
//  SassafrasAppDelegate.h
//  Sassafras
//
//  Created by George Weigt on 2/28/13.
//  Copyright (c) 2013 George Weigt. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface SassafrasAppDelegate : NSObject <NSApplicationDelegate>

@property (assign) IBOutlet NSWindow *window;
-(IBAction)runButton:(id)sender;
@property (unsafe_unretained) IBOutlet NSTextView *userProgram;
@property (unsafe_unretained) IBOutlet NSTextView *outputView;
@property (weak) IBOutlet NSProgressIndicator *progressIndicator;

@end
