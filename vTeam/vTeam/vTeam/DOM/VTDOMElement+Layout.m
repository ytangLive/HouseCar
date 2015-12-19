//
//  VTDOMElement+Layout.m
//  vTeam
//
//  Created by zhang hailong on 13-8-14.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#import "VTDOMElement+Layout.h"
#import "VTDOMElement+Style.h"
#import "VTDOMBRElement.h"



@implementation VTDOMElement (Layout)

-(CGSize) contentSize{
    return [[self valueForKey:@"contentSize"] CGSizeValue];
}

-(void) setContentSize:(CGSize)contentSize{
    [self setValue:[NSValue valueWithCGSize:contentSize] forKey:@"contentSize"];
}

-(UIEdgeInsets) padding{
    
    UIEdgeInsets insets = UIEdgeInsetsZero;

    NSString * padding = [self stringValueForKey:@"padding"];
    NSString * paddingLeft = [self stringValueForKey:@"padding-left"];
    NSString * paddingTop = [self stringValueForKey:@"padding-top"];
    NSString * paddingRight = [self stringValueForKey:@"padding-right"];
    NSString * paddingBottom = [self stringValueForKey:@"padding-bottom"];
    
    CGFloat value = [padding floatValue];
    
    insets.left = paddingLeft == nil ? value : [paddingLeft floatValue];
    insets.top = paddingTop == nil ? value : [paddingTop floatValue];
    insets.right = paddingRight == nil ? value : [paddingRight floatValue];
    insets.bottom = paddingBottom == nil ? value : [paddingBottom floatValue];
    
    return insets;
}

-(UIEdgeInsets) margin{
    
    UIEdgeInsets insets = UIEdgeInsetsZero;
    
    NSString * margin = [self stringValueForKey:@"margin"];
    NSString * marginLeft = [self stringValueForKey:@"margin-left"];
    NSString * marginTop = [self stringValueForKey:@"margin-top"];
    NSString * marginRight = [self stringValueForKey:@"margin-right"];
    NSString * marginBottom = [self stringValueForKey:@"margin-bottom"];
    
    CGFloat value = [margin floatValue];
    
    insets.left = marginLeft == nil ? value : [marginLeft floatValue];
    insets.top = marginTop == nil ? value : [marginTop floatValue];
    insets.right = marginRight == nil ? value : [marginRight floatValue];
    insets.bottom = marginBottom == nil ? value : [marginBottom floatValue];
    
    return insets;
}

-(CGSize) layoutChildren:(UIEdgeInsets) padding{
    
    CGRect frame = [self frame];
    CGSize size = CGSizeZero;
    CGSize insetSize = CGSizeMake(frame.size.width - padding.left - padding.right
                                  , frame.size.height - padding.top - padding.bottom);
    
    NSString * layout = [self stringValueForKey:@"layout"];
    
    if([layout isEqualToString:@"flow"]){
        
        CGPoint p = CGPointMake(padding.left, padding.top);
        CGFloat lineHeight = 0;
        CGFloat width = padding.left + padding.right;
        CGFloat maxWidth = frame.size.width;
        
        if(maxWidth == MAXFLOAT){
            NSString * max = [self attributeValueForKey:@"max-width"];
            if(max){
                maxWidth = [max floatValue];
            }
        }
        
        NSMutableArray* wexpanedChilds = [NSMutableArray arrayWithCapacity:0];
        NSMutableArray* wnormalChilds = [NSMutableArray arrayWithCapacity:0];
        NSMutableArray* hexpanedChilds = [NSMutableArray arrayWithCapacity:0];
        NSMutableArray* hnormalChilds = [NSMutableArray arrayWithCapacity:0];
        NSMutableArray* hBRChilds = [NSMutableArray arrayWithCapacity:0];
        BOOL hasExpand = NO;
        for(VTDOMElement * element in [self childs])
        {
            UIEdgeInsets margin = [element margin];
            CGSize subinsets = CGSizeMake(insetSize.width - margin.left - margin.right, insetSize.height - margin.top - margin.bottom);
            FakedSize fakedSize;
            fakedSize.fakedX = YES;
            fakedSize.fakedY = YES;
            CGSize childSize = [element calculateLayoutSize:subinsets fakedSize:fakedSize maxValue:nil];
            if (childSize.width==MAXFLOAT) {
                [wexpanedChilds addObject:element];
                hasExpand = YES;
            }
            else
            {
                [wnormalChilds addObject:element];
            }
            if (childSize.height==MAXFLOAT) {
                [hexpanedChilds addObject:element];
                hasExpand = YES;
            }
            else
            {
                [hnormalChilds addObject:element];
            }
            if([element isKindOfClass:[VTDOMBRElement class]])
            {
                [hBRChilds addObject:element];
            }
        }
        CGSize totalWidthHeight = CGSizeZero;
        CGFloat expandWidthPerElement = 0;
        CGFloat expandHeightPerElementExtra = 0;
        
        for(VTDOMElement * element in wnormalChilds){
            
            UIEdgeInsets margin = [element margin];
            FakedSize newFakeSize;
            newFakeSize.fakedX = NO;
            newFakeSize.fakedY = YES;
            CGSize subinsets = CGSizeMake(insetSize.width - margin.left - margin.right, insetSize.height - margin.top - margin.bottom);
            CGSize childSize = [element calculateLayoutSize:subinsets fakedSize:newFakeSize maxValue:nil];
            totalWidthHeight.width += childSize.width + margin.left - margin.right;
        }
        CGFloat totalMarginWidth = 0;
        for(VTDOMElement * element in wexpanedChilds){
            
            UIEdgeInsets margin = [element margin];
            
            totalMarginWidth += margin.left + margin.right;
        }
        CGFloat expandWidth = insetSize.width - totalWidthHeight.width;
        
        if (expandWidth>0&&[wexpanedChilds count]>0) {
            expandWidth = expandWidth - totalMarginWidth;
            expandWidthPerElement = expandWidth/[wexpanedChilds count];
        }
        
        expandHeightPerElementExtra = 0;
        for(VTDOMElement * element in hBRChilds){
            
            UIEdgeInsets margin = [element margin];
            FakedSize newFakeSize;
            newFakeSize.fakedX = YES;
            newFakeSize.fakedY = NO;
            CGSize subinsets = CGSizeMake(insetSize.width - margin.left - margin.right, insetSize.height - margin.top - margin.bottom);
            CGSize childSize = [element calculateLayoutSize:subinsets fakedSize:newFakeSize maxValue:nil];
            expandHeightPerElementExtra += childSize.height + margin.left + margin.right;
        }
        
        
        for(VTDOMElement * element in [self childs]){
            
            UIEdgeInsets margin = [element margin];
            CGRect childRect = CGRectZero;
            if ([wexpanedChilds containsObject:element]&&[hexpanedChilds containsObject:element]) {
                childRect.size = [element layout:CGSizeMake(expandWidthPerElement
                                           , insetSize.height - margin.top - margin.bottom - expandHeightPerElementExtra)];
            }
            else if([wexpanedChilds containsObject:element])
            {
                childRect.size = [element layout:CGSizeMake(expandWidthPerElement
                                                            , insetSize.height - margin.top - margin.bottom)];
            }
            else if([hexpanedChilds containsObject:element])
            {
                childRect.size = [element layout:CGSizeMake(insetSize.width - margin.left - margin.right
                                                            , insetSize.height - margin.top - margin.bottom - expandHeightPerElementExtra)];
            }
            else
            {
                childRect.size = [element layout:CGSizeMake(insetSize.width - margin.left - margin.right
                                                            , insetSize.height - margin.top - margin.bottom )];
            }
            childRect.origin = CGPointZero;
            
            if(![element isKindOfClass:[VTDOMBRElement class]]
               &&  ( p.x + childRect.size.width + margin.left + margin.right <= maxWidth - padding.right)){
                
                childRect.origin = CGPointMake(p.x + margin.left, p.y + margin.top);
                p.x += childRect.size.width + margin.left + margin.right;
                
                if(lineHeight < childRect.size.height + margin.top + margin.bottom){
                    lineHeight = childRect.size.height + margin.top + margin.bottom;
                }
                if(width < p.x + padding.right){
                    width = p.x + padding.right;
                }
            }
            else {
                p.x = padding.left;
                p.y += lineHeight;
                lineHeight = childRect.size.height + margin.top + margin.bottom;
                childRect.origin = CGPointMake(p.x + margin.left, p.y + margin.top);
                p.x += childRect.size.width + margin.left + margin.right;
                if(width < p.x + padding.right){
                    width = p.x + padding.right;
                }
                
            }
            [element setFrame:childRect];
        }
        
        size = CGSizeMake(width, p.y + lineHeight + padding.bottom);
        
    }
    else{
        
        for(VTDOMElement * element in [self childs]){
            
            [element layout:insetSize];
            
            CGRect r = [element frame];
            
            float left = [element floatValueForKey:@"left" of:insetSize.width defaultValue:0];
            float right = [element floatValueForKey:@"right" of:insetSize.width defaultValue:0];
            float top = [element floatValueForKey:@"top" of:insetSize.height defaultValue:0];
            float bottom = [element floatValueForKey:@"bottom" of:insetSize.height defaultValue:0];
            
            if(left == MAXFLOAT){
                if(right == MAXFLOAT){
                    r.origin.x = (frame.size.width - r.size.width) / 2.0;
                }
                else {
                    r.origin.x = (frame.size.width - r.size.width - padding.right - right);
                }
            }
            else {
                r.origin.x = left + padding.left;
            }
            
            if(top == MAXFLOAT){
                if(bottom == MAXFLOAT){
                    r.origin.y = (frame.size.height - r.size.height) / 2.0;
                }
                else {
                    r.origin.y = (frame.size.height - r.size.height - padding.bottom - bottom);
                }
            }
            else {
                r.origin.y = top + padding.top;
            }
            
            [element setFrame:r];
            
            if(r.origin.x + r.size.width + padding.right > size.width){
                size.width = r.origin.x +r.size.width +padding.right;
            }
            
            if(r.origin.y + r.size.height + padding.bottom > size.height){
                size.height = r.origin.y +r.size.height +padding.bottom;
            }
        }
    }
    
    [self setContentSize:size];
    
    return size;
}

-(CGSize) layout:(CGSize) size{

    UIEdgeInsets insets = [self padding];
    
    CGRect frame = [self frame];
    
    frame.size.width = [self floatValueForKey:@"width" of:size.width defaultValue:0];
    frame.size.height = [self floatValueForKey:@"height" of:size.height defaultValue:0];
    
    CGRect oldRect = frame;
    if (frame.size.width == MAXFLOAT) {
        frame.size.width = size.width;
    }
    if (frame.size.height == MAXFLOAT) {
        frame.size.height = size.height;
    }
    [self setFrame:frame];
    frame = oldRect;
    
    if(frame.size.width == MAXFLOAT || frame.size.height == MAXFLOAT){
        NSString* widthString = [self stringValueForKey:@"width"];
        NSString* heightString = [self stringValueForKey:@"height"];
        
        CGSize contentSize = [self layoutChildren:insets];
        
        if (![widthString isEqualToString:@"expand"]) {
            if(frame.size.width == MAXFLOAT){
                frame.size.width = contentSize.width;
                float v = [self floatValueForKey:@"max-width" of:size.width defaultValue:frame.size.width];
                if(frame.size.width > v){
                    frame.size.width = v;
                }
                v = [self floatValueForKey:@"min-width" of:size.width defaultValue:frame.size.width];
                if(frame.size.width < v){
                    frame.size.width = v;
                }
            }
        }
        else
        {
            frame.size.width = size.width;
        }
        
        if (![heightString isEqualToString:@"expand"]) {
            if(frame.size.height == MAXFLOAT){
                frame.size.height = contentSize.height;
                float v = [self floatValueForKey:@"max-height" of:size.height defaultValue:frame.size.height];
                if(frame.size.height > v){
                    frame.size.height = v;
                }
                v = [self floatValueForKey:@"min-height" of:size.height defaultValue:frame.size.height];
                if(frame.size.height < v){
                    frame.size.height = v;
                }
            }
        }
        else
        {
            frame.size.height = size.height;
        }
        
        [self setFrame:frame];
        
        return frame.size;
    }
    else{
        CGSize size = [self layoutChildren:insets];
        size = frame.size;
        return size;
    }
    
}

-(CGSize) calculateLayoutSize:(CGSize) size fakedSize:(FakedSize)fakedSize maxValue:(NSMutableDictionary*)maxValue
{
    if (!maxValue) {
        maxValue = [NSMutableDictionary dictionaryWithCapacity:0];
    }
    UIEdgeInsets insets = [self padding];
    
    CGRect frame = CGRectZero;
    
    frame.size.width = [self floatValueForKey:@"width" of:size.width defaultValue:0];
    frame.size.height = [self floatValueForKey:@"height" of:size.height defaultValue:0];
    
    if(frame.size.width == MAXFLOAT||frame.size.height == MAXFLOAT){
        NSString* widthString = [self stringValueForKey:@"width"];
        NSString* heightString = [self stringValueForKey:@"height"];
        CGSize contentSize = CGSizeZero;
        if ([widthString isEqualToString:@"auto"]) {
            fakedSize.fakedX = YES;
        }
        if ([heightString isEqualToString:@"auto"]) {
            fakedSize.fakedY = YES;
        }
        
        if ([widthString isEqualToString:@"expand"]&&[heightString isEqualToString:@"expand"]) {
            contentSize = CGSizeMake(MAXFLOAT, MAXFLOAT);
        }
        else
        {
            float h = [self floatValueForKey:@"max-height" of:size.height defaultValue:frame.size.width];
            float w = [self floatValueForKey:@"max-width" of:size.width defaultValue:frame.size.width];
            h = h==MAXFLOAT?size.height:h;
            w = w==MAXFLOAT?size.width:w;
            
            contentSize = [self calculateLayoutChildrenInsets:insets parentSize:CGSizeMake(w, h) fakedSize:fakedSize maxValue:maxValue];
            if ([widthString isEqualToString:@"expand"]) {
                contentSize.width = MAXFLOAT;
            }
            else if ([heightString isEqualToString:@"expand"]) {
                contentSize.height = MAXFLOAT;
            }
        }
        
        if(frame.size.width == MAXFLOAT&&![widthString isEqualToString:@"expand"]&&contentSize.width!=MAXFLOAT){
            
            if ([widthString isEqualToString:@"auto"]) {
                frame.size.width = contentSize.width;
            }
            
            float v = [self floatValueForKey:@"max-width" of:size.width defaultValue:frame.size.width];
            if(frame.size.width > v){
                frame.size.width = v;
            }
            v = [self floatValueForKey:@"min-width" of:size.width defaultValue:frame.size.width];
            if(frame.size.width < v){
                frame.size.width = v;
            }
        }
        else if([widthString isEqualToString:@"expand"])
        {
            frame.size.width = contentSize.width;
            
        }
        if(frame.size.height == MAXFLOAT&&![heightString isEqualToString:@"expand"]&&contentSize.height!=MAXFLOAT){
            
            if ([heightString isEqualToString:@"auto"]) {
                frame.size.height = contentSize.height;
            }
            
            float v = [self floatValueForKey:@"max-height" of:size.height defaultValue:frame.size.height];
            if(frame.size.height > v){
                frame.size.height = v;
            }
            v = [self floatValueForKey:@"min-height" of:size.height defaultValue:frame.size.height];
            if(frame.size.height < v){
                frame.size.height = v;
            }
        }
        else if([heightString isEqualToString:@"expand"])
        {
            frame.size.height = contentSize.height;
            
        }
        return frame.size;
    }
    else{
        CGSize size = frame.size;
        //CGSize size = [self calculateLayoutChildrenInsets:insets parentSize:frame.size fakedSize:fakedSize maxValue:maxValue];
        return size;
    }
}

-(CGSize) calculateLayoutChildrenInsets:(UIEdgeInsets) padding parentSize:(CGSize)parentSize fakedSize:(FakedSize)fakedSize maxValue:(NSMutableDictionary*)maxValue
{

    CGSize size = CGSizeZero;
    size = parentSize;
    CGSize inset = CGSizeMake(parentSize.width - padding.left - padding.right, parentSize.height - padding.top - padding.bottom) ;
    
    
    NSString * layout = [self stringValueForKey:@"layout"];
    
    if([layout isEqualToString:@"flow"]){
        
        CGPoint p = CGPointMake(padding.left, padding.top);
        CGFloat lineHeight = 0;
        CGFloat width = padding.left + padding.right;
        CGFloat maxWidth = parentSize.width;
        
        if(maxWidth == MAXFLOAT){
            NSString * max = [self attributeValueForKey:@"max-width"];
            if(max){
                maxWidth = [max floatValue];
            }
        }
        
        NSMutableArray* wexpanedChilds = [NSMutableArray arrayWithCapacity:0];
        NSMutableArray* wnormalChilds = [NSMutableArray arrayWithCapacity:0];
        NSMutableArray* hexpanedChilds = [NSMutableArray arrayWithCapacity:0];
        NSMutableArray* hnormalChilds = [NSMutableArray arrayWithCapacity:0];
        NSMutableArray* hBRChilds = [NSMutableArray arrayWithCapacity:0];
        BOOL hasExpand = NO;
        for(VTDOMElement * element in [self childs])
        {
            UIEdgeInsets margin = [element margin];
            CGSize subinsets = CGSizeMake(inset.width - margin.left - margin.right, inset.height - margin.top - margin.bottom);
            fakedSize.fakedX = YES;
            fakedSize.fakedY = YES;
            CGSize childSize = [element calculateLayoutSize:subinsets fakedSize:fakedSize maxValue:maxValue];
            if (childSize.width==MAXFLOAT) {
                [wexpanedChilds addObject:element];
                hasExpand = YES;
            }
            else
            {
                [wnormalChilds addObject:element];
            }
            if (childSize.height==MAXFLOAT) {
                [hexpanedChilds addObject:element];
                hasExpand = YES;
            }
            else
            {
                [hnormalChilds addObject:element];
            }
            if([element isKindOfClass:[VTDOMBRElement class]])
            {
                [hBRChilds addObject:element];
            }
        }
        if (fakedSize.fakedX&&[wexpanedChilds count]&&fakedSize.fakedY&&[hexpanedChilds count]) {
            size.width = size.height = MAXFLOAT;
        }
        else
        {
            CGSize totalWidthHeight = CGSizeZero;
            CGFloat expandWidthPerElement = 0;
            CGFloat expandHeightPerElementExtra = 0;
            
            for(VTDOMElement * element in wnormalChilds){
                
                UIEdgeInsets margin = [element margin];
                FakedSize newFakeSize;
                newFakeSize.fakedX = NO;
                newFakeSize.fakedY = fakedSize.fakedY;
                CGSize subinsets = CGSizeMake(inset.width - margin.left - margin.right, inset.height - margin.top - margin.bottom);
                CGSize childSize = [element calculateLayoutSize:subinsets fakedSize:newFakeSize maxValue:maxValue];
                totalWidthHeight.width += childSize.width + margin.left - margin.right;
            }
            CGFloat totalMarginWidth = 0;
            for(VTDOMElement * element in wexpanedChilds){
                
                UIEdgeInsets margin = [element margin];
                
                totalMarginWidth += margin.left + margin.right;
            }
            CGFloat expandWidth = inset.width - totalWidthHeight.width;
            
            if (expandWidth>0&&[wexpanedChilds count]>0) {
                expandWidth = expandWidth - totalMarginWidth;
                expandWidthPerElement = expandWidth/[wexpanedChilds count];
            }
            
            expandHeightPerElementExtra = 0;
            for(VTDOMElement * element in hBRChilds){
                
                UIEdgeInsets margin = [element margin];
                FakedSize newFakeSize;
                newFakeSize.fakedX = fakedSize.fakedX;
                newFakeSize.fakedY = NO;
                CGSize subinsets = CGSizeMake(inset.width - margin.left - margin.right, inset.height - margin.top - margin.bottom);
                CGSize childSize = [element calculateLayoutSize:subinsets fakedSize:newFakeSize maxValue:maxValue];
                expandHeightPerElementExtra += childSize.height + margin.left + margin.right;
            }
            
            
            for(VTDOMElement * element in [self childs]){
                
                UIEdgeInsets margin = [element margin];
                CGSize childWidthHeight = CGSizeZero;
                FakedSize newFakeSize;
                newFakeSize.fakedX = NO;
                newFakeSize.fakedY = NO;
                if ([wexpanedChilds containsObject:element]&&[hexpanedChilds containsObject:element]) {
                    CGSize subinsets = CGSizeMake(expandWidthPerElement, inset.height - margin.top - margin.bottom - expandHeightPerElementExtra);
                    childWidthHeight = [element calculateLayoutSize:subinsets fakedSize:newFakeSize maxValue:maxValue];
                }
                else if([wexpanedChilds containsObject:element])
                {
                    CGSize subinsets = CGSizeMake(expandWidthPerElement, inset.height - margin.top - margin.bottom);
                    childWidthHeight = [element calculateLayoutSize:subinsets fakedSize:newFakeSize maxValue:maxValue];
                }
                else if([hexpanedChilds containsObject:element])
                {
                    CGSize subinsets = CGSizeMake(inset.width - margin.left - margin.right, inset.height - margin.top - margin.bottom - expandHeightPerElementExtra);
                    childWidthHeight = [element calculateLayoutSize:subinsets fakedSize:newFakeSize maxValue:maxValue];
                }
                else
                {
                    CGSize subinsets = CGSizeMake(inset.width - margin.left - margin.right, inset.height - margin.top - margin.bottom);
                    childWidthHeight = [element calculateLayoutSize:subinsets fakedSize:newFakeSize maxValue:maxValue];
                }
                CGRect childRect = CGRectZero;
                childRect.size = childWidthHeight;
                
                if(![element isKindOfClass:[VTDOMBRElement class]]
                   &&  ( p.x + childRect.size.width + margin.left + margin.right <= maxWidth - padding.right)){
                    
                    childRect.origin = CGPointMake(p.x + margin.left, p.y + margin.top);
                    p.x += childRect.size.width + margin.left + margin.right;
                    
                    if(lineHeight < childRect.size.height + margin.top + margin.bottom){
                        lineHeight = childRect.size.height + margin.top + margin.bottom;
                    }
                    if(width < p.x + padding.right){
                        width = p.x + padding.right;
                    }
                }
                else {
                    p.x = padding.left;
                    p.y += lineHeight;
                    lineHeight = childRect.size.height + margin.top + margin.bottom;
                    childRect.origin = CGPointMake(p.x + margin.left, p.y + margin.top);
                    p.x += childRect.size.width + margin.left + margin.right;
                    if(width < p.x + padding.right){
                        width = p.x + padding.right;
                    }
                }
                
            }
            
            
            size = CGSizeMake(width, p.y + lineHeight + padding.bottom);
            if (fakedSize.fakedY&&[hexpanedChilds count]) {
                size.height = MAXFLOAT;
                
            }
            if (fakedSize.fakedX&&[wexpanedChilds count]) {
                size.width = MAXFLOAT;
            }
        }
    }
    else{
        
        for(VTDOMElement * element in [self childs]){
            
            CGSize childSize = [element calculateLayoutSize:inset fakedSize:fakedSize maxValue:maxValue];
            CGRect childRect = CGRectZero;
            childRect.size = childSize;
            
            if (childRect.size.width==MAXFLOAT) {
                size.width = MAXFLOAT;
            }
            if (childRect.size.height==MAXFLOAT) {
                size.height = MAXFLOAT;
            }
            
            float left = [element floatValueForKey:@"left" of:inset.width defaultValue:0];
            float right = [element floatValueForKey:@"right" of:inset.width defaultValue:0];
            float top = [element floatValueForKey:@"top" of:inset.height defaultValue:0];
            float bottom = [element floatValueForKey:@"bottom" of:inset.height defaultValue:0];
            
            if (childRect.size.width!=MAXFLOAT) {
                if(left == MAXFLOAT){
                    if(right == MAXFLOAT){
                        childRect.origin.x = (parentSize.width - childRect.size.width) / 2.0;
                    }
                    else {
                        childRect.origin.x = (parentSize.width - childRect.size.width - padding.right - right);
                    }
                }
                else {
                    childRect.origin.x = left + padding.left;
                }
            }
            
            if (childRect.size.height!=MAXFLOAT)
            {
                if(top == MAXFLOAT){
                    if(bottom == MAXFLOAT){
                        childRect.origin.y = (parentSize.height - childRect.size.height) / 2.0;
                    }
                    else {
                        childRect.origin.y = (parentSize.height - childRect.size.height - padding.bottom - bottom);
                    }
                }
                else {
                    childRect.origin.y = top + padding.top;
                }
            }
            
            
            if (childRect.size.width!=MAXFLOAT) {
                if(childRect.origin.x + childRect.size.width + padding.right > size.width){
                    size.width = childRect.origin.x +childRect.size.width +padding.right;
                }
            }
            else
            {
                size.width = MAXFLOAT;
            }
            
            if (childRect.size.height!=MAXFLOAT)
            {
                if(childRect.origin.y + childRect.size.height + padding.bottom > size.height){
                    size.height = childRect.origin.y +childRect.size.height +padding.bottom;
                }
            }
            else
            {
                size.height = MAXFLOAT;
            }
            
            
        }
    }
    
    return size;
}

-(CGSize) layout{
    return [self layoutChildren:self.padding];
}

-(CGSize) layoutChildren1:(UIEdgeInsets) padding{
    
    CGRect frame = [self frame];
    CGSize size = CGSizeZero;
    CGSize insetSize = CGSizeMake(frame.size.width - padding.left - padding.right
                                  , frame.size.height - padding.top - padding.bottom);
    
    NSString * layout = [self stringValueForKey:@"layout"];
    
    if([layout isEqualToString:@"flow"]){
        
        CGPoint p = CGPointMake(padding.left, padding.top);
        CGFloat lineHeight = 0;
        CGFloat width = padding.left + padding.right;
        CGFloat maxWidth = frame.size.width;
        
        if(maxWidth == MAXFLOAT){
            NSString * max = [self attributeValueForKey:@"max-width"];
            if(max){
                maxWidth = [max floatValue];
            }
        }
        
        for(VTDOMElement * element in [self childs]){
            
            UIEdgeInsets margin = [element margin];
            
            [element layout:CGSizeMake(insetSize.width - margin.left - margin.right
                                       , insetSize.height - margin.top - margin.bottom)];
            
            CGRect r = [element frame];
            
            if(![element isKindOfClass:[VTDOMBRElement class]]
               &&  ( p.x + r.size.width + margin.left + margin.right <= maxWidth - padding.right)){
                
                r.origin = CGPointMake(p.x + margin.left, p.y + margin.top);
                p.x += r.size.width + margin.left + margin.right;
                
                if(lineHeight < r.size.height + margin.top + margin.bottom){
                    lineHeight = r.size.height + margin.top + margin.bottom;
                }
                if(width < p.x + padding.right){
                    width = p.x + padding.right;
                }
            }
            else {
                p.x = padding.left;
                p.y += lineHeight;
                lineHeight = r.size.height + margin.top + margin.bottom;
                r.origin = CGPointMake(p.x + margin.left, p.y + margin.top);
                p.x += r.size.width + margin.left + margin.right;
                if(width < p.x + padding.right){
                    width = p.x + padding.right;
                }
            }
            
            [element setFrame:r];
        }
        
        size = CGSizeMake(width, p.y + lineHeight + padding.bottom);
        
    }
    else{
        
        for(VTDOMElement * element in [self childs]){
            
            [element layout:insetSize];
            
            CGRect r = [element frame];
            
            float left = [element floatValueForKey:@"left" of:insetSize.width defaultValue:0];
            float right = [element floatValueForKey:@"right" of:insetSize.width defaultValue:0];
            float top = [element floatValueForKey:@"top" of:insetSize.height defaultValue:0];
            float bottom = [element floatValueForKey:@"bottom" of:insetSize.height defaultValue:0];
            
            if(left == MAXFLOAT){
                if(right == MAXFLOAT){
                    r.origin.x = (frame.size.width - r.size.width) / 2.0;
                }
                else {
                    r.origin.x = (frame.size.width - r.size.width - padding.right - right);
                }
            }
            else {
                r.origin.x = left + padding.left;
            }
            
            if(top == MAXFLOAT){
                if(bottom == MAXFLOAT){
                    r.origin.y = (frame.size.height - r.size.height) / 2.0;
                }
                else {
                    r.origin.y = (frame.size.height - r.size.height - padding.bottom - bottom);
                }
            }
            else {
                r.origin.y = top + padding.top;
            }
            
            [element setFrame:r];
            
            if(r.origin.x + r.size.width + padding.right > size.width){
                size.width = r.origin.x +r.size.width +padding.right;
            }
            
            if(r.origin.y + r.size.height + padding.bottom > size.height){
                size.height = r.origin.y +r.size.height +padding.bottom;
            }
        }
    }
    
    [self setContentSize:size];
    
    return size;
}

-(CGSize) layout1:(CGSize) size{
    
    UIEdgeInsets insets = [self padding];
    
    CGRect frame = CGRectZero;
    
    frame.size.width = [self floatValueForKey:@"width" of:size.width defaultValue:0];
    frame.size.height = [self floatValueForKey:@"height" of:size.height defaultValue:0];
    
    [self setFrame:frame];
    
    if(frame.size.width == MAXFLOAT || frame.size.height == MAXFLOAT){
        
        CGSize contentSize = [self layoutChildren:insets];
        
        if(frame.size.width == MAXFLOAT){
            frame.size.width = contentSize.width;
            float v = [self floatValueForKey:@"max-width" of:size.width defaultValue:frame.size.width];
            if(frame.size.width > v){
                frame.size.width = v;
            }
            v = [self floatValueForKey:@"min-width" of:size.width defaultValue:frame.size.width];
            if(frame.size.width < v){
                frame.size.width = v;
            }
        }
        
        if(frame.size.height == MAXFLOAT){
            frame.size.height = contentSize.height;
            float v = [self floatValueForKey:@"max-height" of:size.height defaultValue:frame.size.height];
            if(frame.size.height > v){
                frame.size.height = v;
            }
            v = [self floatValueForKey:@"min-height" of:size.height defaultValue:frame.size.height];
            if(frame.size.height < v){
                frame.size.height = v;
            }
        }
        
        [self setFrame:frame];
        
        return contentSize;
    }
    else{
        return [self layoutChildren:insets];
    }
    
}


@end
