//
//  SFAppLaunchingAnimationController.h
//  VTTeamProject
//
//  Created by zhoucheng on 15/7/10.
//  Copyright (c) 2015年 ytang.com. All rights reserved.
//

#import <Foundation/Foundation.h>

#define APP_ANIMATION_IMAGE_SIZE_IPHONE4     CGSizeMake(320, 480)
#define APP_ANIMATION_IMAGE_SIZE_IPHONE5     CGSizeMake(320, 568)
#define APP_ANIMATION_IMAGE_SIZE_IPAD        CGSizeMake(768, 576)
#define APP_ANIMATION_IMAGE_SIZE_IPHONE6     CGSizeMake(375, 667)
#define APP_ANIMATION_IMAGE_SIZE_IPHONE6P    CGSizeMake(414, 736)
#define APP_ANIMATION_IMAGE_SIZE_NOT_SUPPORT CGSizeMake(0,0)

/*
#define PERFIX_IPHONE4 @"640x960"
#define PERFIX_IPHONE5 @""
#define PERFIX_IPHONE6 @"750x1334"
#define PERFIX_IPHONE6P @"1242x2208"
*/

typedef void (^animationFinishBlcok)(BOOL finish);

@interface HCAppImageAnimationController : NSObject

//动画展示视图View，如果不设置默认为UIWindow，Level为UIWindowLevelAlert
@property(nonatomic, assign)UIView * animationView;

//动画是否正在执行
@property(nonatomic, assign)BOOL isAnimation;

@property(nonatomic, assign)UIWindowLevel animationWindowLevel;

//第一帧动画停留延时，默认0.5秒
@property(nonatomic, assign)NSTimeInterval firstImageDelay;

//最后一帧动画停留延时 默认0.5秒
@property(nonatomic, assign)NSTimeInterval lastImageDelay;

//动画超时时间 默认5秒, 设置为0为永不超时
@property(nonatomic, assign)NSTimeInterval animationTimeoutInterval;

//动画执行时间 默认2秒
@property(nonatomic, assign)NSTimeInterval animationDuration;

//动画执行次数 默认1次,设置为0为无线循环
@property(nonatomic, assign)CGFloat animationRepeatCount;

//动画图片数量
@property(nonatomic, assign)NSInteger animationImageCount;

//第一帧延时图片名
@property(nonatomic, strong)NSString * firstImageNamed;

//第一帧延时图片
@property(nonatomic, strong)UIImage * firstImage;

//自动设置动画第一帧为首帧延时图片，默认为YES
@property(nonatomic, assign)BOOL autoSetFirstImage;

//最后一帧延时图片名
@property(nonatomic, strong)NSString * lastImageNamed;

//最后一帧延时图片
@property(nonatomic, strong)UIImage * lastImage;

//自动设置动画最后一帧为末帧延时图片，默认为YES
@property(nonatomic, assign)BOOL autoSetLastImage;

//返回值为每一帧动画的图片名（imageNamed）会被循环调用(类似tableView的cellForRowAtIndexPath方法),如果实现了animationImageAtIndex的block，则此方法不会执行
@property (nonatomic , copy) NSString *(^animationImageNamedAtIndex)(NSInteger index);

//返回值为每一帧动画的图片（UIImage）会被循环调用(类似tableView的cellForRowAtIndexPath方法)
@property (nonatomic , copy) UIImage *(^animationImageAtIndex)(NSInteger index);

//动画视图即将消失（动画已执行完成，进入最后一帧延时阶段）
@property (nonatomic , copy) void (^animationViewWillDisappear) (void);

//开始动画&动画执行完成的回调block
- (void)beginImageAnimationWithDidFinishBlock:(animationFinishBlcok)finishBlcok;

//停止动画
- (void)stopImageAnimation;

//对比当前设备屏幕尺寸
- (BOOL)comparSizeIsEqual:(CGSize)size;


@end
