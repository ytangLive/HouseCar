//
//  NSString+VTDOMStringSize.h
//  vTeam
//
//  Created by jixuhui on 15/2/6.
//  Copyright (c) 2015年 hailong.org. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface NSString (VTDOMStringSize)
- (CGSize)VTSizeWithFont:(UIFont *)font constrainedToSize:(CGSize)size lineBreakMode:(NSLineBreakMode)lineBreakMode;
- (CGSize)VTSizeWithFont:(UIFont *)font minFontSize:(CGFloat)minFontSize actualFontSize:(CGFloat *)actualFontSize forWidth:(CGFloat)width lineBreakMode:(NSLineBreakMode)lineBreakMode;
- (void)VTDrawInRect:(CGRect)rect withFont:(UIFont *)font lineBreakMode:(NSLineBreakMode)lineBreakMode alignment:(NSTextAlignment)alignment color:(UIColor *)color;
@end
