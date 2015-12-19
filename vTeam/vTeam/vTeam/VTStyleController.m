//
//  VTStyleSheetController.m
//  vTeam
//
//  Created by zhang hailong on 13-5-31.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#import "VTStyleController.h"

@interface VTStyleController ()

@end

@implementation VTStyleController

@synthesize styles = _styles;

-(void) dealloc{
    [_styles release];
    [super dealloc];
}


- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view.
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

-(id)addObjectFromStyleController:(VTStyleController*)controller
{
    BOOL existController = NO;
    @try {
        if (controller) {
            [controller view];
            if (controller.styles&&[controller.styles count]) {
                existController = YES;
            }
        }
    }
    @catch (NSException *exception) {
        
    }
    BOOL existSelf = NO;
    @try {
        [self view];
        if (self.styles&&[self.styles count]) {
            existSelf = YES;
        }
    }
    @catch (NSException *exception) {
        
    }
    id ret = nil;
    if (existSelf||existController) {
        if (!existSelf) {
            ret = controller;
        }
        else
        {
            ret = self;
        }
    }
    if (existSelf&&existController) {
        NSMutableArray* oldStyles = (NSMutableArray*)self.styles;
        if (![oldStyles isKindOfClass:[NSMutableArray class]]) {
            oldStyles = [[[NSMutableArray alloc] initWithArray:oldStyles] autorelease];
            self.styles = oldStyles;
        }
        if (controller.styles) {
            NSArray* newStyles = controller.styles;
            NSInteger newCount = [newStyles count];
            for (int i=0; i<newCount; i++) {
                VTStyle* oneNewStyle = [newStyles objectAtIndex:i];
                
                NSInteger hasContainsInteger = -1;
                NSInteger oldCount = [oldStyles count];
                for (NSInteger j=oldCount-1; j>=0; j--) {
                    VTStyle* oneOldStyle = [oldStyles objectAtIndex:j];
                    if (oneOldStyle.name&&oneOldStyle.key&&oneNewStyle.name&&oneNewStyle.key) {
                        if ([oneOldStyle.name isEqualToString:oneNewStyle.name]&&[oneOldStyle.key isEqualToString:oneNewStyle.key]) {
                            hasContainsInteger = j;
                            break;
                        }
                    }
                }
                if (hasContainsInteger>=0) {
                    [oldStyles replaceObjectAtIndex:hasContainsInteger withObject:oneNewStyle];
                }
                else
                {
                    [oldStyles addObject:oneNewStyle];
                }
            }
        }
    }
    return ret;
}

@end
