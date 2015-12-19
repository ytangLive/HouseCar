//
//  SFAppLaunchingAnimationController.m
//  VTTeamProject
//
//  Created by zhoucheng on 15/7/10.
//  Copyright (c) 2015年 ytang.com. All rights reserved.
//

#import "HCAppImageAnimationController.h"

#define IMAGE_START_INDEX 0

@interface HCAppImageAnimationController()
{
    UIWindow * _animationWindow;
    UIImageView * _animationImageView;
}

@property(nonatomic, copy)animationFinishBlcok finishBlcok;

@end

@implementation HCAppImageAnimationController

- (void)setFirstImageNamed:(NSString *)firstImageNamed
{
    _firstImageNamed = firstImageNamed;
    _firstImage = [self getImageWithImageNamed:_firstImageNamed];
}

- (void)setLastImageNamed:(NSString *)lastImageNamed
{
    _lastImageNamed = lastImageNamed;
    _lastImage = [self getImageWithImageNamed:_lastImageNamed];
}

- (UIImage *)getFirstImage
{//获取第一张图片
    UIImage * image = nil;
    if (_firstImage) {
        image = _firstImage;
    }else if(self.autoSetFirstImage){
        image = [self getImageWithIndex:IMAGE_START_INDEX];
    }
    
    return image;
}

- (UIImage *)getLastImage
{//获取最后一张图片
    UIImage * image = nil;
    if (_lastImage) {
        image = _lastImage;
    }else if(_autoSetLastImage){
        image = [self getImageWithIndex:_animationImageCount - 1];
    }
    return image;
}

- (void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self name:UIApplicationDidEnterBackgroundNotification object:nil];
}

- (id)init
{
    self = [super init];
    if (self) {
        self.animationDuration = 2.0;
        self.animationRepeatCount = 1.0;
        self.firstImageDelay = 0.5;
        self.lastImageDelay = 0.5;
        self.animationTimeoutInterval = 5.0;
        self.autoSetFirstImage = YES;
        self.autoSetLastImage = YES;
        self.animationWindowLevel = UIWindowLevelAlert;
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(imageAnimationApplicationDidEnterBackgroundNotification) name:UIApplicationDidEnterBackgroundNotification object:nil];
    }
    return self;
}

- (BOOL)comparSizeIsEqual:(CGSize)size
{//对比size
    CGSize currentDeviceSize = [UIScreen mainScreen].bounds.size;
    if (currentDeviceSize.width == size.width && currentDeviceSize.height == size.height) {
        return YES;
    }
    return NO;
}

- (void)beginImageAnimationWithDidFinishBlock:(animationFinishBlcok)finishBlcok
{//开始动画
    _finishBlcok = finishBlcok;
    _isAnimation = NO;
    if (!_animationWindow && !_animationView) {
        CGRect windowRect = CGRectMake(0,
                                       0,
                                       [[UIScreen mainScreen] bounds].size.width,
                                       [[UIScreen mainScreen] bounds].size.height);
        _animationWindow = [[UIWindow alloc] initWithFrame:windowRect];
        _animationWindow.windowLevel = self.animationWindowLevel;
        _animationWindow.backgroundColor = [UIColor whiteColor];
        _animationWindow.rootViewController = [[VTViewController alloc] init];
    }
    
    _animationImageView = [[UIImageView alloc] init];
    [_animationImageView setImage:[self getFirstImage]];//设置第一张图片
    _animationImageView.contentMode = UIViewContentModeScaleToFill;
    if (_animationWindow) {
        _animationImageView.frame = _animationWindow.frame;
        [_animationWindow addSubview:_animationImageView];
        [_animationWindow makeKeyAndVisible];
    }else if (_animationView){
        _animationImageView.frame = CGRectMake(0, 0, _animationView.frame.size.width, _animationView.frame.size.height);
        [_animationView addSubview:_animationImageView];
    }
    
    _isAnimation = YES;
    [self performSelector:@selector(beginLaunchingAnimationWithImageIndex:) withObject:[NSNumber numberWithInteger:IMAGE_START_INDEX] afterDelay:self.firstImageDelay];//开始递归显示动画并设置第一帧动画延时
    if (self.animationTimeoutInterval > 0) {
        [self performSelector:@selector(animationTimeout) withObject:nil afterDelay:self.animationTimeoutInterval];
    }
}

- (void)beginLaunchingAnimationWithImageIndex:(NSNumber *)imageIndex
{
    if (_isAnimation) {
        __weak HCAppImageAnimationController * waekSelf = self;
        dispatch_async(dispatch_queue_create("loadLaunchingAnimationImagesWithIndex", NULL), ^{
            NSTimeInterval callDuration = waekSelf.animationDuration / waekSelf.animationImageCount;//计算平均每贞图片展示时间
            __block NSInteger imageIndexIntegerValue = [imageIndex integerValue];//当前图片索引
            if (imageIndexIntegerValue < (waekSelf.animationImageCount - 1) && callDuration > 0) {//非最后一张图片
                UIImage * image = [self getImageWithIndex:imageIndexIntegerValue];
                dispatch_async(dispatch_get_main_queue(), ^{
                    [_animationImageView setImage:image];
                    [waekSelf performSelector:@selector(beginLaunchingAnimationWithImageIndex:) withObject:[NSNumber numberWithInteger:imageIndexIntegerValue + 1] afterDelay:callDuration];//递归调用展示下一帧动画
                });
            }else{//最后一张图片
                UIImage * image = [self getLastImage];
                dispatch_async(dispatch_get_main_queue(), ^{
                    [_animationImageView setImage:image];
                    if (waekSelf.animationRepeatCount > 1) {//如果动画循环次数>1则继续重复动画
                        waekSelf.animationRepeatCount -= 1;
                        [waekSelf performSelector:@selector(beginLaunchingAnimationWithImageIndex:) withObject:[NSNumber numberWithInteger:IMAGE_START_INDEX] afterDelay:callDuration];
                    }else if(waekSelf.animationRepeatCount == 0){
                        [waekSelf performSelector:@selector(beginLaunchingAnimationWithImageIndex:) withObject:[NSNumber numberWithInteger:IMAGE_START_INDEX] afterDelay:callDuration];
                    }else{//结束动画
                        _isAnimation = NO;
                        if (_animationViewWillDisappear) {
                            _animationViewWillDisappear();
                        }
                        [waekSelf performSelector:@selector(finishAnimation:) withObject:_animationImageView afterDelay:self.lastImageDelay];
                    }
                });
            }
        });
    }
}

- (void)finishAnimation:(UIImageView *)animationImageView
{//完成动画
    if (_animationImageView && (_animationWindow || _animationView)) {
        if (_finishBlcok) {
            _finishBlcok(YES);
        }
        [UIView animateWithDuration:0.3 animations:^{
            if (_animationWindow) {
                [_animationWindow setTransform:CGAffineTransformMakeScale(1.5, 1.5)];
                [_animationWindow setAlpha:0];
            }else if(_animationView){
                [_animationView setAlpha:0];
            }
        } completion:^(BOOL finish){
            _animationWindow = nil;
            _animationImageView = nil;
        }];
    }
}

- (void)animationTimeout
{//动画超时，强制退出动画
    [self stopImageAnimation];
}

- (UIImage *)getImageWithIndex:(NSInteger)index
{//根据索引获取每一帧动画图片
    UIImage * image = nil;
    if (_animationImageAtIndex) {
        image = _animationImageAtIndex(index);
    }else if (_animationImageNamedAtIndex) {
        image = [self getImageWithImageNamed:_animationImageNamedAtIndex(index)];
    }
    return image;
}

- (UIImage *)getImageWithImageNamed:(NSString *)imageNamed
{//根据图片名获取图片
    return [UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:imageNamed ofType:nil]];//避免内存暴增，使用pathForResource加载图片
}

- (void)stopImageAnimation
{//停止动画
    if (_isAnimation) {
        _isAnimation = NO;
        UIImage * image = [self getLastImage];
        [_animationImageView setImage:image];
        if (_animationViewWillDisappear) {
            _animationViewWillDisappear();
        }
        [self performSelector:@selector(finishAnimation:) withObject:_animationImageView afterDelay:self.lastImageDelay];
    }
}

- (void)imageAnimationApplicationDidEnterBackgroundNotification
{
    if (self.animationTimeoutInterval > 0 && _isAnimation && _animationImageView) {
        [NSObject cancelPreviousPerformRequestsWithTarget:self];
         [_animationImageView setImage:[self getLastImage]];
        [self performSelector:@selector(stopImageAnimation) withObject:nil afterDelay:0.1];
    }
}

@end
