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
    if([actionName isEqualToString:@""]){
        
    }
}

@end
