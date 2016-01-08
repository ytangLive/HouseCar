//
//  HCActivityOrderItemCell.m
//  HouseCar
//
//  Created by tangyin on 16/1/8.
//  Copyright © 2016年 sina.com. All rights reserved.
//

#import "HCActivityOrderItemCell.h"

@implementation HCActivityOrderItemCell

- (BOOL)textFieldShouldReturn:(UITextField *)textField
{
    [textField resignFirstResponder];
    return YES;
}


@end
