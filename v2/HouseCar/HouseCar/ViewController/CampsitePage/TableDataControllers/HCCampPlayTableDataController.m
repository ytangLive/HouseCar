//
//  SFInfoVer2FinanTableDataController.m
//  HouseCar
//
//  Created by shieh fabo on 14-9-10.
//  Copyright (c) 2014年 ytang.com. All rights reserved.
//

#import "HCCampPlayTableDataController.h"
#import "HCCampPlayDataSource.h"

@implementation HCCampPlayTableDataControllerSection

@end

@interface HCCampPlayTableDataController()

@end

@implementation HCCampPlayTableDataController

-(void)reloadData
{
    if(![self.focusController.dataSource isLoading]){
        [self reloadFocusData];
    }
    [super reloadData];
}

-(void)refreshData
{

    [super refreshData];
}

- (HCTableFocusViewController *)focusController
{
    if(!_focusController){
        _focusController = [[HCTableFocusViewController alloc] init];
    }
    return _focusController;
}

-(void)reloadFocusData
{
    self.focusController.context = self.context;
    self.focusController.celltitleKey = @"title";
    self.focusController.cellsrcKey = @"p_url";
    self.focusController.delegate = self;
    
    //初始化focusController视图
    self.focusController.view.frame = self.focusController.view.frame;
    
    self.focusController.dataSource.urlKey = self.dataUrlKey;
    self.focusController.dataSource.dataKey = @"post";
    if(((HCCampPlayDataSource *)self.dataSource).queryValues){
        self.focusController.dataSource.queryValues = ((HCCampPlayDataSource *)self.dataSource).queryValues;
    }
    [self.focusController reloadData];
}

#pragma mark =========delegate========
-(void)HCTableFocusCellClickedWithData:(NSDictionary*)data
{
    if ([self.tableDatadelegate conformsToProtocol:@protocol(HCCampPlayTableDataDelegate)]) {
        [self.tableDatadelegate HCCampPlayTableDataClickedWithData:data];
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

-(CGFloat) tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section{
    
    if (section == TableSectionTypeTitle) {
        if ([[(HCCampPlayDataSource *)self.dataSource contentArr] count] <= 0) {
            return 0;
        }
    }else if (section == TableSectionTypeComment){
        if ([[(HCCampPlayDataSource *)self.dataSource commentArr] count] <= 0) {
            return 0;
        }
    }
    
    return [[[_sections objectAtIndex:section] headerView] frame].size.height;
}

-(UIView *) tableView:(UITableView *)tableView viewForHeaderInSection:(NSInteger)section{
    
    if (section == TableSectionTypeTitle){
        if ([[(HCCampPlayDataSource *)self.dataSource contentArr] count] <= 0) {
            return nil;
        }
    }else if (section == TableSectionTypeComment){
        if ([[(HCCampPlayDataSource *)self.dataSource commentArr] count] <= 0) {
            return nil;
        }
    }
    return [[_sections objectAtIndex:section] headerView];
}

-(NSInteger) numberOfSectionsInTableView:(UITableView *)tableView{
    return [_sections count];
}

-(NSInteger) tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    
    if (section == TableSectionTypeTitle) {
        return [[(HCCampPlayDataSource *)self.dataSource contentArr] count];
    }
    
    if (section == TableSectionTypeComment) {
        return [[(HCCampPlayDataSource *)self.dataSource commentArr] count];
    }
    
    return 0;
}


- (CGFloat) tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    return tableView.rowHeight;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    if(indexPath.section == TableSectionTypeTitle){
        if(indexPath.row == 0){
            self.itemViewNib = @"HCCampPlayLocationItem";
        }else{
            self.itemViewNib = @"HCCampPlayTitleItem";
        }
        
    }else if(indexPath.section == TableSectionTypeComment){
        
        self.itemViewNib = @"HCCampPlayCommentItem";
        
    }

    NSString * identifier = self.itemViewNib;
    
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
    
    id data = nil;
    
    if(indexPath.section == TableSectionTypeTitle){
        
        data = [[(HCCampPlayDataSource *)self.dataSource contentArr] objectAtIndex:indexPath.row];
        
    }else if(indexPath.section == TableSectionTypeComment){
        
        data = [[(HCCampPlayDataSource *)self.dataSource commentArr] objectAtIndex:indexPath.row];
    }
    
    if(data){
        if([cell isKindOfClass:[VTTableViewCell class]]){
            [(VTTableViewCell *) cell setContext:self.context];
            [(VTTableViewCell *) cell setDataItem:data];
        }
    }
    
    return cell;
}

@end
