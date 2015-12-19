//
//  SFInfoVer2FinanTableDataController.m
//  HouseCar
//
//  Created by shieh fabo on 14-9-10.
//  Copyright (c) 2014年 ytang.com. All rights reserved.
//

#import "HCCampHomeTableDataController.h"

@interface HCCampHomeTableDataController()
@property(nonatomic,strong)NSMutableArray *suspends;//需要悬停的cell集合,目前支持1个
@property(nonatomic,strong)UIView *suspendView;;//需要悬停的View
@end

@implementation HCCampHomeTableDataController

-(void)setContext:(id<IVTUIContext>)context
{
    [super setContext:context];
    
    DeviceScreenType deviceType = [(id<HCContext>)self.context deviceType];
    if (deviceType == AppleIphone6plus) {
        self.tableView.rowHeight = 81;
    }else if(deviceType == AppleIphone6){
        self.tableView.rowHeight = 74;
    }else{
        self.tableView.rowHeight = 65;
    }
}

#pragma mark - Property

- (NSDateFormatter *)defaultDateFormatter
{
    if(!_defaultDateFormatter){
        _defaultDateFormatter = [[NSDateFormatter alloc] init];
        [_defaultDateFormatter setDateFormat:@"MM.dd"];
    }
    return _defaultDateFormatter;
}

- (NSMutableArray *)suspends
{
    if(!_suspends){
        _suspends = [[NSMutableArray alloc]  init];
    }
    return _suspends;
}

- (HCTableFocusViewController *)focusController
{
    if(!_focusController){
        _focusController = [[HCTableFocusViewController alloc] init];
    }
    return _focusController;
}

- (UIView *)suspendView
{
    if(!_suspendView){
        //悬浮tableCell
        _suspendView = [[UIView alloc]initWithFrame:CGRectMake(0, 0, KScreenWidth, 46)];
        _suspendView.hidden = YES;
        [[self.tableView superview] addSubview:_suspendView];
    }
    return _suspendView;
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

-(UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    self.itemViewNib = @"HCCampPlayItemCell";

    return [super tableView:tableView cellForRowAtIndexPath:indexPath];
}

@end
