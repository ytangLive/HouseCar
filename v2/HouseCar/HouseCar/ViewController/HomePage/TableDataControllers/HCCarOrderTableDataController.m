//
//  HCCarOrderTableDataController.m
//  HouseCar
//
//  Created by tangyin on 15/12/1.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import "HCCarOrderTableDataController.h"
#import "HCCarOrderItemCell.h"

@interface HCCarOrderTableDataController() <HCCarOrderItemCellDelegate>

@end

@implementation HCCarOrderTableDataController

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

-(void)reloadData
{
    self.focusController.context = self.context;
    self.focusController.celltitleKey = @"title";
    self.focusController.cellsrcKey = @"p_url";
    self.focusController.delegate = self;
    
    //初始化focusController视图
    self.focusController.view.frame = self.focusController.view.frame;
    
    self.focusController.dataSource.urlKey = self.dataUrlKey;
    [self.focusController reloadData];
}

-(void)refreshData
{
    [self.focusController refreshData];
}

- (HCTableFocusViewController *)focusController
{
    if(!_focusController){
        _focusController = [[HCTableFocusViewController alloc] init];
    }
    return _focusController;
}

#pragma mark =========delegate========
-(void)HCTableFocusCellClickedWithData:(NSDictionary*)data
{
    if ([self.tableDatadelegate conformsToProtocol:@protocol(HCCarOrderTableDataDelegate)]) {
        [self.tableDatadelegate HCCarOrderTableDataClickedWithData:data];
    }
}

-(void)HCTableFocusCellDownloaded:(NSArray *)data error:(NSError *)error
{
    if (!error) {
         [self reloadTableHeaderViewData];
    }
}

#pragma mark - vtdatasource delegate

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

-(NSInteger) tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return 1;
}

- (CGFloat) tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    return 589;
}

-(UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{

    self.itemViewNib = @"HCCarOrderItemCell";

    NSString * identifier = self.itemViewNib;
    
    NSBundle * bundle = [self itemViewBundle];
    
    if(bundle == nil){
        bundle = [self.context resourceBundle];
    }
    
    UITableViewCell * cell = [tableView dequeueReusableCellWithIdentifier:identifier];
    
    if(cell == nil){
        
        cell = (UITableViewCell *) [HCCarOrderItemCell tableViewCellWithNibName:self.itemViewNib bundle:bundle];
        
        [cell setValue:identifier forKey:@"reuseIdentifier"];
        
        if([cell isKindOfClass:[HCCarOrderItemCell class]]){
            [(HCCarOrderItemCell *) cell setController:self];
            [(HCCarOrderItemCell *) cell setDelegate:self];
            [(HCCarOrderItemCell *) cell setOrderCellDeletate:self];
        }
    }
    
    if([cell isKindOfClass:[HCCarOrderItemCell class]]){
        [(HCCarOrderItemCell *) cell setContext:self.context];
    }
    
    
    NSDate *beginDate = [(id<HCContext>)self.context focusValueForKey:@"CarUserBeginDate"];
    NSDate *endDate = [(id<HCContext>)self.context focusValueForKey:@"CarUserEndDate"];
    
    beginDate = beginDate ? beginDate : [NSDate date];
    endDate = endDate ? endDate : [NSDate date];
    
    NSInteger days = [NSDate dayBetweenStartDate:beginDate endDate:endDate];
    ((HCCarOrderItemCell *) cell).daysLabel.text = [NSString stringWithFormat:@"%ld天",(long)days];
    NSString *beginDateStr = [NSDate stringFromDate:beginDate format:@"MM月dd日"];
    NSString *endDateStr = [NSDate stringFromDate:endDate format:@"MM月dd日"];
    if(days > 0){
        ((HCCarOrderItemCell *) cell).orderDateLabel.text = [NSString stringWithFormat:@"%@ - %@",beginDateStr,endDateStr];
    }else{
        ((HCCarOrderItemCell *) cell).orderDateLabel.text = beginDateStr;
    }
    
    if(_detailInfo){
        NSArray *locations = [_detailInfo arrayValueForKeyPath:@"custom_fields.location"];
        NSArray *pNums = [_detailInfo arrayValueForKeyPath:@"custom_fields.capacity"];
        NSArray *prices = [_detailInfo arrayValueForKeyPath:@"custom_fields.price"];
        NSString *location = [locations count] > 0 ? locations[0] : @"";
        NSString *pNum = [pNums count] > 0 ? pNums[0] : @"";
        NSString *price = [prices count] > 0 ? prices[0] : @"";
        ((HCCarOrderItemCell *) cell).titleLabel.text = [_detailInfo stringValueForKey:@"title"];
        ((HCCarOrderItemCell *) cell).locationLabel.text = [NSString stringWithFormat:@"适合人数：%@",location];
        ((HCCarOrderItemCell *) cell).pNumLabel.text = [NSString stringWithFormat:@"适合人数：%@",pNum];
        ((HCCarOrderItemCell *) cell).priceLabel.text = [NSString stringWithFormat:@"%@/ 天",price];
    }
    
    return cell;
}

#pragma mark - HCCarOrderItemCellDelegate


- (void)orderDaysViewTap:(id)sender
{
    if(_tableDatadelegate && [_tableDatadelegate respondsToSelector:@selector(orderDaysViewTap:)]){
        [_tableDatadelegate orderDaysViewTap:sender];
    }
}

- (void)carTakeWayViewTap:(id)sender
{
    if(_tableDatadelegate && [_tableDatadelegate respondsToSelector:@selector(carTakeWayViewTap:)]){
        [_tableDatadelegate carTakeWayViewTap:sender];
    }
}

@end
