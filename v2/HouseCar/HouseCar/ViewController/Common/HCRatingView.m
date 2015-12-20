//
//  RatingView.m
//  BookOnline
//
//  Created by 唐周成 on 12-2-15.
//  Copyright 2012年 __MyCompanyName__. All rights reserved.
//

#import "HCRatingView.h"

@interface HCRatingView(private)
-(void)_commonInit;

@end

@implementation HCRatingView
{
    CGFloat _defaultWidth;
    CGRect _defaultFrame;
    BOOL flag;
}

#define MAX_RETING 5.0
#define GAP_SPACE 2.0

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
        [self _commonInit];
    }
    return self;
}

-(id)initWithCoder:(NSCoder *)aDecoder
{
    if ((self = [super initWithCoder:aDecoder])) {
        [self _commonInit];
    }
    return self;
}

-(void)setRating:(NSString *)rating
{
    _rating = rating;

   foregroundView.frame = CGRectMake(0.0, 0.0, _defaultWidth*([_rating floatValue] / MAX_RETING), foregroundView.bounds.size.height);
    
    self.frame = CGRectMake(_defaultFrame.origin.x, _defaultFrame.origin.y, self.bounds.size.width, self.bounds.size.height);
}

-(void)_commonInit
{
    _defaultWidth = self.frame.size.width;
    _defaultFrame = self.frame;
    
    backgroundView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, self.frame.size.width, self.frame.size.height)];
    foregroundView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, self.frame.size.width, self.frame.size.height)];
    
    for (int i = 0; i < MAX_RETING; i++) {
        CGFloat imageWidth = (_defaultWidth - GAP_SPACE * (MAX_RETING - 1)) / MAX_RETING;
        CGFloat gapSpace = 0;
        UIImageView *halfStars = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"icon_starUnselected"]];
        UIImageView *fullStars = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"icon_starSelected"]];
        if(i > 0){
            gapSpace = GAP_SPACE;
        }
        halfStars.frame = CGRectMake(i * (imageWidth + gapSpace), 0, imageWidth, self.frame.size.height);
        fullStars.frame = CGRectMake(i * (imageWidth + gapSpace), 0, imageWidth, self.frame.size.height);
        [backgroundView addSubview:halfStars];
        [foregroundView addSubview:fullStars];
    }
    
    backgroundView.contentMode = UIViewContentModeLeft;
    backgroundView.clipsToBounds = YES;
    [self addSubview:backgroundView];

    foregroundView.contentMode = UIViewContentModeLeft;
    foregroundView.clipsToBounds = YES;
    [self addSubview:foregroundView];
}

- (BOOL)isPureFloat:(NSString *)string
{
    return [string rangeOfString:@"."].location != NSNotFound;
}

@end
