//
//  SFInfoVer2FinanTableDataController.m
//  HouseCar
//
//  Created by shieh fabo on 14-9-10.
//  Copyright (c) 2014年 ytang.com. All rights reserved.
//

#import "HCHomePageTableDataController.h"

#define Section_headerView(section) [[_sections objectAtIndex:section] headerView]
#define HotActivityCellHeight 415
#define TravelNotesCellHeight 200

@implementation HCHomePageTableDataControllerSection

@end

@interface HCHomePageTableDataController()

@end

@implementation HCHomePageTableDataController

-(void)reloadData
{
    if (self.dataSource && ![self.dataSource isLoading] && ![self.dataSource isLoaded]) {
        [self.dataSource reloadData];
    }else{
        [self.tableView reloadData];
    }
    
    if (self.hotActivitiesDataSource && ![self.hotActivitiesDataSource isLoading] && ![self.hotActivitiesDataSource isLoaded]) {
        [self.hotActivitiesDataSource reloadData];
    }else{
        [self.tableView reloadData];
    }
}

-(void)refreshData
{
    [super refreshData];
    [self.hotActivitiesDataSource refreshData];
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
    if ([self.tableDatadelegate conformsToProtocol:@protocol(HCHomePageTableDataDelegate)]) {
        [self.tableDatadelegate HCHomePageTableDataClickedWithData:data];
    }
}

-(void)HCTableFocusCellDownloaded:(NSArray *)data error:(NSError *)error
{
    if (!error) {
        [self reloadTableHeaderViewData];
    }else{
        NSLog(@"加载轮播图失败");
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

-(NSInteger) numberOfSectionsInTableView:(UITableView *)tableView{
    return [_sections count];
}

-(NSInteger) tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    
    NSUInteger count = 0;
    if (section == TableSectionHotActivity) {
        count = [[self.hotActivitiesDataSource dataObjects] count];
    }
    
    if (section == TableSectionTravelNotes) {
        count = [[self.dataSource dataObjects] count];
    }
    
    if(count){
        return count + (Section_headerView(section) ? 1 : 0);
    }else{
        return count;
    }
}


- (CGFloat) tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    if(indexPath.row == 0 && Section_headerView(indexPath.section)){
        return Section_headerView(indexPath.section).frame.size.height;
    }else if(indexPath.section == TableSectionHotActivity){
        
        return HotActivityCellHeight;
        
    }else if(indexPath.section == TableSectionTravelNotes){
        
        return TravelNotesCellHeight;
        
    }else{
        return tableView.rowHeight;
    }
}

-(UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    HCHomePageTableDataControllerSection *headerSection =[_sections objectAtIndex:indexPath.section];
    if(indexPath.row == 0 && headerSection.headerView){
        UITableViewCell *headerCell =  headerSection.headerView;
        if([headerCell isKindOfClass:[VTTableViewCell class]]){
            [(VTTableViewCell *) headerCell setController:self];
            [(VTTableViewCell *) headerCell setDelegate:self];
        }
        return headerCell;
        
    }else{
        if (indexPath.section == TableSectionHotActivity){
            
            self.itemViewNib = @"HCHomeHotActivityItemCell";
            
        }else if (indexPath.section == TableSectionTravelNotes){
            
            self.itemViewNib = @"HCTravelNotesItemCell";
            
        }
        
        NSString *identifier = self.itemViewNib;
        
        NSBundle * bundle = [self itemViewBundle];
        
        if(bundle == nil){
            bundle = [self.context resourceBundle];
        }
        
        UITableViewCell * cell = [tableView dequeueReusableCellWithIdentifier:identifier];
        
        if(cell == nil){
            
            cell = (UITableViewCell *) [VTTableViewCell tableViewCellWithNibName:self.itemViewNib bundle:bundle];
            
            [cell setValue:identifier forKey:@"reuseIdentifier"];
            
            if([cell isKindOfClass:[VTTableViewCell class]]){
                [(VTTableViewCell *) cell setController:self];
                [(VTTableViewCell *) cell setDelegate:self];
            }
        }
        
        if(indexPath.row > 0){
            id data = nil;
            switch (indexPath.section) {
                case TableSectionHotActivity:
                {
                    data = [[self.hotActivitiesDataSource dataObjects] objectAtIndex:indexPath.row - 1];
                }
                    break;
                case TableSectionTravelNotes:
                {
                    data = [[self.dataSource dataObjects] objectAtIndex:indexPath.row - 1];
                }
                    break;
                default:
                    break;
            }
            
            if([cell isKindOfClass:[VTTableViewCell class]] && data){
                [(VTTableViewCell *) cell setContext:self.context];
                [(VTTableViewCell *) cell setDataItem:data];
            }
        }
        
        
        return cell;
    }
}

-(void) setSections:(NSArray *)sections{
    _sections = [sections sortedArrayUsingComparator:^NSComparisonResult(id obj1, id obj2) {
        NSInteger i = [obj1 index] - [obj2 index];
        if(i < 0 ){
            return NSOrderedAscending;
        }
        if(i > 0) {
            return NSOrderedDescending;
        }
        return NSOrderedSame;
    }];
}

- (void)setContext:(id<IVTUIContext>)context
{
    [super setContext:context];
    [_hotActivitiesDataSource setContext:context];
}

- (void)setHotActivitiesDataSource:(HCHotActivitiesDataSource *)hotActivitiesDataSource
{
    _hotActivitiesDataSource = hotActivitiesDataSource;
    _hotActivitiesDataSource.delegate = self;
}

- (void)dealloc
{
    [_hotActivitiesDataSource setDelegate:nil];
}

@end
