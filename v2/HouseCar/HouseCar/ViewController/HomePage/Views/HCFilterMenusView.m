//
//  HCFilterMenusView.m
//  HouseCar
//
//  Created by tangyin on 15/11/25.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import "HCFilterMenusView.h"

@interface HCFilterMenusView()
@property (nonatomic,strong) NSMutableArray *itemCellArray;
@end

@implementation HCFilterMenusView
{
     UIView *_titleView;
}

- (instancetype)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if(self){
        self.backgroundColor = [UIColor whiteColor];
        [self updateUI];
    }
    return self;
}

- (void)updateUI
{
    _titleView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, self.bounds.size.width, 50.0f)];
    _titleView.backgroundColor = COLOR_HEX(0xD2D2D2);
    UIButton *clearButton = [UIButton buttonWithType:UIButtonTypeCustom];
    CGSize clearSize = CGSizeMake(65.0f, 26.0f);
    clearButton.frame = CGRectMake(15.0f, 12.0f, clearSize.width, clearSize.height);
    [clearButton setImage:[UIImage imageNamed:@"button_cleargray"] forState:UIControlStateNormal];
    //[clearButton setImage:[UIImage imageNamed:@"button_cleargray-press"] forState:UIControlStateHighlighted];
    [clearButton addTarget:self action:@selector(clearButtonTapped:) forControlEvents:UIControlEventTouchUpInside];
    
    UIButton *confirmButton = [UIButton buttonWithType:UIButtonTypeCustom];
    CGSize confirmSize = CGSizeMake(45.0f, 26.0f);
    confirmButton.frame = CGRectMake(self.bounds.size.width - confirmSize.width - 10.0f, 12.0f,
                                     confirmSize.width, confirmSize.height);
    [confirmButton setImage:[UIImage imageNamed:@"button_confirmgray"] forState:UIControlStateNormal];
    //[confirmButton setImage:[UIImage imageNamed:@"button_confirmgray-press"] forState:UIControlStateHighlighted];
    [confirmButton addTarget:self action:@selector(confirmButtonTapped:) forControlEvents:UIControlEventTouchUpInside];
    
    [_titleView addSubview:clearButton];
    [_titleView addSubview:confirmButton];
    
    _tableView = [[VTTableView alloc] initWithFrame:CGRectZero style:UITableViewStylePlain];
    _tableView.separatorStyle = UITableViewCellSeparatorStyleNone;
    _tableView.dataSource = self;
    _tableView.delegate = self;
    
    [self addSubview:_titleView];
    [self addSubview:_tableView];
}

- (void)layoutSubviews
{
    _tableView.frame = CGRectMake(0, CGRectGetMaxY(_titleView.frame), self.frame.size.width, self.frame.size.height);
}

#pragma mark - Property

- (NSMutableDictionary *)selectedItems
{
    if(!_selectedItems){
        _selectedItems = [[NSMutableDictionary alloc] init];
    }
    return _selectedItems;
}

- (void)setFilterMenus:(NSArray *)filterMenus
{
    _filterMenus = filterMenus;
    [self createCells];
    [self.tableView reloadData];
}

- (NSMutableArray *)itemCellArray
{
    if(!_itemCellArray){
        _itemCellArray = [[NSMutableArray alloc] init];
    }
    return _itemCellArray;
}

#pragma mark - UITableViewDataSource

static const CGFloat TableCellHeight = 44.0f;

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    return TableCellHeight;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return [self.itemCellArray count];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    if([self.itemCellArray count] > 0 ){
        UITableViewCell *cell = [self.itemCellArray objectAtIndex:[indexPath row]];
        
        NSString *detailText = @"全部";
        CGFloat detailSpace = 80.0f;
        if(![cell viewWithTag:999]){
            UILabel *detailLabel = [[UILabel alloc] initWithFrame:CGRectMake(detailSpace, 0,
                                                                             self.bounds.size.width - detailSpace - 27.0f, TableCellHeight)];
            detailLabel.backgroundColor = [UIColor clearColor];
            detailLabel.tag = 990;
            detailLabel.textColor = [UIColor redColor];
            detailLabel.textAlignment = NSTextAlignmentRight;
            detailLabel.alpha = 0.3;
            detailLabel.font = [UIFont systemFontOfSize:11.0f];
            detailLabel.text = detailText;
            
            [cell addSubview:detailLabel];
        }else{
            UILabel *detailLabel = (UILabel *)[cell viewWithTag:990];
            detailLabel.text = detailText;
        }

        return cell;
        
    }else{
        
        return nil;
        
    }
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    NSDictionary *menuItem = self.filterMenus[indexPath.row];
    NSString *classifyID = [menuItem stringValueForKey:@"classifyID"];
    if([classifyID isEqualToString:@"City"]){
        
        if(_delegate && [_delegate respondsToSelector:@selector(openCityList)]){
            [self.delegate openCityList];
        }
        
    }else if([classifyID isEqualToString:@"TCFL"]){
        
    }else if([classifyID isEqualToString:@"FCLX"]){
        
    }
    [self.tableView deselectRowAtIndexPath:indexPath animated:YES];
}

//创建行
- (void)createCells
{
    [self.itemCellArray removeAllObjects];
    
    for (int i = 0; i < [self.filterMenus count]; i++) {
        UITableViewCell *cell = [[UITableViewCell alloc]
                                 initWithStyle:UITableViewCellStyleDefault reuseIdentifier:@"ConditionsCell"];
        NSDictionary *menuItem = self.filterMenus[i];
        cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
        cell.textLabel.textColor = COLOR_HEX(0x666666);
        cell.textLabel.font = [UIFont systemFontOfSize:13.0f];
        cell.textLabel.text = [menuItem stringValueForKey:@"classifyName"];
        
        [cell.contentView addSubview:[UIView initLineWidthFrame:CGRectMake(10.0f, TableCellHeight - 1,CGRectGetWidth(self.frame) - 20.0f, 1)]];
        
        [self.itemCellArray addObject:cell];
    }
}

#pragma mark - buttonAction

- (void)clearButtonTapped:(UIButton *)sender
{
    [self.selectedItems removeAllObjects];
    
    [self.tableView reloadData];
}

- (void)confirmButtonTapped:(UIButton *)sender
{
    if (_delegate && [_delegate respondsToSelector:@selector(confirmSelectedItems:)]) {
        [self.delegate confirmSelectedItems:self.selectedItems];
    }
}


@end
