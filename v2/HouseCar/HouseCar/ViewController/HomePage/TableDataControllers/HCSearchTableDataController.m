//
//  HCSearchTableDataController.m
//  HouseCar
//
//  Created by tangyin on 15/12/11.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import "HCSearchTableDataController.h"
#import "HCSearchDataSource.h"
#import "HCHistorySearchGetTask.h"
#import "HCHistorySearchDataObject.h"
#import "IHCHistorySearchClearTask.h"

#define Section_headerViewCell(section) [[_sections objectAtIndex:section] headerViewCell]

#define HOT_SEARCH_KEY_MIDDLE_SPACE 5
#define HOT_SEARCH_KEY_EDGE_SPACE 15
#define HOT_SEARCH_BUTTON_HEIGHT 30
#define HOT_SEARCH_BUTTON_WIDTH 25
#define HOT_SEARCH_BUTTON_TOTAL_WIDTH KScreenWidth - 50 //热词需要展示的总体宽度
#define HOT_SEARCH_CELL_HEIGHT 40
#define HISTORY_SEARCH_CELL_HEIGHT 50

@implementation HCSearchTableDataControllerSection

@end

@interface HCSearchTableDataController()

@property (nonatomic, strong)NSMutableArray *hotSearchKeyArray;

@end

@implementation HCSearchTableDataController

#pragma mark - Property

- (NSMutableArray *)hotSearchKeyArray
{
    if(!_hotSearchKeyArray){
        _hotSearchKeyArray = [NSMutableArray array];
        NSArray *keyArray = [(HCSearchDataSource *)self.dataSource hotSearchArr];
        if (keyArray) {
            //keyArray = @[@"欧莱雅",@"防晒",@"美丽日志",@"T恤",@"彪马彪马表",@"斯莱德嘶",@"彪马",@"nike"];
            CGFloat currentWidth = HOT_SEARCH_KEY_MIDDLE_SPACE;
            NSMutableArray * subKeyArray = [[NSMutableArray alloc] initWithCapacity:0];
            for (int i = 0; i < keyArray.count; i++) {
                NSString * key = [keyArray objectAtIndex:i];
                CGSize size = [key VTSizeWithFont:[UIFont systemFontOfSize:15] constrainedToSize:CGSizeMake(HOT_SEARCH_BUTTON_TOTAL_WIDTH, HOT_SEARCH_BUTTON_HEIGHT) lineBreakMode:NSLineBreakByWordWrapping];
                if ((currentWidth + size.width + HOT_SEARCH_KEY_MIDDLE_SPACE + HOT_SEARCH_BUTTON_WIDTH > HOT_SEARCH_BUTTON_TOTAL_WIDTH) || (i == keyArray.count - 1)){
                    if ((i == keyArray.count - 1) && (currentWidth + size.width + HOT_SEARCH_KEY_MIDDLE_SPACE + HOT_SEARCH_BUTTON_WIDTH > HOT_SEARCH_BUTTON_TOTAL_WIDTH)) {
                        [_hotSearchKeyArray addObject:[NSArray arrayWithArray:subKeyArray]];
                        [subKeyArray removeAllObjects];
                        [subKeyArray addObject:key];
                        [_hotSearchKeyArray addObject:[NSArray arrayWithArray:subKeyArray]];
                        
                    }else{
                        if (i == keyArray.count - 1) {
                            [subKeyArray addObject:key];
                        }
                        [_hotSearchKeyArray addObject:[NSArray arrayWithArray:subKeyArray]];
                        [subKeyArray removeAllObjects];
                        currentWidth = HOT_SEARCH_KEY_MIDDLE_SPACE;
                    }
                }
                [subKeyArray addObject:key];
                currentWidth += size.width + HOT_SEARCH_KEY_MIDDLE_SPACE + HOT_SEARCH_BUTTON_WIDTH;
            }
        }

    }
    return _hotSearchKeyArray;
}

- (void)reloadData
{
    [super reloadData];
    
    HCHistorySearchGetTask *getTask = [[HCHistorySearchGetTask alloc] init];
    [self.context handle:@protocol(IHCHistorySearchGetTask) task:getTask priority:0];
    
    [(HCSearchDataSource *)self.dataSource setHistorySearchKeys:[getTask searchKeys]];
    
    [self stopLoading];
}

- (void)refreshData
{
    
    HCHistorySearchGetTask *getTask = [[HCHistorySearchGetTask alloc] init];
    [self.context handle:@protocol(IHCHistorySearchGetTask) task:getTask priority:0];
    
    [(HCSearchDataSource *)self.dataSource setHistorySearchKeys:[getTask searchKeys]];
    
    [self.tableView reloadData];
    
    [self stopLoading];
}

#pragma mark - Table view delegate

-(NSInteger) numberOfSectionsInTableView:(UITableView *)tableView{
    return [_sections count];
}

-(NSInteger) tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    
    NSUInteger count = 0;
    if (section == TableSectionTypeHot) {
        count = [self.hotSearchKeyArray count];
    }
    
    if (section == TableSectionTypeHistory) {
        count = [[(HCSearchDataSource *)self.dataSource historySearchKeys] count] ? [[(HCSearchDataSource *)self.dataSource historySearchKeys] count] + 1 : 0;
    }
    
    if(count){
        return count + (Section_headerViewCell(section) ? 1 : 0);
    }else{
        return count;
    }
}


- (CGFloat) tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    if(indexPath.row == 0 && Section_headerViewCell(indexPath.section)){
        return Section_headerViewCell(indexPath.section).frame.size.height;
    }else{
        if (indexPath.section == TableSectionTypeHot) {
            return HOT_SEARCH_CELL_HEIGHT;
        }else if (indexPath.section == TableSectionTypeHistory) {
            return HISTORY_SEARCH_CELL_HEIGHT;
        }
        return 50;
    }
}


-(UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    HCSearchTableDataControllerSection *headerSection =[_sections objectAtIndex:indexPath.section];
    if(indexPath.row == 0 && headerSection.headerViewCell){
        
        UITableViewCell *headerCell =  headerSection.headerViewCell;
        
        return headerCell;
        
    }else{
        NSString *identifier = @"TableViewCell";
        
        if(indexPath.section == TableSectionTypeHot){
          
            identifier = @"HotSearchTableViewCell";
            

            
        }else if(indexPath.section == TableSectionTypeHistory){
            
            NSArray *historySearchKeys = [(HCSearchDataSource *)self.dataSource historySearchKeys];
            if(indexPath.row > [historySearchKeys count]){
                identifier = @"HistoryClearButtonTableViewCell";
            }else{
                identifier = @"HistorySearchTableViewCell";
            }
            
        }
        
        UITableViewCell * cell = [tableView dequeueReusableCellWithIdentifier:identifier];
        
        if(cell == nil){
            
            cell = [(UITableViewCell *) [VTTableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:identifier];
            
            if([cell isKindOfClass:[VTTableViewCell class]]){
                [(VTTableViewCell *) cell setController:self];
                [(VTTableViewCell *) cell setDelegate:self];
            }
        }
        
        if(indexPath.row > 0){
            if(indexPath.section == TableSectionTypeHot){
                
                NSArray * hotKeyArray = [_hotSearchKeyArray objectAtIndex:indexPath.row - 1];
                if(hotKeyArray){
                    for (UIView * view in cell.contentView.subviews) {
                        [view removeFromSuperview];
                    }
                   
                    CGFloat current_X = HOT_SEARCH_KEY_EDGE_SPACE;
                    for (int i = 0; i < hotKeyArray.count; i++) {//循环布局button
                        NSString * hotKey = [hotKeyArray objectAtIndex:i];
                        CGSize size = [hotKey VTSizeWithFont:[UIFont systemFontOfSize:15] constrainedToSize:CGSizeMake(HOT_SEARCH_BUTTON_TOTAL_WIDTH, HOT_SEARCH_BUTTON_HEIGHT) lineBreakMode:NSLineBreakByWordWrapping];
                        UIButton * button = [UIButton buttonWithType:UIButtonTypeCustom];
                        button.frame = CGRectMake(current_X, (HOT_SEARCH_CELL_HEIGHT - HOT_SEARCH_BUTTON_HEIGHT)/2, size.width+HOT_SEARCH_BUTTON_WIDTH, HOT_SEARCH_BUTTON_HEIGHT);
                        current_X+=size.width + HOT_SEARCH_KEY_MIDDLE_SPACE + HOT_SEARCH_BUTTON_WIDTH;
                        [button setTitle:hotKey forState:UIControlStateNormal];
                        button.titleLabel.font = [UIFont systemFontOfSize:15];
                        button.backgroundColor = DefaultNavBarColor;
                        [button setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
                        
                        [button addTarget:self action:@selector(hotSearchButtonTouchUpInside:) forControlEvents:UIControlEventTouchUpInside];
                        cell.selectionStyle = UITableViewCellSelectionStyleNone;
                        [cell.contentView addSubview:button];
                    }
                }
                
            }else if(indexPath.section == TableSectionTypeHistory){
                NSArray *historySearchKeys = [(HCSearchDataSource *)self.dataSource historySearchKeys];
                if([historySearchKeys count] > 0){
                    if(indexPath.row > [historySearchKeys count]){
                        UIButton *clearButton = [cell viewWithTag:999];
                        if(!clearButton){
                            clearButton = [UIButton buttonWithType:UIButtonTypeCustom];
                            CGSize buttonSize = CGSizeMake(180, 30);
                            clearButton.frame = CGRectMake(KScreenWidth/2 - buttonSize.width/2 , HISTORY_SEARCH_CELL_HEIGHT/2, buttonSize.width, buttonSize.height);
                            [clearButton setTitle:@"清除历史" forState:UIControlStateNormal];
                            clearButton.titleLabel.font = [UIFont systemFontOfSize:14];
                            [clearButton setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
                            clearButton.backgroundColor = DefaultNavBarColor;
                            clearButton.layer.cornerRadius = 3.0f;
                            clearButton.tag = 999;
                            [clearButton addTarget:self action:@selector(clearHistorySearch:) forControlEvents:UIControlEventTouchUpInside];
                            
                            cell.selectionStyle = UITableViewCellSelectionStyleNone;
                            
                            [cell addSubview:clearButton];
                        }
                        
                        return cell;
                        
                    }else{
                        HCHistorySearchDataObject *dataObject = [historySearchKeys objectAtIndex:indexPath.row - 1];
                        UIView *lineView = [cell viewWithTag:998];
                        UIButton *clearButton = [cell viewWithTag:999];
                        if(clearButton){
                            [clearButton removeFromSuperview];
                        }
                        if(!lineView){
                            lineView = [UIView initLineWidthFrame:CGRectMake(15, HISTORY_SEARCH_CELL_HEIGHT - 1, KScreenWidth - 30, 1)];
                            lineView.tag = 998;
                            [cell addSubview:lineView];
                        }
                        cell.textLabel.text = dataObject.searchKey;
                        
                        return cell;
                    }
                    
                }
            }
        
        }
        
        return cell;
    }
}

- (void)clearHistorySearch:(id)sender
{
    [self.context handle:@protocol(IHCHistorySearchClearTask) task:nil priority:0];
    [(HCSearchDataSource *)self.dataSource setHistorySearchKeys:nil];
    
    [self.tableView reloadData];
}

- (void)hotSearchButtonTouchUpInside:(id)sender
{
    
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

@end
