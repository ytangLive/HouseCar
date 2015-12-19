//
//  SFTabBar.m
//  SinaFinance
//
//  Created by zhang hailong on 14-3-28.
//  Copyright (c) 2014年 ytang.com. All rights reserved.
//

#import "HCTabBar.h"

@implementation HCTabBar


+(void) initialize{
    [super initialize];
    
}

-(id) initWithCoder:(NSCoder *)aDecoder{
    if((self = [super initWithCoder:aDecoder])){
        
        self.bubbleImageview = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"dian.png"]];
        self.bubbleImageview.frame = CGRectMake(174, 6, 8, 8);
        [self addSubview:self.bubbleImageview];
        [self.bubbleImageview setHidden:YES];
        
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(didReceiveNewOpinionNotification:) name:HCNewOpinionNotification object:nil];
    }
    return self;
}

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
        
        self.bubbleImageview = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"dian.png"]];
        self.bubbleImageview.frame = CGRectMake(174, 6, 8, 8);
        [self addSubview:self.bubbleImageview];
        [self.bubbleImageview setHidden:YES];
        
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(didReceiveNewOpinionNotification:) name:HCNewOpinionNotification object:nil];
    }
    return self;
}

- (void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self name:HCNewOpinionNotification object:nil];
}

/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect
{
    // Drawing code
}
*/

-(void) didReceiveNewOpinionNotification:(NSNotification *) notification{
    
    NSDictionary * userInfo  =[notification userInfo];
    
    BOOL isBubbleShow = [[userInfo valueForKey:HCNewOpinionBubbleShowKey] boolValue];
    [self.bubbleImageview setHidden:!isBubbleShow];
}

@end
