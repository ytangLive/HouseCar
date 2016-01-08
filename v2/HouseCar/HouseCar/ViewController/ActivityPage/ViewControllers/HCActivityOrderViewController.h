//
//  HCActivityOrderViewController.h
//  HouseCar
//
//  Created by tangyin on 16/1/6.
//  Copyright © 2016年 sina.com. All rights reserved.
//

#import "HCViewController.h"

@interface HCActivityOrderViewController : HCViewController <UITableViewDataSource, UITableViewDelegate>

@property (nonatomic, weak) IBOutlet VTTableView *tableView;
@property (nonatomic, weak) IBOutlet UILabel *totalLabel;

@end
