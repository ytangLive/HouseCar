//
//  HCSearchBarView.h
//  HouseCar
//
//  Created by a on 14-11-25.
//  Copyright (c) 2014年 ytang.com. All rights reserved.
//

#import <UIKit/UIKit.h>
@class HCSearchBarView;

@protocol HCSearchBarViewViewDelegate <NSObject>

@optional
- (void)searchBtnClick:(HCSearchBarView *)sender searchKey:(NSString *)searchStr;

- (void)textFieldDidBeginEditing:(UITextField *)textField;

@end

@interface HCSearchBarView : UIView

@property (nonatomic,weak) IBOutlet id<HCSearchBarViewViewDelegate> delegate;
@property (nonatomic,retain)UITextField *textField;

@end
