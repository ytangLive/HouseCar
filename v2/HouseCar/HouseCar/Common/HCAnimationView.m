//
//  HCAnimationView.m
//  HouseCar
//
//  Created by sinanews on 13-9-18.
//  Copyright (c) 2013年 ytang.com. All rights reserved.
//

#import "HCAnimationView.h"
@interface HCAnimationView ()

@property (assign,nonatomic) CGImageRef bgImageRef;
@property (assign,nonatomic) CGImageRef coverImageRef;
@property (assign,nonatomic) CGImageRef borderImageRef;
@property (assign,nonatomic) CGImageRef loopImageRef;
@property (strong,nonatomic) NSTimer *loopTimer;
@property (assign,nonatomic) CGFloat loopCount;
@property (assign,nonatomic) CGFloat loopCountMax;

@end

@implementation HCAnimationView

- (void)setHidden:(BOOL)hidden
{
    [super setHidden:NO];
}

-(void)awakeFromNib
{
    self.bgImageRef = CGImageRetain([[UIImage imageNamed:_backgroundImg] CGImage]);
    self.coverImageRef = CGImageRetain([[UIImage imageNamed:_coverImg] CGImage]);
    self.borderImageRef = CGImageRetain([[UIImage imageNamed:_borderImg] CGImage]);
    self.loopImageRef = CGImageRetain([[UIImage imageNamed:_loopImg] CGImage]);
    self.loopCountMax = self.bounds.size.width * 2;
}

-(void)dealloc
{
    CGImageRelease(_bgImageRef);
    CGImageRelease(_coverImageRef);
    CGImageRelease(_borderImageRef);
    CGImageRelease(_loopImageRef);
}

-(void)reloadRes
{
    CGImageRelease(_bgImageRef);
    CGImageRelease(_coverImageRef);
    CGImageRelease(_borderImageRef);
    CGImageRelease(_loopImageRef);
    
    self.bgImageRef = CGImageRetain([[UIImage imageNamed:_backgroundImg] CGImage]);
    self.coverImageRef = CGImageRetain([[UIImage imageNamed:_coverImg] CGImage]);
    self.borderImageRef = CGImageRetain([[UIImage imageNamed:_borderImg] CGImage]);
    self.loopImageRef = CGImageRetain([[UIImage imageNamed:_loopImg] CGImage]);
    self.loopCountMax = self.bounds.size.width * 2;
}

// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect
{
    // Drawing code
    CGContextRef context = UIGraphicsGetCurrentContext();
    CGContextSaveGState(context);
    CGContextTranslateCTM(context, 0, self.bounds.size.height);
    CGContextScaleCTM(context, 1.0, -1.0);
    
    if (_animating) {
        CGContextDrawImage(context, self.bounds, _borderImageRef);
        CGRect rcOffset = self.bounds;
        rcOffset.origin.x = -1*_loopCount;
        rcOffset.size.width = _loopCountMax;
        CGContextDrawImage(context, rcOffset, _loopImageRef);
    }
    else {
        CGRect rcOffset = self.bounds;
        rcOffset.size.width = self.bounds.size.width * _offsetValue;
        CGContextDrawImage(context, self.bounds, _bgImageRef);
        CGContextClipToRect(context, rcOffset);
        CGContextDrawImage(context, self.bounds, _coverImageRef);
    }
    
    CGContextRestoreGState(context);
}

- (void)startAnimating {
    self.loopCount = 0.0;
    self.loopTimer = [NSTimer scheduledTimerWithTimeInterval:0.03
                                                      target:self
                                                    selector:@selector(handleTimer)
                                                    userInfo:nil
                                                     repeats:YES];
    _animating = YES;
    [self setNeedsDisplay];
}

- (void)stopAnimating {
    [_loopTimer invalidate];
    _animating = NO;
    [self setNeedsDisplay];
}

- (BOOL)isAnimating {
    return _animating;
}

- (void)setOffsetValue:(CGFloat)value {
    if (value < 0.0)
        value = 0.0;
    else if (value > 1.0)
        value = 1.0;
    _offsetValue = value;
    [self setNeedsDisplay];
}

-(void)handleTimer
{
    _loopCount += 1.0;
    if (_loopCount > self.bounds.size.width) {
        _loopCount = 0.0;
    }
    [self setNeedsDisplay];
}

@end
