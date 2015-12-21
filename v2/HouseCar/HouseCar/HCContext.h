/**
 *  HCContext.h
 *  HouseCar
 *
 *  Created by zhang hailong on 13-7-5.
 *  Copyright ytang.com 2013年. All rights reserved.
 */

#import <vTeam/vTeam.h>
#import "UMSocial.h"
#import "NSError+SFError.h"

#define WEIBO_APPKEY    @""
#define WEIBO_SECRET    @""

// 系统版本号枚举
#define IOS_2_0 @"2.0"
#define IOS_3_0 @"3.0"
#define IOS_4_0 @"4.0"
#define IOS_5_0 @"5.0"
#define IOS_6_0 @"6.0"
#define IOS_7_0 @"7.0"
#define IOS_8_0 @"8.0"

typedef enum {
    AppleIphone4 = 0,
    AppleIphone5,
    AppleIphone6,
    AppleIphone6plus,
} DeviceScreenType;

//第三方APPKey
#define UMAppKey @"56696de167e58ef83e000060"

//kCalendar
#define kDarkGrayColor       COLOR_RGB(51, 51, 51)
#define kGrayColor           COLOR_RGB(153, 153, 153)
#define kLightGrayColor      COLOR_RGB(185, 185, 185)

#define kTileSize CGSizeMake((KScreenWidth + 2) / 7, 48)

//当前屏幕宽度
#define KScreenWidth [[UIScreen mainScreen] bounds].size.width
#define KOffOriginalY   ((KDeviceSystemVersion >= 7) ? (20) : (0))
#define KDeviceSystemVersion [[[[[UIDevice currentDevice] systemVersion]componentsSeparatedByString:@"."] objectAtIndex:0] intValue]

//默认头部和底部导航颜色
#define COLOR_RGB(r,g,b) [UIColor colorWithRed:(r)/255.0 green:(g)/255.0 blue:(b)/255.0 alpha:1.0]
#define COLOR_HEX(hex)  [UIColor colorWithRed:((hex & 0xFF0000) >> 16) / 255.f green:((hex & 0xFF00) >> 8) / 255.f blue:(hex & 0xFF) / 255.f alpha:1]

#define DefaultNavBarColor COLOR_RGB(51,187,235)
#define DefaultTabBarColor COLOR_RGB(237,238,240)
#define DefaultBackgroundColor COLOR_HEX(0xefefef)
#define TableViewBackgroundColor COLOR_HEX(0xFBF8EF)
#define DefaultColor COLOR_RGB(0,188,212)

// rgb颜色转换（16进制->10进制）
#define UIColorFromRGB(rgbValue) [UIColor colorWithRed:((float)((rgbValue & 0xFF0000) >> 16))/255.0 green:((float)((rgbValue & 0xFF00) >> 8))/255.0 blue:((float)(rgbValue & 0xFF))/255.0 alpha:1.0]

// detect current system version upon v
// >=
#define SYSTEM_VERSION_GREATER_THAN_OR_EQUAL_TO(v)  ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] != NSOrderedAscending)
// >
#define SYSTEM_VERSION_GREATER_THAN(v)  ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] == NSOrderedDescending)
// ==
#define SYSTEM_VERSION_EQUAL_TO(v)  ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] == NSOrderedSame)

#define IOS_VERSION [[[UIDevice currentDevice] systemVersion] floatValue]

#define CurrentSystemVersion [[UIDevice currentDevice] systemVersion]

#define IOS7_OR_LATER ( [[[UIDevice currentDevice] systemVersion] compare:@"7.0"] != NSOrderedAscending )

@protocol HCContext <IVTUIContext>

@property(nonatomic,readonly) VTDBDataContext * appDBContext;
@property(nonatomic,readonly) VTDBDataContext * userDBContext;
@property(nonatomic,readonly) VTDBDataContext * anonymousDBContext;

@property(nonatomic,retain) NSString * lastVersion;

@property(nonatomic,assign,getter=isGuided) BOOL guided;

@property(nonatomic,retain) NSString * access_token;
@property(nonatomic,retain) NSString * refresh_token;

@property(nonatomic,retain) NSString * deviceToken;
@property(nonatomic,assign,getter=isPushDeviceToken) BOOL pushDeviceToken;

//城市列表
@property (nonatomic, strong) NSArray *citiesData;
//处理后城市查询列表
@property (nonatomic, strong) NSMutableArray *chineseLetterCitiesData;


//设备类型
@property(nonatomic) DeviceScreenType deviceType;
//运营商类型
@property(nonatomic,retain) NSString *mobileCarrier;

-(void) toRootViewController;

@end
