//
//  HCCityViewController.h
//  HouseCar
//
//  Created by tangyin on 15/11/17.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface HCCityViewController : VTViewController <UITableViewDataSource, UITabBarDelegate>

@property (nonatomic, weak) IBOutlet UITableView *tableView;

@property (nonatomic, strong) NSArray *cities;
@property (nonatomic, strong) NSArray *cityKeys;

@end

