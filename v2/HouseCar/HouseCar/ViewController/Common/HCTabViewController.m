//
//  SFTabViewController.m
//  VTTeamProject
//
//  Created by zhang hailong on 13-7-5.
//  Copyright (c) 2013年 ytang.com. All rights reserved.
//

#import "HCTabViewController.h"

@interface HCTabViewController ()

@end

@implementation HCTabViewController

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
    // Do any additional setup after loading the view from its nib.
    
    self.isBubblehidden = YES;
    
    [self.dataOutletContainer applyDataOutlet:self];
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(didReceiveNewOpinionNotification:) name:HCNewOpinionNotification object:nil];
    
}

-(void) didReceiveNewOpinionNotification:(NSNotification *) notification{
    
    NSDictionary * userInfo  =[notification userInfo];
    
    self.isBubblehidden = ![[userInfo valueForKey:HCNewOpinionBubbleShowKey] boolValue];
    [self.dataOutletContainer applyDataOutlet:self];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)viewDidUnload {
    [self setLightView:nil];
    [super viewDidUnload];
}

- (void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self name:HCNewOpinionNotification object:nil];
}

-(IBAction) doTabAction:(id)sender{
    
    [UIView beginAnimations:nil context:nil];
    [UIView setAnimationDuration:0.2];
    
    [_lightView setFrame:[sender frame]];
    
    [UIView commitAnimations];
    
    [super doTabAction:sender];
}

-(void) viewDidDisappear:(BOOL)animated{
    [super viewDidDisappear:animated];
}


-(void) setSelectedIndex:(NSUInteger)selectedIndex{

    [super setSelectedIndex:selectedIndex];
}


@end
