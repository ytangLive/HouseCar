//
//  SFAnimationView.h
//  SinaFinance
//
//  Created by sinanews on 13-9-18.
//  Copyright (c) 2013年 sina.com. All rights reserved.
//

#import <VTeam/VTAnimationView.h>

@interface HCAnimationView : VTAnimationView {
    BOOL _animating;
}

@property (assign,nonatomic) CGFloat offsetValue;
@property (copy,nonatomic) NSString *backgroundImg;
@property (copy,nonatomic) NSString *coverImg;
@property (copy,nonatomic) NSString *borderImg;
@property (copy,nonatomic) NSString *loopImg;

-(void)reloadRes;

@end
