//
//  HCRegisterPageViewController.m
//  HouseCar
//
//  Created by tangyin on 16/1/11.
//  Copyright © 2016年 sina.com. All rights reserved.
//

#import "HCRegisterPageViewController.h"
#import "HCNavigationTitleBar.h"

@interface HCRegisterPageViewController ()

@property(nonatomic,weak) IBOutlet HCNavigationTitleBar *titleBar;
@property(nonatomic,weak) IBOutlet VTButton *actionButton;
@property(nonatomic, weak) IBOutlet UIView *protocolView;

@end

@implementation HCRegisterPageViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    
    
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
