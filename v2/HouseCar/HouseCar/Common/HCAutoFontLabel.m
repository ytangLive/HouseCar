//
//  SFSpringFontLabel.m
//  SinaFinance
//
//  Created by Sina_Mobile on 15/4/20.
//  Copyright (c) 2015年 sina.com. All rights reserved.
//

#import "HCAutoFontLabel.h"

@implementation HCAutoFontLabel

/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect {
    // Drawing code
}
*/

- (void)initialize
{
    self.needAutoFont = YES;
    if (!self.miniFontSize) {
        self.miniFontSize = @"0";
    }
}

- (instancetype)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        [self initialize];
    }
    return self;
}

- (instancetype)init
{
    self = [super init];
    if (self) {
        [self initialize];
    }
    return self;
}

- (id)initWithCoder:(NSCoder *)aDecoder
{
    self = [super initWithCoder:aDecoder];
    if (self) {
        [self initialize];
    }
    return self;
}

- (void)setFont:(UIFont *)font
{
    [super setFont:font];
}

- (void)setDefaultFontSize:(NSString *)defaultFontSize
{
    _defaultFontSize = defaultFontSize;
}

- (void)setMiniFontSize:(NSString *)miniFontSize
{
    _miniFontSize = miniFontSize;
}

- (void)setNeedAutoFont:(BOOL)needAutoFont
{
    _needAutoFont = needAutoFont;
}

- (void)setText:(NSString *)text
{
    BOOL textEqual = [text isEqualToString:self.text];
    [super setText:text];
    if (text && text.length > 0 && !textEqual) {
        [self autoLabelFont];
    }
}

- (CGFloat)fontSizeWithLable:(UILabel *)label textFont:(CGFloat)fontSize
{
    if (self.numberOfLines == 1) {
        CGSize textSize = [self.text VTSizeWithFont:[self.font fontWithSize:fontSize] constrainedToSize:CGSizeMake(100000, self.frame.size.height) lineBreakMode:self.lineBreakMode];
        if (textSize.width > self.frame.size.width && fontSize > [_miniFontSize floatValue]) {
            fontSize -= 1;
            fontSize = [self fontSizeWithLable:self textFont:fontSize];
        }
    }else{
        CGSize textSize = [self.text VTSizeWithFont:[self.font fontWithSize:fontSize] constrainedToSize:CGSizeMake(self.frame.size.width, 100000) lineBreakMode:self.lineBreakMode];
        if(textSize.height > self.frame.size.height && fontSize > [_miniFontSize floatValue]){
            fontSize -= 1;
            fontSize = [self fontSizeWithLable:self textFont:fontSize];
        }
    }
    return fontSize;
}

- (void)autoLabelFont
{
    if (self.text && self.text.length > 0) {
        if (!_defaultFontSize && self.font.pointSize > 0) {
            self.defaultFontSize = [NSString stringWithFormat:@"%@", @(self.font.pointSize)];
        }
        if (_needAutoFont) {
            CGFloat fontSize = [self fontSizeWithLable:self textFont:[_defaultFontSize floatValue]];
            [self setFont:[self.font fontWithSize:fontSize]];
        }
    }
}

- (void)layoutSubviews
{
    [self autoLabelFont];
}

@end
