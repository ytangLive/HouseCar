//
//  HCServicePageViewController.m
//  HouseCar
//
//  Created by tangyin on 15/12/8.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import "HCRegionPageViewController.h"

@interface HCRegionPageViewController ()

@end

@implementation HCRegionPageViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    
    [_tableDataController setContext:self.context];
    self.dataSource.delegate = self.tableDataController;
    self.tableDataController.delegate = self;
    self.tableDataController.tableDatadelegate = self;
    self.tableDataController.tableView.backgroundColor = TableViewBackgroundColor;
    
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

- (void)doAction:(id)sender
{
    [super doAction:sender];
    
    //NSString * actionName =[sender actionName];
    
}

#pragma mark - VTTableDataControllerDelegate

- (void)vtTableDataController:(VTTableDataController *)dataController cell:(VTTableViewCell *)cell doAction:(id<IVTAction>)action
{
    NSString * actionName =[action actionName];
    if([actionName isEqualToString:@"cellSelected"]){
        NSDictionary *pageInfo = nil;
        
        [self openUrl:[NSURL URLWithString:[action userInfo]
                             relativeToURL:self.url
                               queryValues:pageInfo] animated:YES];
        
    }
    
}


#pragma mark - HCCarBaseTableDataDelegate

- (void)HCRegionTableDataClickedWithData:(NSDictionary *)data
{
    
}

@end
