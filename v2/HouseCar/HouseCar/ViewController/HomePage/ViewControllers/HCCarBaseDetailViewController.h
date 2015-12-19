//
//  HCCommonWebViewController.h
//  HouseCar
//
//  Created by tangyin on 15/12/1.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "HCViewController.h"
#import "HCNavigationTitleBar.h"

@interface HCCarBaseDetailViewController : HCViewController

@property (weak, nonatomic) IBOutlet HCNavigationTitleBar *titleBar;

@property (strong, nonatomic) NSString *requestUrl;

@end
