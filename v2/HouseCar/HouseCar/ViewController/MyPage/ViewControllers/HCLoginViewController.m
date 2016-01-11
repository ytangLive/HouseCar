//
//  HCLoginViewController.m
//  HouseCar
//
//  Created by tangyin on 16/1/11.
//  Copyright © 2016年 sina.com. All rights reserved.
//

#import "HCLoginViewController.h"

@interface HCLoginViewController ()

@end

@implementation HCLoginViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)doAction:(id)sender
{
    NSString *actionName = [sender actionName];
    NSDictionary *queryValues = nil;
    if([actionName isEqualToString:@"register"]){
        queryValues = @{@"pageType":@"register"};
    }else if([actionName isEqualToString:@"forgetpassword"]){
        queryValues = @{@"pageType":@"forgetpassword"};
    }
    
    [self openUrl:[NSURL URLWithString:[sender userInfo]
                         relativeToURL:self.url
                           queryValues:queryValues] animated:YES];
}

@end
