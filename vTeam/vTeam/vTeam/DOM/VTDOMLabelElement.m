//
//  VTDOMLabelElement.m
//  vTeam
//
//  Created by zhang hailong on 13-8-14.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#import "VTDOMLabelElement.h"

#import "VTDOMElement+Render.h"
#import "VTDOMElement+Style.h"
#import "VTDOMElement+Layout.h"
#import "VTDOMDocument.h"
#import "NSString+VTDOMStringSize.h"

@interface VTDOMLabelElement(){
    CGFloat _actualFontSize;
}

@end

@implementation VTDOMLabelElement

-(UIFont *) font{    
    return [self elementFont:[UIFont systemFontOfSize:14]];
}

-(CGFloat) minFontSize{
    return [self floatValueForKey:@"min-font-size"];
}

-(UIColor *) textColor{
    
    UIColor * color = [self colorValueForKey:@"color"];
    
    if(color == nil){
        color = [UIColor blackColor];
    }
    
    return color;
}

-(NSLineBreakMode) lineBreakMode{
    
    NSLineBreakMode breakMode = NSLineBreakByCharWrapping;
    
    NSString * mode = [self stringValueForKey:@"break-mode"];
    
    if([mode isEqualToString:@"wrap"]){
        breakMode = NSLineBreakByWordWrapping;
    }
    else if([mode isEqualToString:@"clip"]){
        breakMode = NSLineBreakByClipping;
    }
    else if([mode isEqualToString:@"head"]){
        breakMode = NSLineBreakByTruncatingHead;
    }
    else if([mode isEqualToString:@"tail"]){
        breakMode = NSLineBreakByTruncatingTail;
    }
    else if([mode isEqualToString:@"middle"]){
        breakMode = NSLineBreakByTruncatingMiddle;
    }
    else{
        breakMode = NSLineBreakByCharWrapping;
    }

    return breakMode;
}

-(NSTextAlignment) alignment{
    
    NSTextAlignment alignment = NSTextAlignmentLeft;
    
    NSString * align = [self stringValueForKey:@"align"];
    
    if([align isEqualToString:@"center"]){
        alignment = NSTextAlignmentCenter;
    }
    else if([align isEqualToString:@"right"]){
        alignment = NSTextAlignmentRight;
    }

    return alignment;
}

-(CGSize) layoutChildren:(UIEdgeInsets)padding{
    CGRect r = [self frame];
    NSString* widthString = [self stringValueForKey:@"width"];
    NSString* heightString = [self stringValueForKey:@"height"];
    if([widthString isEqualToString:@"auto"] || [heightString isEqualToString:@"auto"]){

        NSString * text = [self text];
        
        if(text){
            
            UIEdgeInsets padding = [self padding];
            UIFont * font = [self font];
            CGFloat minFontSize = [self minFontSize];
            
            CGFloat width = r.size.width;
            CGFloat height = r.size.height;
            
            if([widthString isEqualToString:@"auto"]){
                NSString * max = [self stringValueForKey:@"max-width"];
                if(max){
                    width = [max floatValue];
                }
            }
            
            if([heightString isEqualToString:@"auto"]){
                NSString * max = [self stringValueForKey:@"max-height"];
                if(max){
                    height = [max floatValue];
                }
            }
            
            CGSize s = CGSizeZero;
            
            if(minFontSize ==0){
                _actualFontSize = 0;
                s = [text VTSizeWithFont:font constrainedToSize:CGSizeMake(width, height) lineBreakMode:self.lineBreakMode];
            }
            else{
                s = [text VTSizeWithFont:font minFontSize:minFontSize actualFontSize:&_actualFontSize forWidth:width lineBreakMode:NSLineBreakByClipping];
//                if ([[UIDevice currentDevice].systemVersion floatValue] >= 9.0 && (s.width + 2) < width) {
//                    s.width += 2;
//                }
                if(_actualFontSize != font.pointSize){
                    font = [UIFont fontWithName:font.fontName size:_actualFontSize];
                    s = [text VTSizeWithFont:font constrainedToSize:CGSizeMake(width, height) lineBreakMode:self.lineBreakMode];
                }
            }
      
            if([widthString isEqualToString:@"auto"]){
                r.size.width = (int) (s.width + 0.99999f) + padding.left + padding.right;
                NSString * min = [self stringValueForKey:@"min-width"];
                if(min){
                    if(r.size.width < [min floatValue]){
                        r.size.width = [min floatValue];
                    }
                }
            }
            
            if([heightString isEqualToString:@"auto"]){
                r.size.height = (int) (s.height + 0.99999f) + padding.top + padding.bottom;
                NSString * min = [self stringValueForKey:@"min-height"];
                if(min){
                    if(r.size.height < [min floatValue]){
                        r.size.height = [min floatValue];
                    }
                }
                NSString * max = [self stringValueForKey:@"max-height"];
                if(max){
                    if(r.size.height > [max floatValue]){
                        r.size.height = [max floatValue];
                    }
                }
            }
        }
        else{
            if([widthString isEqualToString:@"auto"]){
                r.size.width = 0;
            }
            
            if([heightString isEqualToString:@"auto"]){
                r.size.height = 0;
            }
        }
        
        [self setFrame:r];

    }
    return r.size;
}

-(CGSize) calculateLayoutChildrenInsets:(UIEdgeInsets) padding parentSize:(CGSize)parentSize fakedSize:(FakedSize)fakedSize maxValue:(NSMutableDictionary*)maxValue
{
    CGRect r = CGRectZero;
    r.size = parentSize;
    NSString* widthString = [self stringValueForKey:@"width"];
    NSString* heightString = [self stringValueForKey:@"height"];
    if([widthString isEqualToString:@"auto"] || [heightString isEqualToString:@"auto"]){
        
        NSString * text = [self text];
        
        if(text){
            
            UIEdgeInsets padding = [self padding];
            UIFont * font = [self font];
            CGFloat minFontSize = [self minFontSize];
            
            CGFloat width = r.size.width;
            CGFloat height = r.size.height;
            
            if([widthString isEqualToString:@"auto"]){
                NSString * max = [self stringValueForKey:@"max-width"];
                if(max){
                    width = [max floatValue];
                }
            }
            
            if([heightString isEqualToString:@"auto"]){
                NSString * max = [self stringValueForKey:@"max-height"];
                if(max){
                    height = [max floatValue];
                }
            }
            
            CGSize s = CGSizeZero;
            
            if(minFontSize ==0){
                _actualFontSize = 0;
                s = [text VTSizeWithFont:font constrainedToSize:CGSizeMake(width, height) lineBreakMode:self.lineBreakMode];
            }
            else{
                s = [text VTSizeWithFont:font minFontSize:minFontSize actualFontSize:&_actualFontSize forWidth:width lineBreakMode:NSLineBreakByClipping];
                if(_actualFontSize != font.pointSize){
                    font = [UIFont fontWithName:font.fontName size:_actualFontSize];
                    s = [text VTSizeWithFont:font constrainedToSize:CGSizeMake(width, height) lineBreakMode:self.lineBreakMode];
                }
            }
            
            if([widthString isEqualToString:@"auto"]){
                r.size.width = (int) (s.width + 0.99999f) + padding.left + padding.right;
                NSString * min = [self stringValueForKey:@"min-width"];
                if(min){
                    if(r.size.width < [min floatValue]){
                        r.size.width = [min floatValue];
                    }
                }
            }
            
            if([heightString isEqualToString:@"auto"]){
                r.size.height = (int) (s.height + 0.99999f) + padding.top + padding.bottom;
                NSString * min = [self stringValueForKey:@"min-height"];
                if(min){
                    if(r.size.height < [min floatValue]){
                        r.size.height = [min floatValue];
                    }
                }
                NSString * max = [self stringValueForKey:@"max-height"];
                if(max){
                    if(r.size.height > [max floatValue]){
                        r.size.height = [max floatValue];
                    }
                }
            }
        }
        else{
            if([widthString isEqualToString:@"auto"]){
                r.size.width = 0;
            }
            
            if([heightString isEqualToString:@"auto"]){
                r.size.height = 0;
            }
        }
        
    }
    return r.size;
}

-(void) draw:(CGRect) rect context:(CGContextRef) context{
    [super draw:rect context:context];

    NSString * text = [self text];
    
    if(text){
        
        UIEdgeInsets padding = [self padding];
        
        UIFont * font = [self font];
        
        CGFloat minFontSize = [self minFontSize];
        
        if(minFontSize && _actualFontSize){
            font = [UIFont fontWithName:font.fontName size:_actualFontSize];
        }
        
        UIColor * color = [self textColor];
    
        NSLineBreakMode breakMode = [self lineBreakMode];
        
        NSTextAlignment alignment = [self alignment];
        
        CGContextSetFillColorWithColor(context, [color CGColor]);
    
        CGContextSetStrokeColorWithColor(context, [color CGColor]);

        UIGraphicsPushContext(context);

        CGSize size = self.frame.size;
        
        CGRect rect = CGRectMake(padding.left, padding.top
                                 , size.width - padding.left - padding.right
                                 , size.height - padding.top - padding.bottom);
        [text VTDrawInRect:rect
                  withFont:font lineBreakMode:breakMode alignment:alignment color:color];
        
        UIGraphicsPopContext();
    }

}


@end
