//
//  RatingView.h
//  BookOnline
//
//  Created by 唐周成 on 12-2-15.
//  Copyright 2012年 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface HCRatingView : UIView
{
    UIView * backgroundView;
    UIView * foregroundView;
}

@property(nonatomic, strong) NSString *rating;

@end
