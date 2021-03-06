//
//  SFInfoVer2FinanTableDataController.m
//  HouseCar
//
//  Created by shieh fabo on 14-9-10.
//  Copyright (c) 2014年 ytang.com. All rights reserved.
//

#import "HCCampHomeTableDataController.h"

#define Section_headerView(section) [[_sections objectAtIndex:section] headerView]

@implementation HCCampHomeTableDataControllerSection

@end

@interface HCCampHomeTableDataController()

@end

@implementation HCCampHomeTableDataController

#pragma mark - Property

- (NSDateFormatter *)defaultDateFormatter
{
    if(!_defaultDateFormatter){
        _defaultDateFormatter = [[NSDateFormatter alloc] init];
        [_defaultDateFormatter setDateFormat:@"MM.dd"];
    }
    return _defaultDateFormatter;
}

- (HCTableFocusViewController *)focusController
{
    if(!_focusController){
        _focusController = [[HCTableFocusViewController alloc] init];
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
    if ([self.tableDatadelegate conformsToProtocol:@protocol(HCCampHomeTableDataDelegate)]) {
        [self.tableDatadelegate HCCampHomeTableDataClickedWithData:data];
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

#pragma mark - table view delegate

-(CGFloat) tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section{
    
    return Section_headerView(section) ? [Section_headerView(section) frame].size.height : 0;
}

-(UIView *) tableView:(UITableView *)tableView viewForHeaderInSection:(NSInteger)section{
    
    return Section_headerView(section) ? Section_headerView(section) : nil;
}

-(NSInteger) numberOfSectionsInTableView:(UITableView *)tableView{
    return [_sections count] ? [_sections count] : 1;
}

@end
