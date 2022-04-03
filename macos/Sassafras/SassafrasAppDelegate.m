//
//  SassafrasAppDelegate.m
//  Sassafras
//
//  Created by George Weigt on 2/28/13.
//  Copyright (c) 2013 George Weigt. All rights reserved.
//

// NSTextView : NSText : NSView : NSResponder : NSObject

#import "SassafrasAppDelegate.h"

char *input_c_string;
extern char output_c_string[];
char *prefix;
int running;

NSString *filename;

@implementation SassafrasAppDelegate

-(BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender
{
    return YES;
}

-(void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    // Insert code here to initialize your application

    [_userProgram setFont:[NSFont fontWithName:[NSString stringWithUTF8String:"Courier"] size:14.0]];
    [_outputView setFont:[NSFont fontWithName:[NSString stringWithUTF8String:"Courier"] size:14.0]];

    // Get home directory prefix

    NSString *s = [@"~" stringByExpandingTildeInPath];
    prefix = strdup([s UTF8String]);
}

extern void run(char *);
extern void clear_display(void);

extern int total_h;

// this function is linked to file menu automatically

-(IBAction)openDocument:(id)sender {
    NSOpenPanel* panel = [NSOpenPanel openPanel];
    [panel beginWithCompletionHandler:^(NSInteger result){
        if (result == NSFileHandlingPanelOKButton) {
            NSURL *url = [[panel URLs] objectAtIndex:0];
            NSString *s = [NSString stringWithContentsOfURL:url encoding:NSUTF8StringEncoding error:nil];
            [self->_userProgram setString:s];
            [self->_userProgram setNeedsDisplay:YES];
            filename = [[NSString alloc] initWithString:[url absoluteString]];
            [[self->_userProgram window] setTitle:filename];
        }
        
    }];
}

// this function is linked to file menu automatically

-(IBAction)saveDocument:(id)sender
{
    if (filename == nil)
        [self saveDocumentAs:sender];
    else {
        NSString *s = [_userProgram string];
        [s writeToURL:[NSURL URLWithString:filename] atomically:NO encoding:NSUTF8StringEncoding error:nil];
    }
}

// this function is linked to file menu automatically

-(IBAction)saveDocumentAs:(id)sender
{
    NSSavePanel *panel = [NSSavePanel savePanel];
    [panel setCanSelectHiddenExtension:YES];
    [panel setTreatsFilePackagesAsDirectories:YES];
    [panel beginSheetModalForWindow:[_userProgram window] completionHandler:^(NSInteger result) {
        if (result == NSFileHandlingPanelOKButton) {
            NSURL *url = [panel URL];
            NSString *s = [self->_userProgram string];
            [s writeToURL:url atomically:NO encoding:NSUTF8StringEncoding error:nil];
            filename = [[NSString alloc] initWithString:[url absoluteString]];
            [[self->_userProgram window] setTitle:filename];
        }
    }];
}

-(IBAction)runButton:(id)sender
{
    NSString *t;

    if (running)
        return;
    
    if (input_c_string)
        free((void *) input_c_string);
    
    const char *s = [[_userProgram string] UTF8String];
    
    input_c_string = strdup(s);
    
    clear_display();
    t = [[NSString alloc] initWithCString:output_c_string encoding:NSASCIIStringEncoding];
    [_outputView setString:t];
    [_outputView setNeedsDisplay:YES];

    // run as thread

    [_progressIndicator startAnimation:nil];
    running = 1;
    [NSThread detachNewThreadSelector:@selector(workerThread:) toTarget:self withObject:nil];
}

-(void)workerThread:(id)inObject
{
    run(input_c_string);
    [self performSelectorOnMainThread:@selector(myCustomDrawing:) withObject:nil waitUntilDone:YES];
    running = 0;
}

-(void)myCustomDrawing:(id)myCustomData
{
    NSString *s;
    [_progressIndicator stopAnimation:nil];
    s = [[NSString alloc] initWithCString:output_c_string encoding:NSASCIIStringEncoding];
    [_outputView setString:s];
    [_outputView setNeedsDisplay:YES];
}

@end
