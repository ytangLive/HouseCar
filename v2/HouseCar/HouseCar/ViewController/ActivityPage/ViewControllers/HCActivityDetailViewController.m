//
//  HCActivityDetailViewController.m
//  HouseCar
//
//  Created by tangyin on 15/12/24.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import "HCActivityDetailViewController.h"

@interface HCActivityDetailViewController ()
@property (weak ,nonatomic) IBOutlet HCTableHeaderView *tableHeaderView;
@end

@implementation HCActivityDetailViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    
    [_tableDataController setContext:self.context];
    self.dataSource.delegate = self.tableDataController;
    self.tableDataController.delegate = self;
    self.tableDataController.tableView.backgroundColor = DefaultBackgroundColor;
    
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

#pragma mark - VTTableDataControllerDelegate

- (void)vtTableDataController:(VTTableDataController *)dataController cell:(VTTableViewCell *)cell doAction:(id<IVTAction>)action
{
    NSString * actionName =[action actionName];
    NSDictionary *pageInfo = nil;
    if([actionName isEqualToString:@"activityGuidelines"]){
        pageInfo = @{@"activityGuidelines":[VTJSON encodeObject:_dataSource.costActivityInfos]};
    }
    
    [self openUrl:[NSURL URLWithString:[action userInfo]
                         relativeToURL:self.url
                           queryValues:pageInfo] animated:YES ];
    
}

- (void)doAction:(id)sender
{
    NSString *actionName = [sender actionName];
    NSDictionary *pageInfo = nil;
    if([actionName isEqualToString:@"registration"]){
        NSString *title = [_dataSource.headerViewDic stringValueForKey:@"title"];
        NSString *imgUrl = [_dataSource.headerViewDic stringValueForKey:@"imgUrl"];
        NSString *totalUsersNum =  [[_dataSource.attendUsers firstObject] stringValueForKey:@"totalUser"];
        NSString *attendUsersNum =  [[_dataSource.attendUsers firstObject] stringValueForKey:@"attendUser"];
        NSString *activityStartDate = [[_dataSource.attendInfos firstObject] stringValueForKey:@"activityStartDate"];
        NSString *activityCost = [[_dataSource.attendInfos firstObject] stringValueForKey:@"activityCost"];
        NSMutableDictionary *dataObject = [[NSMutableDictionary alloc] init];
        [dataObject setValue:title forKey:@"title"];
        [dataObject setValue:imgUrl forKey:@"imgUrl"];
        [dataObject setValue:[NSString stringWithFormat:@"/%@",totalUsersNum] forKey:@"totalUsersNum"];
        [dataObject setValue:attendUsersNum forKey:@"attendUsersNum"];
        [dataObject setValue:activityStartDate forKey:@"activityStartDate"];
        [dataObject setValue:[NSString stringWithFormat:@"￥%@",activityCost] forKey:@"activityCost"];
        
        NSArray *dataObjects = @[dataObject];
        
        pageInfo = @{@"activityDetails":[VTJSON encodeObject:dataObjects]};
    }
    
    [self openUrl:[NSURL URLWithString:[sender userInfo]
                         relativeToURL:self.url
                           queryValues:pageInfo] animated:YES];
}

@end
