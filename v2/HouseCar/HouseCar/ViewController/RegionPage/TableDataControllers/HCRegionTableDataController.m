//
//  HCRegionTableDataController.m
//  HouseCar
//
//  Created by tangyin on 15/12/21.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import "HCRegionTableDataController.h"

@implementation HCRegionTableDataController

#pragma mark - Property

- (HCTableFocusViewController *)focusController
{
    if(!_focusController){
        _focusController = [[HCTableFocusViewController alloc] init];
        _focusController.focusCount = 1;
    }
    return _focusController;
}

#pragma mark - reloadData

-(void)reloadData
{
    
    [super reloadData];
}

-(void)refreshData
{
    
    [super refreshData];
    
}

#pragma mark =========delegate========
-(void)HCTableFocusCellClickedWithData:(NSDictionary*)data
{
    if ([self.tableDatadelegate conformsToProtocol:@protocol(HCRegionTableDataDelegate)]) {
        [self.tableDatadelegate HCRegionTableDataClickedWithData:data];
    }
}

-(void)HCTableFocusCellDownloaded:(NSArray *)data error:(NSError *)error
{
    if (!error) {
        NSDictionary *dic = nil;
        if (data) {
            dic = [NSDictionary dictionaryWithObject:data forKey:@"posts"];
        }
        [self.dataSource vtDownlinkTaskDidLoaded:dic forTaskType:nil];
    }else{
        [self.dataSource vtDownlinkTaskDidFitalError:error forTaskType:nil];
    }
}

#pragma mark - vtdatasource delegate

-(void)vtDataSourceWillLoading:(VTDataSource *)dataSource
{
    [super vtDataSourceWillLoading:dataSource];
    
    self.focusController.context = self.context;
    self.focusController.zxKey = @"zxlist";
    self.focusController.celltitleKey = @"title";
    self.focusController.cellsrcKey = @"p_url";
    self.focusController.delegate = self;
    
    //初始化focusController视图
    self.focusController.view.frame = self.focusController.view.frame;
    
    self.focusController.dataSource.urlKey = self.dataUrlKey;
    [self.focusController reloadData];
}

-(void) vtDataSourceDidLoadedFromCache:(VTDataSource *) dataSource timestamp:(NSDate *) timestamp
{
    [self reloadTableHeaderViewData];
    [super vtDataSourceDidLoadedFromCache:dataSource timestamp:timestamp];
}

-(void) vtDataSourceDidLoaded:(VTDataSource *) dataSource
{
    [self reloadTableHeaderViewData];
    [super vtDataSourceDidLoaded:dataSource];
}

-(void)reloadTableHeaderViewData
{
    if ([self.focusController.dataSource.dataObjects count]>0) {
        if (!self.tableView.tableHeaderView) {
            [self.tableView setTableHeaderView:self.focusController.view];
        }
    }
    else
    {
        self.tableView.tableHeaderView = nil;
    }
}

-(void) vtDataSource:(VTDataSource *) dataSource didFitalError:(NSError *) error
{
    [super vtDataSource:dataSource didFitalError:error];
}

@end
