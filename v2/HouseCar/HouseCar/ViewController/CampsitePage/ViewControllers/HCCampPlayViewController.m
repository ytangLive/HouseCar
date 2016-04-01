//
//  HCCampPlayViewController.m
//  HouseCar
//
//  Created by tangyin on 15/12/8.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import "HCCampPlayViewController.h"
#import "HCCommentViewController.h"

@interface HCCampPlayViewController ()
@property (weak ,nonatomic)IBOutlet UILabel *titleLabel;
@end

@implementation HCCampPlayViewController
{
    NSString *_campID;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    
    NSDictionary *queryValues = self.url.queryValues;
    _campID = [queryValues stringValueForKey:@"campID"];
    NSString *campTitle = [queryValues stringValueForKey:@"campTitle"];
    
    [_tableDataController setContext:self.context];
    self.dataSource.delegate = self.tableDataController;
    self.dataSource.campID = _campID;
    
    self.tableDataController.delegate = self;
    self.tableDataController.tableDatadelegate = self;
    self.tableDataController.tableView.rowHeight = 50;
    
    self.tableDataController.dataUrlKey = @"camps-detail";
    
    self.titleLabel.text = campTitle;
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    
    if(![_tableDataController.dataSource isLoading]){
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
    
    if([[sender actionName] isEqualToString:@"comment"]){
        pageInfo = @{@"commentType":[NSNumber numberWithInt:HCCommentTypeCamp],
                     @"postID":_campID,
                     @"userName":@"admin",
                     @"email":@"ytang_live@163.com"};
    }
    
    [self openUrl:[NSURL URLWithString:[sender userInfo]
                         relativeToURL:self.url
                           queryValues:pageInfo] animated:YES];
}

- (IBAction)viewTapAction:(id)sender
{
    NSDictionary *pageInfo = nil;
    if(self.dataSource.dataObject){
        NSString *title = [self.dataSource.dataObject stringValueForKey:@"title"];
        NSString *htmlContent = [self.dataSource.dataObject stringValueForKey:@"content" defaultValue:@""];
        pageInfo = @{@"title":title, @"htmlContent":htmlContent};
    }
    
    [self openUrl:[NSURL URLWithString:@"root:///root/tab/campPaly/webBrowser"
                         relativeToURL:self.url
                           queryValues:pageInfo] animated:YES];
}

#pragma mark - HCHomePageTableDataDelegate

- (void)HCCampPlayTableDataClickedWithData:(NSDictionary *)data
{
    
}

#pragma mark - VTDataControllerDelegate

- (void)vtDataControllerDidLoaded:(VTDataController *)controller
{
    
}

@end
