//
//  HCMyPageViewController.m
//  HouseCar
//
//  Created by tangyin on 15/12/8.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import "HCMyPageViewController.h"

@interface HCMyPageViewController ()

@end

@implementation HCMyPageViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    
    [self.tableDataController.tableView setTableHeaderView:_tableViewHeaderView];
    
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
