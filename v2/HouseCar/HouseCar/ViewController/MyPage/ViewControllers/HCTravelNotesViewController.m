//
//  HCTravelNotesViewController.m
//  HouseCar
//
//  Created by tangyin on 16/1/29.
//  Copyright © 2016年 sina.com. All rights reserved.
//

#import "HCTravelNotesViewController.h"

@interface HCTravelNotesViewController ()

@end

@implementation HCTravelNotesViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    
    [_tableDataController setContext:self.context];
    self.dataSource.delegate = self.tableDataController;
    self.tableDataController.delegate = self;
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    
    if(![_tableDataController.dataSource isLoading] && ![_tableDataController.dataSource isLoaded]){
        [_tableDataController reloadData];
    }else{
        [[_tableDataController tableView] reloadData];
    }
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
