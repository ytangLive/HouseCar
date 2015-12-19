//
//  HCCarOrderItemCell.m
//  HouseCar
//
//  Created by tangyin on 15/12/2.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import "HCCarOrderItemCell.h"

@interface HCCarOrderItemCell()

@end

@implementation HCCarOrderItemCell

- (BOOL)textFieldShouldReturn:(UITextField *)textField
{
    [textField resignFirstResponder];
    return YES;
}

- (IBAction)orderDaysViewTap:(id)sender {
    if(_orderCellDeletate && [_orderCellDeletate respondsToSelector:@selector(orderDaysViewTap:)]){
        [_orderCellDeletate orderDaysViewTap:sender];
    }
}


- (IBAction)carTakeWayViewTap:(id)sender {
    if(_orderCellDeletate && [_orderCellDeletate respondsToSelector:@selector(carTakeWayViewTap:)]){
        [_orderCellDeletate carTakeWayViewTap:sender];
    }
}

@end
