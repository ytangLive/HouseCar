//
//  SFAutoLabel.m
//  VTTeamProject
//
//  Created by fabo on 14/11/25.
//  Copyright (c) 2014年 ytang.com. All rights reserved.
//

#import "HCAutoLabel.h"

@interface HCAutoLabel ()

@property(nonatomic,assign)CGRect oldRect;
@property(nonatomic,assign)BOOL bSizeToFiting;

@end

@implementation HCAutoLabel


-(void)setFrame:(CGRect)frame
{
    BOOL oldRectChanged = NO;
    if (!self.bSizeToFiting&&!CGRectEqualToRect(self.oldRect, frame)) {
        oldRectChanged = YES;
        _oldRect = frame;
    }
    [super setFrame:frame];
    if (oldRectChanged) {
        [self sizeToFit];
    }
}

-(void)sizeToFit
{
    self.frame = self.oldRect;
    _bSizeToFiting = YES;
    [super sizeToFit];
    _bSizeToFiting = NO;
}

-(void)setText:(NSString *)text
{
    [super setText:text];
    [self sizeToFit];
}


/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect {
    // Drawing code
}
*/

@end
