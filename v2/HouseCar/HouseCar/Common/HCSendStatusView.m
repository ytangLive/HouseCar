//
//  HCSendStatusView.m
//  HourseCar
//
//  Created by Tang yin on 16-3-31.
//  Copyright (c) 2016年 ytang.com. All rights reserved.
//

#import "HCSendStatusView.h"

@implementation HCSendStatusView

-(id)initWithImage:(NSString *)imgName title:(NSString *)title {
    if((self = [super init])){
        UIImage *image = [UIImage imageNamed:imgName];
        UIImageView *imageView = [[UIImageView alloc] initWithImage:image];
        imageView.frame = CGRectMake(13, (20 - image.size.height)/2.0, image.size.width, image.size.height);
        [self addSubview:imageView];
        
        UILabel * label = [[UILabel alloc] initWithFrame:CGRectMake(imageView.frame.origin.x + imageView.frame.size.width + 6, 0, self.bounds.size.width, self.bounds.size.height)];
        [label setBackgroundColor:[UIColor clearColor]];
        [label setTextAlignment:NSTextAlignmentLeft];
        [label setFont:[UIFont systemFontOfSize:12]];
        [label setTextColor:[UIColor whiteColor]];
        [label setAutoresizingMask:UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight];
        [label setText:title];
        [self addSubview:label];
    }
    return self;
}


@end
