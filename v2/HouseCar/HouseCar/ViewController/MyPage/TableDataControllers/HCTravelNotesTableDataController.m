//
//  HCTravelNotesTableDataController.m
//  HouseCar
//
//  Created by tangyin on 16/1/29.
//  Copyright © 2016年 sina.com. All rights reserved.
//

#import "HCTravelNotesTableDataController.h"
#import "HCTravelNotesDetailDataSource.h"

#define ColorChangeOffset 360

#define Section_headerView(section) [[((HCTravelNotesDetailDataSource *)self.dataSource).sections objectAtIndex:section] headerView]
#define TravelNotesDetailCellHeight 200

@implementation HCTravelNotesTableDataControllerSection

@end

@implementation HCTravelNotesTableDataController
{
    CGFloat _backgroundViewHeight;
}

- (void)awakeFromNib
{
    self.toolBarView.backgroundColor = [UIColor clearColor];
}

- (void)setContext:(id<IVTUIContext>)context
{
    [super setContext:context];
    [self.tableView setTableHeaderView:_tableViewHeaderView];
    _backgroundViewHeight = _backgroundView.height;
}


- (void)scrollViewDidScroll:(UIScrollView *)scrollView
{
    [super scrollViewDidScroll:scrollView];
    
    CGFloat yOffset   = self.tableView.contentOffset.y;
    if (yOffset < 0) {
        //向下滑动，图片高度增加相同的高度，宽度按比例增加 yOffset/3是为了减缓宽度变化速度
        CGFloat factor = ((ABS(yOffset/3)+_backgroundViewHeight) * KScreenWidth)/_backgroundViewHeight;
        //-(factor-ImageWidth)/2 使变宽的图片居中显示
        CGRect changeFrame = CGRectZero;
        changeFrame = CGRectMake(-(factor - KScreenWidth)/2, 0, factor, _backgroundViewHeight + ABS(yOffset));
        
        _backgroundView.frame = changeFrame;
        _shadowView.frame = changeFrame;
        
    }else{
        _backgroundView.frame = CGRectMake(0, -yOffset, KScreenWidth, _backgroundViewHeight);
        _shadowView.frame = CGRectMake(0, -yOffset, KScreenWidth, _backgroundViewHeight);
        
        CGFloat alphaValue = yOffset / ColorChangeOffset;
        
        if(alphaValue >= 1){
            alphaValue = 1;
        }
        
        _toolBarView.backgroundColor = [UIColor colorWithRed:51/255.0 green:187/255.0 blue:235/255.0 alpha:alphaValue];
    }
}

#pragma mark - table view delegate

-(NSInteger) numberOfSectionsInTableView:(UITableView *)tableView{
    return [((HCTravelNotesDetailDataSource *)self.dataSource).sections count];
}

-(NSInteger) tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    
    NSUInteger count = 0;
    
    NSDictionary *travelNotesInfos = ((HCTravelNotesDetailDataSource *)self.dataSource).travelNotesInfos;
    NSArray *trabelNotes = [travelNotesInfos arrayValueForKey:[NSString stringWithFormat:@"day_%ld",(long)section]];
    
    count = [trabelNotes count];
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
    }else{
        return tableView.rowHeight;
    }
}

-(UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    HCTravelNotesTableDataControllerSection *headerSection = [((HCTravelNotesDetailDataSource *)self.dataSource).sections objectAtIndex:indexPath.section];
    if(indexPath.row == 0 && headerSection.headerView){
        
        return headerSection.headerView;
        
    }else{
        
        self.itemViewNib = @"HCTravelNotesDetailItemCell";
        
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
            
            NSDictionary *travelNotesInfos = ((HCTravelNotesDetailDataSource *)self.dataSource).travelNotesInfos;
            NSArray *trabelNotes = [travelNotesInfos arrayValueForKey:[NSString stringWithFormat:@"day_%ld",(long)indexPath.section]];
            data = [trabelNotes objectAtIndex:indexPath.row - 1];
            
            if([cell isKindOfClass:[VTTableViewCell class]] && data){
                [(VTTableViewCell *) cell setContext:self.context];
                [(VTTableViewCell *) cell setDataItem:data];
            }
        }
        
        return cell;
    }
}

@end
