//
//  HCMenusSwitchView.m
//  HouseCar
//
//  Created by TangYin on 12/20/15.
//  Copyright © 2015 sina.com. All rights reserved.
//

#import "HCMenusSwitchView.h"

@interface HCMenusSwitchView()
@property (weak, nonatomic) IBOutlet UIView *bottomLineView;
@property (strong, nonatomic) IBOutletCollection(UIButton) NSArray *menuButtons;
@end

@implementation HCMenusSwitchView

- (IBAction)doAction:(UIButton *)sender
{
    NSUInteger baseTag = 100;
    
    for (UIButton *menuButton in _menuButtons) {
        if(menuButton.tag != sender.tag){
            menuButton.selected = NO;
        }else{
            menuButton.selected = YES;
        }
    }
    
    [UIView animateWithDuration:0.3 animations:^{
        _bottomLineView.frame = CGRectMake(sender.minX, _bottomLineView.minY, _bottomLineView.width, _bottomLineView.height);
    }];
    
    if(_delegate && [_delegate respondsToSelector:@selector(menusDoAction:)]){
        [self.delegate menusDoAction:sender.tag - baseTag];
    }
}

@end
