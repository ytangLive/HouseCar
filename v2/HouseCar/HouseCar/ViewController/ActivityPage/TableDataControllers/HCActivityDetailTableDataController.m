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

#define ActivityAttendUserCellHeight 65
#define ActivityDetailLocationCellHeight 260
#define ActivityDetailAttendInfoCellHeight 220
#define ActivityDetailCommentCellHeight 50

#define TravelRouteImageHeight 220

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
        
    }else if(section == TableSectionTypeComment){
        
        return [[(HCActivityDetailDataSource *)self.dataSource activityComments] count] > 0 ? [[(HCActivityDetailDataSource *)self.dataSource activityComments] count] + 1 : 0;
        
    }
    
    return 0;
}

- (CGFloat) tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (indexPath.section == TableSectionTypeAttendUser) {
        
        return ActivityAttendUserCellHeight;
        
    }else if (indexPath.section == TableSectionTypeLocation) {
        
        return ActivityDetailLocationCellHeight;
        
    }else if (indexPath.section == TableSectionTypeAttendInfo) {
        
        return ActivityDetailAttendInfoCellHeight;
        
    }else if(indexPath.section == TableSectionTypetravelRoute){
        
        UITableViewCell *cell = [VTTableViewCell tableViewCellWithNibName:@"HCActivityDetailTravelRouteCell" bundle:nil];
        
        id data = [[(HCActivityDetailDataSource *)self.dataSource travelRouteInfos] objectAtIndex:indexPath.row];
        
        NSString *describe = [data stringValueForKey:@"describe"];
        NSArray *imageInfo = [data arrayValueForKey:@"imageInfo"];
        
        UIView *describeView = [cell.contentView viewWithTag:991];
        UILabel *describeLabel = [cell.contentView viewWithTag:993];
        
        CGFloat defaultHeight = describeLabel.height;
        CGFloat gapSpace = 8;
        CGFloat describeViewWith = KScreenWidth - describeView.minX - gapSpace;
        CGFloat regularLabelW = describeViewWith - 2*gapSpace;
        
        CGSize describeSize = [describe VTSizeWithFont:describeLabel.font constrainedToSize:CGSizeMake(regularLabelW, CGFLOAT_MAX) lineBreakMode:NSLineBreakByWordWrapping];
        
        CGFloat variables = describeSize.height - defaultHeight;
        CGFloat cellHeight = cell.height + variables;
        CGFloat imageHeight = 0;
        
        if(imageInfo && [imageInfo count] > 0){
            imageHeight = TravelRouteImageHeight * imageInfo.count + gapSpace * (imageInfo.count - 1);
        }
        
        return cellHeight + imageHeight;
        
    }else if(indexPath.section == TableSectionTypeComment){
        if(indexPath.row == 0){
            return _commentHeaderView ? _commentHeaderView.height : 0;
        }else{
            return ActivityDetailCommentCellHeight;
        }
        
    }
    
    return tableView.rowHeight;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    if(indexPath.section == TableSectionTypeLocation){
        
        self.itemViewNib = @"HCActivityDetailLocationCell";
        
    }else if(indexPath.section == TableSectionTypeAttendInfo){
        
        self.itemViewNib = @"HCActivityDetailAttendInfoCell";
        
    }else if(indexPath.section == TableSectionTypetravelRoute){
        
        self.itemViewNib = @"HCActivityDetailTravelRouteCell";
        
    }else if(indexPath.section == TableSectionTypeComment){
        if(indexPath.row > 0){
            self.itemViewNib = @"HCCampPlayCommentItem";
        }
        
        
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
        
        
        CGFloat floatNum = (KScreenWidth - 2 * edgeSpace - CGRectGetMaxX(textLabel.frame) + imageGapSpace) /  (userImageSize.width + imageGapSpace);
        
        NSInteger imageCount = [[self decimalwithFormat:@"0" floatV:floatNum] integerValue];
        
        imageCount = imageCount > [usersInfo count] ? [usersInfo count] : imageCount;
        
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
            
            if(imageCount < [usersInfo count] && i == (imageCount - 1)){
                UIButton *moreUserBtn = [UIButton buttonWithType:UIButtonTypeCustom];
                moreUserBtn.backgroundColor = COLOR_RGB_ALPHA(0, 0, 0, 0.5);
                moreUserBtn.frame = imageView.frame;
                moreUserBtn.layer.cornerRadius = userImageSize.width / 2.f;
                moreUserBtn.layer.masksToBounds = NO;
                moreUserBtn.clipsToBounds = YES;
                [moreUserBtn setTitle:@"•••" forState:UIControlStateNormal];
                [moreUserBtn addTarget:self action:@selector(moreUserInfo:) forControlEvents:UIControlEventTouchUpInside];
                
                [cell.contentView addSubview:moreUserBtn];
            }else{
                [imageView addGestureRecognizer:[[UIGestureRecognizer alloc] initWithTarget:self action:@selector(userDetailInfo:)]];
            }
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
            
            NSString *title = [data stringValueForKey:@"title"];
            NSString *describe = [data stringValueForKey:@"describe"];
            NSArray *imageInfo = [data arrayValueForKey:@"imageInfo"];
            
            UILabel *dayLabel = [cell.contentView viewWithTag:990];
            UIView *describeView = [cell.contentView viewWithTag:991];
            UILabel *titleLabel = [cell.contentView viewWithTag:992];
            UILabel *describeLabel = [cell.contentView viewWithTag:993];
            
            CGFloat defaultHeight = describeLabel.height;
            CGFloat gapSpace = 8;
            CGFloat describeViewWith = KScreenWidth - describeView.minX - gapSpace;
            CGFloat regularLabelW = describeViewWith - 2*gapSpace;
            
            CGSize describeSize = [describe VTSizeWithFont:describeLabel.font constrainedToSize:CGSizeMake(regularLabelW, CGFLOAT_MAX) lineBreakMode:NSLineBreakByWordWrapping];
            
            CGFloat variables = describeSize.height - defaultHeight;
            CGFloat regularHeight = describeView.height + variables;
            
            describeLabel.frame = CGRectMake(describeLabel.minX, describeLabel.minY, describeSize.width, describeSize.height);
            describeLabel.text = describe;
            dayLabel.frame = CGRectMake(dayLabel.minX, dayLabel.minY, dayLabel.width, regularHeight);
            describeView.frame = CGRectMake(describeView.minX, describeView.minY, describeViewWith, regularHeight);
            
            dayLabel.text = [NSString stringWithFormat:@"%ld",(long)indexPath.row + 1];
            titleLabel.text = title;
            describeLabel.text = describe;
            
            for (UIView *view in [cell.contentView subviews]) {
                if([view isKindOfClass:[VTImageView class]]){
                    [view removeFromSuperview];
                }
            }
            
            if(imageInfo && [imageInfo count] > 0){
                
                for (int i = 0; i < [imageInfo count]; i++) {
                    NSString *imageUrl = [imageInfo objectValueAtIndex:i];
                    
                    VTImageView *imageView = [[VTImageView alloc] initWithFrame:CGRectMake(dayLabel.minX , dayLabel.maxY + gapSpace + (TravelRouteImageHeight + gapSpace) * i, KScreenWidth - 2*gapSpace, TravelRouteImageHeight)];
                    imageView.src = imageUrl;
                    
                    if([imageView isLoading]){
                        [self.context cancelHandle:@protocol(IVTImageTask) task:imageView];
                    }
                    if(![imageView isLoaded]&&![imageView isLoading]){
                        [self.context handle:@protocol(IVTImageTask) task:imageView priority:0];
                    }
                    
                    [cell.contentView addSubview:imageView];
                }
            }
            
        }else if(indexPath.section == TableSectionTypeComment){
            if(indexPath.row == 0){
                
                return _commentHeaderView;

            }else{
                data = [[(HCActivityDetailDataSource *)self.dataSource activityComments] objectAtIndex:indexPath.row - 1];
            }
        }
        
        if(data){
            if([cell isKindOfClass:[VTTableViewCell class]] &&
               indexPath.section != TableSectionTypetravelRoute){
                
                [(VTTableViewCell *) cell setContext:self.context];
                [(VTTableViewCell *) cell setDataItem:data];
                
            }
        }
        
        
        return cell;
        
    }
}

- (void)userDetailInfo:(id)sender
{
    
}

- (void)moreUserInfo:(id)sender
{
    
}

//格式话小数 四舍五入类型
- (NSString *) decimalwithFormat:(NSString *)format floatV:(float)floatV
{
    NSNumberFormatter *numberFormatter = [[NSNumberFormatter alloc] init];
    
    [numberFormatter setPositiveFormat:format];
    
    return  [numberFormatter stringFromNumber:[NSNumber numberWithFloat:floatV]];
}

@end
