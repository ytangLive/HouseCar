//
//  HCActivityDetailTableDataController.m
//  HouseCar
//
//  Created by tangyin on 15/12/24.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import "HCActivityDetailTableDataController.h"
#import "HCActivityDetailDataSource.h"

#define TableHeaderViewHeight 168

@implementation HCActivityDetailTableDataController

- (void)reloadTableHeaderViewData
{
    VTImageView *imageView = [[VTImageView alloc] initWithFrame:CGRectMake(0, 0, KScreenWidth, TableHeaderViewHeight)];
    imageView.autoresizingMask = UIViewAutoresizingFlexibleHeight|UIViewAutoresizingFlexibleWidth;
    imageView.src = ((HCActivityDetailDataSource *)self.dataSource).headerViewUrl;
    
    if([imageView isLoading]){
        [self.context cancelHandle:@protocol(IVTImageTask) task:imageView];
    }
    if(![imageView isLoaded]&&![imageView isLoading]){
        [self.context handle:@protocol(IVTImageTask) task:imageView priority:0];
    }
    
    [self.tableView setTableHeaderView:imageView];
}

- (void)vtDataSourceDidLoadedFromCache:(VTDataSource *)dataSource timestamp:(NSDate *)timestamp
{
    [self reloadTableHeaderViewData];
    [super vtDataSourceDidLoadedFromCache:dataSource timestamp:timestamp];
}

- (void)vtDataSourceDidLoaded:(VTDataSource *)dataSource
{
    
    [self reloadTableHeaderViewData];
    [super vtDataSourceDidLoaded:dataSource];
}

-(NSInteger) numberOfSectionsInTableView:(UITableView *)tableView{
    return 4;
}

-(NSInteger) tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    
//    if (section == TableSectionTypeTitle) {
//        return [[(HCCampPlayDataSource *)self.dataSource contentArr] count];
//    }
//    
//    if (section == TableSectionTypeComment) {
//        return [[(HCCampPlayDataSource *)self.dataSource commentArr] count];
//    }
    
    return 0;
}

- (CGFloat) tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    return 100;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    if(indexPath.section == TableSectionTypeLocation){
        self.itemViewNib = @"HCCampPlayCommentItem";
        
    }else if(indexPath.section == TableSectionTypeAttendInfo){
        
        self.itemViewNib = @"HCCampPlayCommentItem";
        
    }else if(indexPath.section == TableSectionTypetravelRoute){
        
        self.itemViewNib = @"HCCampPlayCommentItem";
        
    }else if(indexPath.section == TableSectionTypeAttendInfo){
        
        self.itemViewNib = @"HCCampPlayCommentItem";
        
    }else if(indexPath.section == TableSectionTypeCostActivity){
        
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
    
    if(indexPath.section == TableSectionTypeLocation){
        
        //data = [[(HCActivityDetailDataSource *)self.dataSource locationInfos] objectAtIndex:indexPath.row];
        
    }else if(indexPath.section == TableSectionTypeAttendInfo){
        
        //data = [[(HCActivityDetailDataSource *)self.dataSource commentArr] objectAtIndex:indexPath.row];
        
    }else if(indexPath.section == TableSectionTypetravelRoute){
        
        //data = [[(HCActivityDetailDataSource *)self.dataSource commentArr] objectAtIndex:indexPath.row];
        
    }else if(indexPath.section == TableSectionTypeAttendInfo){
        
        //data = [[(HCActivityDetailDataSource *)self.dataSource commentArr] objectAtIndex:indexPath.row];
        
    }else if(indexPath.section == TableSectionTypeCostActivity){
        
        //data = [[(HCActivityDetailDataSource *)self.dataSource commentArr] objectAtIndex:indexPath.row];
        
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
