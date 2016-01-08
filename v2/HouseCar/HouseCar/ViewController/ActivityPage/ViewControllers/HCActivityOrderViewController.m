//
//  HCActivityOrderViewController.m
//  HouseCar
//
//  Created by tangyin on 16/1/6.
//  Copyright © 2016年 sina.com. All rights reserved.
//

#import "HCActivityOrderViewController.h"

#define ActivityOrderCellHeight 420

@interface HCActivityOrderViewController () <VTTableViewCellDelegate>

@property (nonatomic, strong)NSArray *dataObjects;
@property (nonatomic) UIEdgeInsets tableViewContentInset;

@end

@implementation HCActivityOrderViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    
    NSDictionary *params = [self.url queryValues];
    NSString *jsonStr = [params stringValueForKey:@"activityDetails"];
    if(jsonStr){
        _dataObjects = [VTJSON decodeText:jsonStr];
    }
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(keyboardWillShown:) name:UIKeyboardWillShowNotification object:nil];
    
    [[NSNotificationCenter defaultCenter]  addObserver:self selector:@selector(keyboardWillHidden:) name:UIKeyboardWillHideNotification object:nil];
    
    _tableViewContentInset = _tableView.contentInset;
    
    _tableView.backgroundColor = TableViewBackgroundColor;
    
    [_tableView reloadData];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (NSArray *)dataObjects
{
    if(!_dataObjects){
        _dataObjects = [[NSArray alloc] init];
    }
    return _dataObjects;
}

#pragma mark - table view delegate

-(NSInteger) tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    
    return [self.dataObjects count];
}

- (CGFloat) tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    return ActivityOrderCellHeight;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    NSString *itemViewNib = @"HCActivityOrderItemCell";
    NSString * identifier = itemViewNib;
    
    UITableViewCell * cell = [tableView dequeueReusableCellWithIdentifier:identifier];
    
    if(cell == nil){
        
        cell = (UITableViewCell *) [VTTableViewCell tableViewCellWithNibName:itemViewNib bundle:nil];
        
        [cell setValue:identifier forKey:@"reuseIdentifier"];
        
        if([cell isKindOfClass:[VTTableViewCell class]]){
            [(VTTableViewCell *) cell setDelegate:self];
        }
    }
    
    id data = [self.dataObjects objectAtIndex:indexPath.row];
    
    if([cell isKindOfClass:[VTTableViewCell class]]){
        [(VTTableViewCell *) cell setContext:self.context];
        [(VTTableViewCell *) cell setDataItem:data];
    }
    
    
    return cell;
}

#pragma mark - VTTableViewCellDelegate

- (void)vtTableViewCell:(VTTableViewCell *)tableViewCell doAction:(id)action
{
    NSString *actionName = [action actionName];
    UILabel *adultLabel = [tableViewCell viewWithTag:100];
    UILabel *childLabel = [tableViewCell viewWithTag:101];
    
    if([actionName isEqualToString:@"adultPlus"]){
        NSInteger adultCount = [adultLabel.text integerValue];
        adultCount += 1;
        if(adultCount > 99){
            adultCount = 99;
        }
        adultLabel.text = [NSString stringWithFormat:@"%ld",(long)adultCount];
        
    }else if([actionName isEqualToString:@"adultMinus"]){
        
        NSInteger adultCount = [adultLabel.text integerValue];
        adultCount -= 1;
        if(adultCount < 0){
            adultCount = 0;
        }
        adultLabel.text = [NSString stringWithFormat:@"%ld",(long)adultCount];
        
    }else if([actionName isEqualToString:@"childPlus"]){
        
        NSInteger childCount = [childLabel.text integerValue];
        childCount += 1;
        if(childCount > 99){
            childCount = 99;
        }
        childLabel.text = [NSString stringWithFormat:@"%ld",(long)childCount];
        
    }else if([actionName isEqualToString:@"childMinus"]){
        
        NSInteger childCount = [childLabel.text integerValue];
        childCount -= 1;
        if(childCount < 0){
            childCount = 0;
        }
        childLabel.text = [NSString stringWithFormat:@"%ld",(long)childCount];
    }
}

#pragma mark - KeyboardNotification

- (void) keyboardWillShown:(NSNotification *)notif
{
    
    NSDictionary *info = [notif userInfo];
    NSValue *value = [info objectForKey:UIKeyboardFrameBeginUserInfoKey];
    CGRect keyboardRect = [value CGRectValue];
    CGSize keyboardSize = keyboardRect.size;
    
    if(_tableView){
        [_tableView setContentInset:UIEdgeInsetsMake(_tableViewContentInset.top, _tableViewContentInset.left, keyboardSize.height + 20, _tableViewContentInset.right)];
    }
}

- (void) keyboardWillHidden:(NSNotification *)notif
{
    if(_tableView){
        [_tableView setContentInset:_tableViewContentInset];
    }
}

@end
