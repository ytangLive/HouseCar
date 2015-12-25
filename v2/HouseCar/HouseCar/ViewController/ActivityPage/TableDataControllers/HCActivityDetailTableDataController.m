//
//  HCActivityDetailTableDataController.m
//  HouseCar
//
//  Created by tangyin on 15/12/24.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import "HCActivityDetailTableDataController.h"
#import "HCActivityDetailDataSource.h"
#import "HCCircleView.h"

#define AttendUserCellHeight 65

@implementation HCActivityDetailTableDataController

- (void)reloadTableHeaderViewData
{
    NSDictionary *headerViewDic = ((HCActivityDetailDataSource *)self.dataSource).headerViewDic;
    _tableHeaderView.imageView.src = [headerViewDic stringValueForKey:@"imgUrl"];
    _tableHeaderView.label.text = [headerViewDic stringValueForKey:@"title"];
    
    if([_tableHeaderView.imageView isLoading]){
        [self.context cancelHandle:@protocol(IVTImageTask) task:_tableHeaderView.imageView];
    }
    if(![_tableHeaderView.imageView isLoaded]&&![_tableHeaderView.imageView isLoading]){
        [self.context handle:@protocol(IVTImageTask) task:_tableHeaderView.imageView priority:0];
    }
    
    [self.tableView setTableHeaderView:_tableHeaderView];
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
    return 5;
}

-(NSInteger) tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    
    if (section == TableSectionTypeAttendUser) {
        
        return [[(HCActivityDetailDataSource *)self.dataSource attendUsers] count];
        
    }else if (section == TableSectionTypeLocation) {
        
        return [[(HCActivityDetailDataSource *)self.dataSource locationInfos] count];
        
    }else if (section == TableSectionTypeAttendInfo) {
        
        return [[(HCActivityDetailDataSource *)self.dataSource attendInfos] count];
        
    }else if(section == TableSectionTypetravelRoute){
        
        return [[(HCActivityDetailDataSource *)self.dataSource travelRouteInfos] count];
        
    }else if(section == TableSectionTypeCostActivity){
        
        return [[(HCActivityDetailDataSource *)self.dataSource costActivityInfos] count];
        
    }
    
    return 0;
}

- (CGFloat) tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (indexPath.section == TableSectionTypeAttendUser) {
        
        return AttendUserCellHeight;
        
    }else if (indexPath.section == TableSectionTypeLocation) {
        
        return 100;
        
    }else if (indexPath.section == TableSectionTypeAttendInfo) {
        
        return 100;
        
    }else if(indexPath.section == TableSectionTypetravelRoute){
        
        return 100;
        
    }else if(indexPath.section == TableSectionTypeCostActivity){
        
        return 100;
        
    }
    
    return 100;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    if(indexPath.section == TableSectionTypeLocation){
        
        self.itemViewNib = @"HCActivityDetailLocationCell";
        
    }else if(indexPath.section == TableSectionTypeAttendInfo){
        
        self.itemViewNib = @"HCActivityDetailAttendInfoCell";
        
    }else if(indexPath.section == TableSectionTypetravelRoute){
        
        self.itemViewNib = @"HCActivityDetailTravelRouteCell";
        
    }else if(indexPath.section == TableSectionTypeCostActivity){
        
        self.itemViewNib = @"HCActivityDetailCostActivityCell";
        
    }
    
    if(indexPath.section == TableSectionTypeAttendUser){
        
        NSString * identifier = @"HCActivityDetailAttendUserCell";
        
        UITableViewCell * cell = [tableView dequeueReusableCellWithIdentifier:identifier];
        
        if(cell == nil){
            cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:identifier];
            cell.selectionStyle = UITableViewCellSelectionStyleNone;
        }
        
        NSDictionary *data = [[(HCActivityDetailDataSource *)self.dataSource attendUsers] objectAtIndex:indexPath.row];
        NSString *totalUser = [NSString stringWithFormat:@"/%@",[data stringValueForKey:@"totalUser"]];
        NSString *attendUser = [data stringValueForKey:@"attendUser"];
        NSArray *usersInfo = [data arrayValueForKey:@"usersInfo"];
        
        UIFont *labelFont = [UIFont systemFontOfSize:14];
        CGSize totalSize = [totalUser VTSizeWithFont:labelFont constrainedToSize:CGSizeMake(CGFLOAT_MAX, CGFLOAT_MAX) lineBreakMode:NSLineBreakByTruncatingTail];
        CGSize attendSize = [[NSString stringWithFormat:@"%@",attendUser] VTSizeWithFont:labelFont constrainedToSize:CGSizeMake(CGFLOAT_MAX, CGFLOAT_MAX) lineBreakMode:NSLineBreakByTruncatingTail];
        
        CGFloat edgeSpace = 15;
        CGFloat imageGapSpace = 10;
        CGSize userImageSize = CGSizeMake(40, 40);
        
        UILabel *attendLabel = [[UILabel alloc] initWithFrame:CGRectMake(edgeSpace, 15, attendSize.width, attendSize.height)];
        attendLabel.font = labelFont;
        attendLabel.textColor = DefaultNavBarColor;
        attendLabel.text = attendUser;
        
        UILabel *totaleLabel = [[UILabel alloc] initWithFrame:CGRectMake(CGRectGetMaxX(attendLabel.frame), CGRectGetMinY(attendLabel.frame), totalSize.width, totalSize.height)];
        totaleLabel.font = labelFont;
        totaleLabel.textColor = COLOR_HEX(0x999999);
        totaleLabel.text = totalUser;
        
        UILabel *textLabel = [[UILabel alloc] initWithFrame:CGRectMake(edgeSpace, CGRectGetMaxY(attendLabel.frame), 45, 20)];
        textLabel.font = labelFont;
        textLabel.textColor = DefaultNavBarColor;
        textLabel.text = @"已报名";
        
        [cell.contentView addSubview:attendLabel];
        [cell.contentView addSubview:totaleLabel];
        [cell.contentView addSubview:textLabel];
        
        
        NSUInteger imageCount = (KScreenWidth - 3 * edgeSpace - CGRectGetMaxX(textLabel.frame)) /  (userImageSize.width + imageGapSpace);
        
        imageCount = imageCount > [usersInfo count] ? [usersInfo count] : (imageCount - 1);
        
        for(int i = 0; i < imageCount; i++){
            NSDictionary *userInfo = [usersInfo objectAtIndex:i];
            NSString *pUrl = [userInfo stringValueForKey:@"pUrl"];
            HCCircleView *imageView = [[HCCircleView alloc] initWithFrame:CGRectMake(CGRectGetMaxX(textLabel.frame) + edgeSpace + (userImageSize.width + imageGapSpace) * i, 12, userImageSize.width, userImageSize.height)];
            imageView.userInteractionEnabled = YES;
            imageView.defaultImageStr = @"default_headimage";
            imageView.src = pUrl;
            
            if([imageView isLoading]){
                [self.context cancelHandle:@protocol(IVTImageTask) task:imageView];
            }
            if(![imageView isLoaded]&&![imageView isLoading]){
                [self.context handle:@protocol(IVTImageTask) task:imageView priority:0];
            }
            
             [cell.contentView addSubview:imageView];
        }
        
        
        
        return cell;
        
    }else{
        
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
            
            data = [[(HCActivityDetailDataSource *)self.dataSource locationInfos] objectAtIndex:indexPath.row];
            
        }else if(indexPath.section == TableSectionTypeAttendInfo){
            
            data = [[(HCActivityDetailDataSource *)self.dataSource attendInfos] objectAtIndex:indexPath.row];
            
        }else if(indexPath.section == TableSectionTypetravelRoute){
            
            data = [[(HCActivityDetailDataSource *)self.dataSource travelRouteInfos] objectAtIndex:indexPath.row];
            
        }else if(indexPath.section == TableSectionTypeCostActivity){
            
            data = [[(HCActivityDetailDataSource *)self.dataSource costActivityInfos] objectAtIndex:indexPath.row];
            
        }
        
        if(data){
            if([cell isKindOfClass:[VTTableViewCell class]]){
                [(VTTableViewCell *) cell setContext:self.context];
                [(VTTableViewCell *) cell setDataItem:data];
            }
        }
        
        
        return cell;
        
    }
}

@end
