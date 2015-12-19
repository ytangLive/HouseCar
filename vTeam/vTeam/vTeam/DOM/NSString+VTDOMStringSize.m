//
//  NSString+VTDOMStringSize.m
//  vTeam
//
//  Created by jixuhui on 15/2/6.
//  Copyright (c) 2015年 hailong.org. All rights reserved.
//

#import "NSString+VTDOMStringSize.h"

@implementation NSString (VTDOMStringSize)

- (CGSize)VTSizeWithFont:(UIFont *)font constrainedToSize:(CGSize)size lineBreakMode:(NSLineBreakMode)lineBreakMode{
    /*iOS9由于字体变化，所以为了在界面显示上不出错，就算是固定长度的文字也还是建议使用对size向上取整 ceilf()*/
    if ([self getCurDeviceVersion]<7.0) {
        return [self sizeWithFont:font constrainedToSize:size lineBreakMode:lineBreakMode];
    }else{
        NSMutableParagraphStyle *paragraphStyle = [[NSMutableParagraphStyle alloc]init];
        paragraphStyle.lineBreakMode = lineBreakMode;
        NSDictionary *attributes = @{NSFontAttributeName:font, NSParagraphStyleAttributeName:paragraphStyle};
        if ([self getCurDeviceVersion] < 9.0) {
            return [self boundingRectWithSize:size options:NSStringDrawingUsesLineFragmentOrigin attributes:attributes context:nil].size;
        }else{
            CGSize currentSize = [self boundingRectWithSize:size options:NSStringDrawingUsesLineFragmentOrigin attributes:attributes context:nil].size;
            CGSize adjustedSize = CGSizeMake(ceilf(currentSize.width), ceilf(currentSize.height));
            return adjustedSize;
        }
    }
}

- (CGSize)VTSizeWithFont:(UIFont *)font minFontSize:(CGFloat)minFontSize actualFontSize:(CGFloat *)actualFontSize forWidth:(CGFloat)width lineBreakMode:(NSLineBreakMode)lineBreakMode{
    /*iOS9由于字体变化，所以为了在界面显示上不出错，就算是固定长度的文字也还是建议使用对size向上取整 ceilf()*/
    if ([self respondsToSelector:@selector(sizeWithFont:minFontSize:actualFontSize:forWidth:lineBreakMode:)]) {
        if ([self getCurDeviceVersion] < 9.0) {
            return [self sizeWithFont:font minFontSize:minFontSize actualFontSize:actualFontSize forWidth:width lineBreakMode:NSLineBreakByClipping];
        }else{
            CGSize currentSize = [self sizeWithFont:font minFontSize:minFontSize actualFontSize:actualFontSize forWidth:width lineBreakMode:NSLineBreakByClipping];
            CGSize adjustedSize = CGSizeMake(ceilf(currentSize.width), ceilf(currentSize.height));
            return adjustedSize;
        }
    }else{
        NSStringDrawingContext *drawingContext = [[[NSStringDrawingContext alloc] init] autorelease];
        drawingContext.minimumScaleFactor = minFontSize/[font pointSize]; // Half the font size
        
        NSMutableParagraphStyle *paragraphStyle = [[NSMutableParagraphStyle alloc]init];
        paragraphStyle.lineBreakMode = lineBreakMode;
        NSDictionary *attributes = @{NSFontAttributeName:font, NSParagraphStyleAttributeName:paragraphStyle};
        [paragraphStyle release];
        
        CGSize s = [self boundingRectWithSize:CGSizeMake(width,CGFLOAT_MAX) options:NSStringDrawingUsesLineFragmentOrigin attributes:attributes context:drawingContext].size;
        *actualFontSize = drawingContext.actualScaleFactor*[font pointSize];
        if ([self getCurDeviceVersion] < 9.0) {
            return s;
        }else{
            CGSize adjustedSize = CGSizeMake(ceilf(s.width), ceilf(s.height));
            return adjustedSize;
        }
        
    }
//    if ([self getCurDeviceVersion]<7.0) {
//        return [self sizeWithFont:font minFontSize:minFontSize actualFontSize:actualFontSize forWidth:width lineBreakMode:NSLineBreakByClipping];
//    }else{
//        NSStringDrawingContext *drawingContext = [[[NSStringDrawingContext alloc] init] autorelease];
//        drawingContext.minimumScaleFactor = minFontSize/[font pointSize]; // Half the font size
//        
//        NSMutableParagraphStyle *paragraphStyle = [[NSMutableParagraphStyle alloc]init];
//        paragraphStyle.lineBreakMode = lineBreakMode;
//        NSDictionary *attributes = @{NSFontAttributeName:font, NSParagraphStyleAttributeName:paragraphStyle};
//        [paragraphStyle release];
//        
//        CGSize s = [self boundingRectWithSize:CGSizeMake(width,CGFLOAT_MAX) options:NSStringDrawingUsesLineFragmentOrigin attributes:attributes context:drawingContext].size;
//        *actualFontSize = drawingContext.actualScaleFactor*[font pointSize];
//        return s;
//    }
}

- (void)VTDrawInRect:(CGRect)rect withFont:(UIFont *)font lineBreakMode:(NSLineBreakMode)lineBreakMode alignment:(NSTextAlignment)alignment color:(UIColor *)color{
    if ([self getCurDeviceVersion]<7.0) {
        [self drawInRect:rect
                withFont:font lineBreakMode:lineBreakMode alignment:alignment];
    }else{
        NSStringDrawingContext *drawingContext = [[[NSStringDrawingContext alloc] init] autorelease];
        drawingContext.minimumScaleFactor = 0.5;
        
        NSMutableParagraphStyle *paragraphStyle = [[NSMutableParagraphStyle alloc]init];
        paragraphStyle.lineBreakMode = lineBreakMode;
        paragraphStyle.alignment = alignment;
        NSDictionary *attributes = @{NSFontAttributeName:font,NSParagraphStyleAttributeName:paragraphStyle,NSForegroundColorAttributeName:color};
        [paragraphStyle release];
        
        [self drawWithRect:rect options:NSStringDrawingUsesLineFragmentOrigin attributes:attributes context:drawingContext];
    }
}

-(float)getCurDeviceVersion
{
    return [[[UIDevice currentDevice] systemVersion] floatValue];
}

@end
