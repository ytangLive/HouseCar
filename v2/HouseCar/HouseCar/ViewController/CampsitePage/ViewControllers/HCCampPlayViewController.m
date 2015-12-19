//
//  HCCampPlayViewController.m
//  HouseCar
//
//  Created by tangyin on 15/12/8.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import "HCCampPlayViewController.h"

@interface HCCampPlayViewController ()

@end

@implementation HCCampPlayViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    
    [_tableDataController setContext:self.context];
    self.dataSource.delegate = self.tableDataController;
    self.tableDataController.delegate = self;
    self.tableDataController.tableDatadelegate = self;
    self.tableDataController.tableView.rowHeight = 50;
    
    self.tableDataController.dataUrlKey = @"carlist-interface";
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

- (void)vtTableDataController:(VTTableDataController *)dataController cell:(VTTableViewCell *)cell doAction:(id<IVTAction>)action
{
    
    
}

- (IBAction)doAction:(id)sender
{
    NSDictionary *pageInfo = nil;
    
    [self openUrl:[NSURL URLWithString:[sender userInfo]
                         relativeToURL:self.url
                           queryValues:pageInfo] animated:YES];
}

#pragma mark - HCHomePageTableDataDelegate

- (void)HCCampPlayTableDataClickedWithData:(NSDictionary *)data
{
    
}

@end
