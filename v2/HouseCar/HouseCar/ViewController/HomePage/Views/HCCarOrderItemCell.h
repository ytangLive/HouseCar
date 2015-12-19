//
//  HCCarOrderItemCell.h
//  HouseCar
//
//  Created by tangyin on 15/12/2.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import <vTeam/vTeam.h>

@protocol HCCarOrderItemCellDelegate <NSObject>

- (void)orderDaysViewTap:(id)sender;

- (void)carTakeWayViewTap:(id)sender;

@end

@interface HCCarOrderItemCell : VTTableViewCell <UITextFieldDelegate>

@property (weak, nonatomic) IBOutlet UILabel *titleLabel;
@property (weak, nonatomic) IBOutlet UILabel *locationLabel;
@property (weak, nonatomic) IBOutlet UILabel *pNumLabel;
@property (weak, nonatomic) IBOutlet UILabel *priceLabel;

@property (weak, nonatomic) IBOutlet UILabel *orderDateLabel;
@property (weak, nonatomic) IBOutlet UILabel *daysLabel;
@property (weak, nonatomic) IBOutlet UILabel *amountLabel;

@property (nonatomic, weak) id<HCCarOrderItemCellDelegate> orderCellDeletate;

@end
