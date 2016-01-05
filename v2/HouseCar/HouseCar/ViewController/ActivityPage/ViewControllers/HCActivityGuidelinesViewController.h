//
//  HCActivityGuidelinesViewController.h
//  HouseCar
//
//  Created by tangyin on 16/1/5.
//  Copyright © 2016年 sina.com. All rights reserved.
//

#import "HCViewController.h"

@interface HCActivityGuidelinesViewController : HCViewController <UITableViewDataSource, UITableViewDelegate>

@property (nonatomic, weak) IBOutlet VTTableView *tableView;

@end
