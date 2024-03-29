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
int running;
NSString *filename;
extern char *output_buffer;
extern void run(char *);

@implementation SassafrasAppDelegate

-(BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender
{
    return YES;
}

-(void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    // Insert code here to initialize your application
    [_userProgram setFont:[NSFont fontWithName:[NSString stringWithUTF8String:"Courier"] size:12.0]];
    [_outputView setFont:[NSFont fontWithName:[NSString stringWithUTF8String:"Courier"] size:12.0]];
    [_userProgram setAutomaticSpellingCorrectionEnabled:NO];
    [_userProgram setAutomaticQuoteSubstitutionEnabled:NO];
    _userProgram.enabledTextCheckingTypes = 0; // need this so -- (dash dash) isn't changed
    // Get home directory prefix
    // NSString *s = [@"~" stringByExpandingTildeInPath];
    // prefix = strdup([s UTF8String]);
}

// this function is linked to file menu automatically

-(IBAction)openDocument:(id)sender {
    NSOpenPanel* panel = [NSOpenPanel openPanel];
    [panel beginWithCompletionHandler:^(NSInteger result){
//        if (result == NSFileHandlingPanelOKButton) { [DEPRECATED]
        if (result == NSModalResponseOK) {
            NSURL *url = [[panel URLs] objectAtIndex:0];
            NSString *s = [NSString stringWithContentsOfURL:url encoding:NSUTF8StringEncoding error:nil];
            [self->_userProgram setString:s];
            [self->_userProgram setNeedsDisplay:YES];
            filename = [[NSString alloc] initWithString:[url absoluteString]];
            // [[self->_userProgram window] setTitle:filename];
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
//        if (result == NSFileHandlingPanelOKButton) { [DEPRECATED]
        if (result == NSModalResponseOK) {
            NSURL *url = [panel URL];
            NSString *s = [self->_userProgram string];
            [s writeToURL:url atomically:NO encoding:NSUTF8StringEncoding error:nil];
            filename = [[NSString alloc] initWithString:[url absoluteString]];
            // [[self->_userProgram window] setTitle:filename];
        }
    }];
}

-(IBAction)runButton:(id)sender
{
    if (running)
        return;

    if (input_c_string)
        free((void *) input_c_string);

    const char *s = [[_userProgram string] UTF8String];

    input_c_string = strdup(s);

    // clear
 
    [_outputView setString:@""];
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
    s = [[NSString alloc] initWithCString:output_buffer encoding:NSASCIIStringEncoding];
    [_outputView setString:s];
    [_outputView setNeedsDisplay:YES];
}

extern char *alfalfa_str;

-(IBAction)alfalfa_demo:(id)sender
{
    if (running)
        return;
    NSString *s = [[NSString alloc] initWithCString:alfalfa_str encoding:NSASCIIStringEncoding];
    [self->_userProgram setString:s];
    [self->_userProgram setNeedsDisplay:YES];
}

extern char *dryweight_str;

-(IBAction)dryweight_demo:(id)sender
{
    if (running)
        return;
    NSString *s = [[NSString alloc] initWithCString:dryweight_str encoding:NSASCIIStringEncoding];
    [self->_userProgram setString:s];
    [self->_userProgram setNeedsDisplay:YES];
}

extern char *fishtank_str;

-(IBAction)fishtank_demo:(id)sender
{
    if (running)
        return;
    NSString *s = [[NSString alloc] initWithCString:fishtank_str encoding:NSASCIIStringEncoding];
    [self->_userProgram setString:s];
    [self->_userProgram setNeedsDisplay:YES];
}

extern char *peanut_str;

-(IBAction)peanut_demo:(id)sender
{
    if (running)
        return;
    NSString *s = [[NSString alloc] initWithCString:peanut_str encoding:NSASCIIStringEncoding];
    [self->_userProgram setString:s];
    [self->_userProgram setNeedsDisplay:YES];
}

extern char *redclover_str;

-(IBAction)redclover_demo:(id)sender
{
    if (running)
        return;
    NSString *s = [[NSString alloc] initWithCString:redclover_str encoding:NSASCIIStringEncoding];
    [self->_userProgram setString:s];
    [self->_userProgram setNeedsDisplay:YES];
}

extern char *strawberry_str;

-(IBAction)strawberry_demo:(id)sender
{
    if (running)
        return;
    NSString *s = [[NSString alloc] initWithCString:strawberry_str encoding:NSASCIIStringEncoding];
    [self->_userProgram setString:s];
    [self->_userProgram setNeedsDisplay:YES];
}

extern char *trees_str;

-(IBAction)trees_demo:(id)sender
{
    if (running)
        return;
    NSString *s = [[NSString alloc] initWithCString:trees_str encoding:NSASCIIStringEncoding];
    [self->_userProgram setString:s];
    [self->_userProgram setNeedsDisplay:YES];
}

@end
