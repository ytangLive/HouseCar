//
//  HCMenusSwitchView.h
//  HouseCar
//
//  Created by TangYin on 12/20/15.
//  Copyright © 2015 sina.com. All rights reserved.
//

#import <UIKit/UIKit.h>

@protocol HCMenusSwitchViewDelegate <NSObject>

- (void)menusDoAction:(NSUInteger)buttonIndex;

@end

@interface HCMenusSwitchView : UIView

@property (weak, nonatomic)IBOutlet id <HCMenusSwitchViewDelegate> delegate;

@end
