//
//  VTDomStyleController.m
//  vTeam
//
//  Created by shieh fabo on 14-10-8.
//  Copyright (c) 2014年 hailong.org. All rights reserved.
//

#import "VTDomStyleController.h"
#import "VTStyleController.h"
#import "VTDOMStyleSheet.h"

@implementation VTDomStyleController

@synthesize styles = _styles;

-(void) dealloc{
    [_styles release];
    [super dealloc];
}

-(id)initWithCSSName:(NSString*)cssName bundle:(NSBundle*)bundle;
{
    self = [super init];
    if (self) {
        NSString* cssContent = [NSString stringWithContentsOfFile:[bundle pathForResource:cssName ofType:@"css"] encoding:NSUTF8StringEncoding error:nil];
        NSMutableArray* styleArray = [[NSMutableArray alloc] init];
        _styles = styleArray;
        if (cssContent) {
            [styleArray addObject:cssContent];
        }
        
    }
    return self;
}

-(id)addObjectFromDomStyleController:(VTDomStyleController*)controller
{
    if (controller) {
        NSMutableArray* oldStyles = (NSMutableArray*)self.styles;
        if (controller.styles) {
            NSArray* newStyles = controller.styles;
            [oldStyles addObjectsFromArray:newStyles];
            
        }
    }
    return self;
}

-(VTStyleController*)styleController
{
    VTStyleController* ret = nil;
    if (self.styles&&[self.styles count]) {
        VTDOMStyleSheet* newSheet = [[[VTDOMStyleSheet alloc] init] autorelease];
        [newSheet setStyleController:self];
        NSArray* realStyleArray = newSheet.styles;
        if (realStyleArray&&[realStyleArray count]) {
            VTStyleController* newController = [[[VTStyleController alloc] init] autorelease];
            NSMutableArray* newStyleArray = [NSMutableArray arrayWithCapacity:0];
            newController.styles = newStyleArray;
            
            for (int i=0; i<[realStyleArray count]; i++) {
                VTDOMStyle* oneDomStyle = [realStyleArray objectAtIndex:i];
                NSArray* allKey = [oneDomStyle allKeys];
                for (int j=0; j<[allKey count]; j++) {
                    NSString* oneKey = [allKey objectAtIndex:j];
                    if ([oneKey hasPrefix:@"notdom-"]) {
                        NSString* tempKey = [oneKey substringFromIndex:7];
                        NSString* value = [oneDomStyle stringValueForKey:oneKey];
                        NSString* name = [oneDomStyle name];
                        VTStyle* newStyle = [[VTStyle alloc] init];
                        
                        if (value&&[value length]) {
                            UIColor* colorValue = [self colorWithHexString:value];
                            if (colorValue) {
                                newStyle.name = name;
                                newStyle.key = tempKey;
                                newStyle.value = colorValue;
                                [newStyleArray addObject:newStyle];
                            }
                            else if ([[value lowercaseString] hasPrefix:@"image#"]) {
                                NSString* tempValue = [value substringFromIndex:6];
                                newStyle.name = name;
                                newStyle.key = tempKey;
                                newStyle.imageValue = tempValue;
                                [newStyleArray addObject:newStyle];
                            }
                            else if ([[value lowercaseString] hasPrefix:@"font#"]) {
                                NSString* tempValue = [value substringFromIndex:5];
                                newStyle.name = name;
                                newStyle.key = tempKey;
                                newStyle.fontValue = tempValue;
                                [newStyleArray addObject:newStyle];
                            }
                            else if ([[value lowercaseString] hasPrefix:@"edge#"]) {
                                NSString* tempValue = [value substringFromIndex:5];
                                newStyle.name = name;
                                newStyle.key = tempKey;
                                newStyle.edgeValue = tempValue;
                                [newStyleArray addObject:newStyle];
                            }
                            else if ([[value lowercaseString] hasPrefix:@"<boolean>"]) {
                                NSString* tempValue = [value substringFromIndex:6];
                                NSNumber* realValue = nil;
                                if ([[tempValue lowercaseString] isEqualToString:@"yes"]||[[tempValue lowercaseString] isEqualToString:@"true"]) {
                                    realValue = [NSNumber numberWithBool:YES];
                                }
                                else if ([[tempValue lowercaseString] isEqualToString:@"no"]||[[tempValue lowercaseString] isEqualToString:@"false"]) {
                                    realValue = [NSNumber numberWithBool:NO];
                                }
                                newStyle.name = name;
                                newStyle.key = tempKey;
                                newStyle.value = realValue;
                                [newStyleArray addObject:newStyle];
                            }
                            else if ([[value lowercaseString] hasPrefix:@"<number>"]) {
                                NSString* tempValue = [value substringFromIndex:8];
                                NSNumber* realValue = nil;
                                NSNumberFormatter * f = [[NSNumberFormatter alloc] init];
                                [f setNumberStyle:NSNumberFormatterDecimalStyle];
                                realValue = [f numberFromString:tempValue];
                                [f release];
                                
                                
                                newStyle.name = name;
                                newStyle.key = tempKey;
                                newStyle.value = realValue;
                                [newStyleArray addObject:newStyle];
                            }
                            else
                            {
                                newStyle.name = name;
                                newStyle.key = tempKey;
                                newStyle.value = value;
                                [newStyleArray addObject:newStyle];
                            }
                            
                        }
                        [newStyle release];
                    }
                }
            }
            
            if ([newController.styles count]) {
                ret = newController;
            }
        }
    }
    
    return ret;
    
    
}


- (UIColor *) colorWithHexString: (NSString *) stringToConvert
{
    NSString *cString = [[stringToConvert stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]] uppercaseString];//字符串处理
    //例子，stringToConvert #ffffff
    if ([cString length] < 6)
        return nil;//如果非十六进制，返回nil
    if ([cString hasPrefix:@"#"])
        cString = [cString substringFromIndex:1];//去掉头
    if ([cString length] != 6&&[cString length] != 8)//去头非十六进制，返回nil
        return nil;
    //分别取RGB的值
    NSRange range;
    range.location = 0;
    range.length = 2;
    NSString *rString = [cString substringWithRange:range];
    
    range.location = 2;
    NSString *gString = [cString substringWithRange:range];
    
    range.location = 4;
    NSString *bString = [cString substringWithRange:range];
    
    NSString *aString = @"FF";
    if (cString.length==8) {
        range.location = 6;
        aString = [cString substringWithRange:range];
    }
    
    unsigned int r, g, b ,a;
    //NSScanner把扫描出的制定的字符串转换成Int类型
    [[NSScanner scannerWithString:rString] scanHexInt:&r];
    [[NSScanner scannerWithString:gString] scanHexInt:&g];
    [[NSScanner scannerWithString:bString] scanHexInt:&b];
    [[NSScanner scannerWithString:aString] scanHexInt:&a];
    //转换为UIColor
    return [UIColor colorWithRed:((float) r / 255.0f)
                           green:((float) g / 255.0f)
                            blue:((float) b / 255.0f)
                           alpha:((float) a / 255.0f)];
}


@end
