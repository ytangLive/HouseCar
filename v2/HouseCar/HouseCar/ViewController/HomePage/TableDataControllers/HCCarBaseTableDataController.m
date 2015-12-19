//
//  SFInfoVer2FinanTableDataController.m
//  HouseCar
//
//  Created by shieh fabo on 14-9-10.
//  Copyright (c) 2014年 ytang.com. All rights reserved.
//

#import "HCCarBaseTableDataController.h"

@interface HCCarBaseTableDataController()
@property(nonatomic,strong)NSMutableArray *suspends;//需要悬停的cell集合,目前支持1个
@property(nonatomic,strong)UIView *suspendView;;//需要悬停的View
@end

@implementation HCCarBaseTableDataController

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

- (NSDate *)defaultDate
{
    if(!_defaultDate){
        _defaultDate = [NSDate dateWithTimeIntervalSinceNow:24*60*60];
    }
    return _defaultDate;
}


- (NSDate *)takeDate
{
    if(!_takeDate){
        _takeDate = self.defaultDate;
    }
    return _takeDate;
}

- (NSDate *)returnDate
{
    if(!_returnDate){
        _returnDate = self.defaultDate;
    }
    return _returnDate;
}

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
    if ([self.tableDatadelegate conformsToProtocol:@protocol(HCCarBaseTableDataDelegate)]) {
        [self.tableDatadelegate HCCarBaseTableDataClickedWithData:data];
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

- (CGFloat) tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
//    if (indexPath.row == 0) {
//        return 46;
//    }else{
        //return 81;
    //}
    
    if(indexPath.row < [self.headerCells count]){
        return [[self.headerCells objectAtIndex:indexPath.row] frame].size.height;
    }
    
    if(indexPath.row >= [self.dataSource count] + [self.headerCells count]){
        return [[self.footerCells objectAtIndex:indexPath.row
                 - [self.dataSource count] - [self.headerCells count]] frame].size.height;
    }
    
    if([_pageType isEqualToString:@"carBook"]){
        return 81;
    }else if([_pageType isEqualToString:@"campPlay"]){
        return 200;
    }else{
        return 81;
    }
    
}

-(UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
//    if (indexPath.row == 0) {
//        self.itemViewNib = @"HCCarBookMenuCell";
//    }else{
        //self.itemViewNib = @"HCCarBookItemCell";
   // }
    
    if([_pageType isEqualToString:@"carBook"]){
        
        self.itemViewNib = @"HCCarBookItemCell";
        
    }else if([_pageType isEqualToString:@"campPlay"]){
        
        self.itemViewNib = @"HCCampPlayItemCell";
        
    }else{
        self.itemViewNib = @"HCCarBookItemCell";
    }
    
    UITableViewCell *tableViewCell = [super tableView:tableView cellForRowAtIndexPath:indexPath];
 
    if(tableViewCell){
        if([_pageType isEqualToString:@"carBook"]){
            if(indexPath.row == 0){
                VTButton *takeButton  = (VTButton *)[tableViewCell viewWithTag:996];
                VTButton *returnButton  = (VTButton *)[tableViewCell viewWithTag:997];
                NSString *takeDateStr = [self.defaultDateFormatter stringFromDate:self.takeDate];
                NSString *returnDateStr = [self.defaultDateFormatter stringFromDate:self.returnDate];
                NSString *takeStr = @"取车 ";
                NSString *returnStr = @"还车 ";
                [takeButton setTitle:[takeStr stringByAppendingString:takeDateStr] forState:UIControlStateNormal];
                [returnButton setTitle:[returnStr stringByAppendingString:returnDateStr] forState:UIControlStateNormal];
                
    //            if(!_suspends && [_suspends count] == 0){
    //                [self.suspends addObject:tableViewCell];
    //            }
            }else{
                UILabel *titleLabel  = (UILabel *)[tableViewCell viewWithTag:998];
                UILabel *tagLabel  = (UILabel *)[tableViewCell viewWithTag:999];
                if(titleLabel && tagLabel){
                    NSDictionary *dataItem = ((VTTableViewCell *)tableViewCell).dataItem;
                    NSString *title = [dataItem stringValueForKey:@"title"];
                    CGSize titleSize = [title VTSizeWithFont:titleLabel.font constrainedToSize:CGSizeMake(titleLabel.frame.size.width, CGFLOAT_MAX) lineBreakMode:NSLineBreakByTruncatingTail];
                    
                    tagLabel.frame = CGRectMake(titleLabel.frame.origin.x + titleSize.width + 9, tagLabel.frame.origin.y, tagLabel.frame.size.width, tagLabel.frame.size.height);
                }
            }
        }else if([_pageType isEqualToString:@"campPlay"]){
            
        }
    }
    
    return tableViewCell;
}

#pragma mark -- UIScrollViewDelegate
//- (void)scrollViewDidScroll:(UIScrollView *)scrollView{
//    if (_suspends.count > 0) {//如果有需要悬停的cell
//        VTTableViewCell *cell = (VTTableViewCell *)[self tableView:self.tableView cellForRowAtIndexPath:[NSIndexPath indexPathForRow:0 inSection:0]];
//        //获取悬停cell在TableView中的高度
//        CGRect rectInTableView = [self.tableView rectForRowAtIndexPath:[self indexPathInTableView:cell]];
//
//        //悬停的控制
//        if ((scrollView.contentOffset.y - rectInTableView.origin.y) > 0 && (scrollView.contentOffset.y - rectInTableView.origin.y)!= 0){
//            cell.frame = CGRectMake(cell.frame.origin.x, cell.frame.origin.y, rectInTableView.size.width, rectInTableView.size.height);
//            [self.suspendView addSubview:cell];
//            if (self.suspendView.hidden == YES) {
//                self.suspendView.hidden = NO;
//            }
//        }
//        if ((scrollView.contentOffset.y - rectInTableView.origin.y) < 0){
//            if (self.suspendView.hidden == NO) {
//                self.suspendView.hidden = YES;
//                self.suspendView = nil;
//            }
//        }
//    }
//}

#pragma mark -- Tools
- (NSIndexPath *)indexPathInTableView:(UITableViewCell *)cell{
    NSIndexPath * index = [self.tableView indexPathForCell:cell];
    return index;
}



@end
