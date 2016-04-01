/**
 *  HCShell.m
 *  HouseCar
 *
 *  Created by zhang hailong on 13-7-5.
 *  Copyright ytang.com. 2013年. All rights reserved.
 */

#import <AdSupport/ASIdentifierManager.h>

#import "HCShell.h"

#import "HCGuideViewController.h"
#import "HCNotificationKeys.h"

#import <CoreTelephony/CTTelephonyNetworkInfo.h>
#import <CoreTelephony/CTCarrier.h>

CGFloat SFStatusOffsetHeight = 0;
CGFloat SFSearchBarHeight = 54;
CGFloat SFTitleBarHeight = 44;
CGFloat SFTopBarHeight = 44;
CGFloat SFBottomOffsetHeight = 0;

@interface HCShell(){
    
}

@property(nonatomic, assign)NSInteger tabbarItemSelectedIndex;

@end

@implementation HCShell

@synthesize appDBContext = _appDBContext;
@synthesize userDBContext = _userDBContext;
@synthesize anonymousDBContext = _anonymousDBContext;
@synthesize mobileCarrier = _mobileCarrier;
@synthesize deviceType = _deviceType;
@synthesize citiesData = _citiesData;
@synthesize chineseLetterCitiesData = _chineseLetterCitiesData;

-(id) init{
    if((self = [super initWithConfig:[NSDictionary dictionaryWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"cfg" ofType:@"plist"]] bundle:nil])){
        
        if([[[UIDevice currentDevice] systemVersion] doubleValue] >= 7.0){
            SFStatusOffsetHeight = 20;
            SFSearchBarHeight = 64;
            SFTitleBarHeight = 64;
            SFTopBarHeight = 64;
            SFBottomOffsetHeight = 49;
        }
        
        //获取当前设备
        int width = [UIScreen mainScreen].bounds.size.width;
        int height = [UIScreen mainScreen].bounds.size.height;
        switch(width){
            case 320:{
                if (height == 480){
                    self.deviceType = AppleIphone4;
                } else {
                    self.deviceType = AppleIphone5;
                }
            }
                break;
            case 375:
                self.deviceType = AppleIphone6;
                break;
            case 414:
                self.deviceType = AppleIphone6plus;
                break;
            default:
                self.deviceType = AppleIphone5;
                break;
                
        }
        
        
        NSString *path=[[NSBundle mainBundle] pathForResource:@"citydict" ofType:@"plist"];
        self.citiesData = [[NSArray alloc] initWithContentsOfFile:path];
        [self handleCityName];
        
    }
    return self;
}

-(NSString *)aboutInfo
{
    UIDevice * device = [UIDevice currentDevice];
    NSBundle * bundle = [NSBundle mainBundle];
    
    NSMutableDictionary * header = [NSMutableDictionary dictionaryWithCapacity:1];
    
    NSMutableDictionary *app = [NSMutableDictionary dictionaryWithCapacity:15];
    
    [app setValue:[[bundle infoDictionary] objectForKey:@"CFBundleVersion"] forKey:@"version"];
    UInt64 recordTime = [[NSDate date] timeIntervalSince1970]*1000;
    [app setValue:[NSString stringWithFormat:@"%lld",recordTime] forKey:@"timestamp"];
    [app setValue:[NSString stringWithFormat:@"%f",[[[UIDevice currentDevice] systemVersion] floatValue]] forKey:@"osv"];
    [app setValue:@{} forKey:@"targeting"];
    [app setValue:@"iOS" forKey:@"os"];
    [app setValue:@[@"640*104",@"unknown"] forKey:@"size"];//640*104
    [app setValue:@"4" forKey:@"device_type"];
    [app setValue:[self getNetWorkStates] forKey:@"connection_type"];
    [app setValue:@"" forKey:@"ip"];
    [app setValue:@"apple" forKey:@"make"];
    [app setValue:[device standardPlat]?[device standardPlat]:@"" forKey:@"model"];
    [app setValue:[self mobileCarrier] forKey:@"carrier"];
    [app setValue:[[bundle infoDictionary] objectForKey:@"CFBundleName"] forKey:@"name"];
    
    [header setValue:app forKey:@"app"];
    
    NSString *jsonStr = [VTJSON encodeObject:header];
    
    return jsonStr;
}

-(NSString *)getNetWorkStates{
    UIApplication *app = [UIApplication sharedApplication];
    NSArray *children = [[[app valueForKeyPath:@"statusBar"]valueForKeyPath:@"foregroundView"]subviews];
    NSString *state = [[NSString alloc]init];
    int netType = 0;
    //获取到网络返回码
    for (id child in children) {
        if ([child isKindOfClass:NSClassFromString(@"UIStatusBarDataNetworkItemView")]) {
            //获取到状态栏
            netType = [[child valueForKeyPath:@"dataNetworkType"]intValue];
            
            switch (netType) {
                case 0:
                    state = @"0";//@"无网络"
                    //无网模式
                    break;
                case 1:
                    state = @"4";//2G
                    break;
                case 2:
                    state = @"5";//3G
                    break;
                case 3:
                    state = @"6";//4G
                    break;
                case 5:
                    state = @"2";//WIFI
                    break;
                default:
                    state = @"3";//cellular network-unknown generation
                    break;
            }
        }
    }
    //根据状态选择
    return state;
}

-(id) initWithBundle:(NSBundle *) bundle{
    
    if((self = [super initWithConfig:[NSDictionary dictionaryWithContentsOfFile:[bundle pathForResource:@"cfg" ofType:@"plist"]] bundle:bundle])){
        
        if([[[UIDevice currentDevice] systemVersion] doubleValue] >= 7.0){
            SFStatusOffsetHeight = 20;
            SFSearchBarHeight = 44;
            SFTitleBarHeight = 44;
            SFTopBarHeight = 64;
            SFBottomOffsetHeight = 49;
        }
    }
    
    return self;
}
/*
 -(void)registerTester
 {
 //    [self setAccess_token:@"2.00XOFjmBY99Z8Dd6795a19a6NvxAoB"];
 [self setAccess_token:nil];
 [self setUid:@"1636141841"];
 [self setRefresh_token:@"2.00XOFjmBY99Z8D940e1ce046OiEqKE"];
 [self setExpires_in:2651160];
 }
 */
-(BOOL) application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions{
    
    NSLog(@"%@",NSHomeDirectory());
    
    //友盟分享
    [UMSocialData setAppKey:UMAppKey];
    
    _window.rootViewController = [[VTViewController alloc] init];
    if (!self.window.rootViewController || self.window.rootViewController != self.rootViewController) {
        [self initWindowRootViewController];
    }
    
    return YES;
}

- (void)initWindowRootViewController
{//初始化rootViewController
    if([self isGuided]){
        [_window setRootViewController:self.rootViewController];
    }
    else{
        id viewController = [self getViewController:[NSURL URLWithString:@"local:///guide"] basePath:@"/"];
        [viewController setParentController:self.rootViewController];
        [_window setRootViewController:viewController];
    }
    
    if ([[UIDevice currentDevice].systemVersion floatValue] >= 7.0
        && [self.rootViewController isKindOfClass:[VTHeapViewController class]]
        && [[[self rootViewController] bottomViewController] isKindOfClass:[VTTabBarController class]]) {
        VTTabBarController * tabBarController = (VTTabBarController *)[[self rootViewController] bottomViewController];
        if (tabBarController && !tabBarController.delegate) {
            [tabBarController setDelegate:self];
            if (tabBarController.selectedViewController) {
                self.tabbarItemSelectedIndex = 999;
                if ([self respondsToSelector:@selector(tabBarController:didSelectViewController:)]) {
                    [self tabBarController:tabBarController didSelectViewController:tabBarController.selectedViewController];
                }
            }
        }
    }

}

-(void)application:(UIApplication *)application didRegisterUserNotificationSettings:(UIUserNotificationSettings *)notificationSettings{
    if (notificationSettings.types != UIUserNotificationTypeNone) {
        [application registerForRemoteNotifications];
    }
}

-(void) applicationWillTerminate:(UIApplication *)application{
    [self setFocusValue:nil forKey:@"CarUserBeginDate"];
    [self setFocusValue:nil forKey:@"CarUserEndDate"];
    
    [self didReceiveMemoryWarning];
}

-(void) applicationDidReceiveMemoryWarning:(UIApplication *)application{
    [self didReceiveMemoryWarning];
}


-(void) applicationDidEnterBackground:(UIApplication *)application{
    [self didReceiveMemoryWarning];
    
}

- (void)applicationDidBecomeActive:(UIApplication *)application{
    
}

-(void) applicationWillEnterForeground:(UIApplication *)application{
    
    while([VTPopWindow topPopWindow]){
        [[VTPopWindow topPopWindow] closeAnimated:NO];
    }
    
}

-(void) application:(UIApplication *)application didFailToRegisterForRemoteNotificationsWithError:(NSError *)error{
    NSLog(@"%@",error);
}

-(void) application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken{
    
    NSString * token = [[[[deviceToken description] stringByReplacingOccurrencesOfString:@" " withString:@""]
                         stringByReplacingOccurrencesOfString:@"<" withString:@""]
                        stringByReplacingOccurrencesOfString:@">" withString:@""];
    
    
    NSLog(@"didRegisterForRemoteNotificationsWithDeviceToken %@",token);
    
}


-(void) application:(UIApplication *)application didReceiveLocalNotification:(UILocalNotification *)notification{
    
    
}

-(void) application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo
{
    
    if([application applicationState] == UIApplicationStateInactive){
        
    }
    else{
        
    }
    
}

-(BOOL) application:(UIApplication *)application handleOpenURL:(NSURL *)url{
    
    NSString * scheme = [url scheme];
    
    
    
    return YES;
}

-(BOOL) application:(UIApplication *)application openURL:(NSURL *)url sourceApplication:(NSString *)sourceApplication annotation:(id)annotation{
    
    NSString * scheme = [url scheme];
    
    
    return YES;
}

-(VTDBDataContext *) appDBContext{
    
    if(_appDBContext == nil){
        
        VTSqlite * sqlite = [[VTSqlite alloc] initWithPath:[NSHomeDirectory() stringByAppendingPathComponent:@"Documents/app.db"]];
        
        _appDBContext = [[VTDBDataContext alloc] init];
        
        [_appDBContext setDb:sqlite];
        
        NSArray * items = [self.config valueForKey:@"app-db"];
        for(NSString * item in items){
            Class clazz = NSClassFromString(item);
            if(clazz && [clazz isSubclassOfClass:[VTDBObject class]]){
                [_appDBContext regDBObjectClass:clazz];
            }
        }
        
    }
    
    return _appDBContext;
}

-(VTDBDataContext *) anonymousDBContext{
    
    if(_anonymousDBContext == nil){
        
        VTSqlite * sqlite = [[VTSqlite alloc] initWithPath:[NSHomeDirectory() stringByAppendingPathComponent:[NSString stringWithFormat:@"Documents/anonymous.db"]]];
        
        _anonymousDBContext = [[VTDBDataContext alloc] init];
        
        [_anonymousDBContext setDb:sqlite];
        
        NSArray * items = [self.config valueForKey:@"user-db"];
        for(NSString * item in items){
            Class clazz = NSClassFromString(item);
            if(clazz && [clazz isSubclassOfClass:[VTDBObject class]]){
                [_anonymousDBContext regDBObjectClass:clazz];
            }
        }
        
    }
    
    return _anonymousDBContext;
}

-(VTDBDataContext *) userDBContext{
    
    if(_userDBContext == nil){
        
        if([self uid]){
            VTSqlite * sqlite = [[VTSqlite alloc] initWithPath:[NSHomeDirectory() stringByAppendingPathComponent:[NSString stringWithFormat:@"Documents/u_%@.db",self.uid]]];
            
            _userDBContext = [[VTDBDataContext alloc] init];
            
            [_userDBContext setDb:sqlite];
        }
        else{
            _userDBContext = self.anonymousDBContext;
        }
        
        NSArray * items = [self.config valueForKey:@"user-db"];
        for(NSString * item in items){
            Class clazz = NSClassFromString(item);
            if(clazz && [clazz isSubclassOfClass:[VTDBObject class]]){
                [_userDBContext regDBObjectClass:clazz];
            }
        }
        
    }
    
    return _userDBContext;
}

-(NSString *) access_token{
    return [self authValueForKey:@"access_token"];
}

-(void) setAccess_token:(NSString *)access_token{
    [self setAuthValue:access_token forKey:@"access_token"];
}

-(NSString *) refresh_token{
    return [self authValueForKey:@"refresh_token"];
}

-(void) setRefresh_token:(NSString *)refresh_token{
    [self setAuthValue:refresh_token forKey:@"refresh_token"];
}

-(NSString *) deviceToken{
    NSUserDefaults * userDefaults = [NSUserDefaults standardUserDefaults];
    
    NSString * deviceToken = [userDefaults valueForKey:@"deviceToken"];
    
    if(deviceToken == nil){
        if([[UIDevice currentDevice] respondsToSelector:@selector(identifierForVendor)]){
            deviceToken = [[[[UIDevice currentDevice] identifierForVendor] UUIDString] vtMD5String];
        }
        else{
            deviceToken = [[[NSUUID UUID] UUIDString] vtMD5String];
        }
        [userDefaults setValue:deviceToken forKey:@"deviceToken"];
        [userDefaults synchronize];
    }
    
    return deviceToken;
    
}

-(void) setDeviceToken:(NSString *)deviceToken{
    NSUserDefaults * userDefaults = [NSUserDefaults standardUserDefaults];
    [userDefaults setValue:deviceToken forKey:@"deviceToken"];
    [userDefaults synchronize];
}

-(BOOL) isPushDeviceToken{
    NSUserDefaults * userDefaults = [NSUserDefaults standardUserDefaults];
    return [[userDefaults valueForKey:@"isPushDeviceToken"] boolValue];
}

-(void) setPushDeviceToken:(BOOL)pushDeviceToken{
    NSUserDefaults * userDefaults = [NSUserDefaults standardUserDefaults];
    [userDefaults setValue:[NSNumber numberWithBool:pushDeviceToken] forKey:@"isPushDeviceToken"];
    [userDefaults synchronize];
    
}

-(BOOL) isGuided{
    NSString * key = [NSString stringWithFormat:@"isGuided_%@"
                      ,[[[NSBundle mainBundle] infoDictionary] valueForKey:(id)kCFBundleVersionKey]];
    NSUserDefaults * userDefaults = [NSUserDefaults standardUserDefaults];
    return [[userDefaults valueForKey:key] boolValue];
}

-(void) setGuided:(BOOL)guided{
    NSString * key = [NSString stringWithFormat:@"isGuided_%@"
                      ,[[[NSBundle mainBundle] infoDictionary] valueForKey:(id)kCFBundleVersionKey]];
    NSUserDefaults * userDefaults = [NSUserDefaults standardUserDefaults];
    [userDefaults setValue:[NSNumber numberWithBool:guided] forKey:key];
    [userDefaults synchronize];
    
}

-(NSString *) lastVersion{
    NSUserDefaults * userDefaults = [NSUserDefaults standardUserDefaults];
    return [userDefaults valueForKey:@"AppLastVersion"];
}

-(void) setLastVersion:(NSString *)lastVersion{
    NSUserDefaults * userDefaults = [NSUserDefaults standardUserDefaults];
    [userDefaults setValue:lastVersion forKey:@"AppLastVersion"];
    [userDefaults synchronize];
}

-(void) toRootViewController{
    [_window setRootViewController:[self rootViewController]];
    [self doTransitionsAnimation];
}

-(void)doTransitionsAnimation
{
    CATransition *trans=[CATransition animation];
    
    trans.type=kCATransitionReveal;
    trans.subtype=kCATransitionFromRight;
    trans.timingFunction = [CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseInEaseOut];
    trans.duration = 0.5;
    
    [self.window.layer addAnimation:trans forKey:nil];
}

-(NSInteger) vtDownlinkPageTaskPageSize{
    return 20;
}

-(NSInteger) vtDownlinkPageTaskPageIndex{
    return 1;
}

#pragma mark - 获取手机运营商 名称
-(NSString *)mobileCarrier
{
    if (!_mobileCarrier) {
        CTTelephonyNetworkInfo *info = [[CTTelephonyNetworkInfo alloc]init];
        CTCarrier *carrier = [info subscriberCellularProvider];
        
        if (carrier==nil) {
            return @"";
        }
        
        NSString *code = [carrier mobileNetworkCode];
        if (code==nil) {
            return @"";
        }
        
        if ([code isEqualToString:@"00"] || [code isEqualToString:@"02"] || [code isEqualToString:@"07"]) {
            _mobileCarrier = @"中国移动";
        }else if([code isEqualToString:@"01"] || [code isEqualToString:@"06"]){
            _mobileCarrier = @"中国联通";
        }else if([code isEqualToString:@"03"] || [code isEqualToString:@"05"]){
            _mobileCarrier = @"中国电信";
        }else if([code isEqualToString:@"20"]){
            _mobileCarrier = @"中国铁通";
        }else{
            return @"";
        }
    }
    return _mobileCarrier;
}

- (void)setAuthValue:(id)value forKey:(NSString *)key
{
    [super setAuthValue:value forKey:key];
    [self updateKeyChainBackupData];
}

- (void)resetKeyChainData
{
    if ([[NSUserDefaults standardUserDefaults] objectForKey:@"userDefaults_cache_data"]) {
        NSDictionary * dict = [NSKeyedUnarchiver unarchiveObjectWithData:[[NSUserDefaults standardUserDefaults] objectForKey:@"userDefaults_cache_data"]];
        for (NSString * key in dict.allKeys) {
            [self setAuthValue:[dict objectForKey:key] forKey:key];
        }
    }
}

- (void)reloadKeyChainAndBackupData
{
    NSDictionary * dict = [NSKeyedUnarchiver unarchiveObjectWithData:[[NSUserDefaults standardUserDefaults] objectForKey:@"userDefaults_cache_data"]];
    if (self.uid == nil && [dict objectForKey:@"uid"]) {
        [self resetKeyChainData];
    }else if (self.uid && ![dict objectForKey:@"uid"]){
        [self updateKeyChainBackupData];
    }
}

- (void)updateKeyChainBackupData
{
    NSMutableDictionary * dict = [[NSMutableDictionary alloc] initWithCapacity:0];
    for (NSString * key in self.authKeys) {
        [dict  setObject:[self authValueForKey:key] forKey:key];
    }
    [[NSUserDefaults standardUserDefaults] setObject:[NSKeyedArchiver archivedDataWithRootObject:dict] forKey:@"userDefaults_cache_data"];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

//处理城市查询数据以方便进行查询

- (NSMutableArray *)chineseLetterCitiesData
{
    if(!_chineseLetterCitiesData){
        _chineseLetterCitiesData = [NSMutableArray array];
    }
    return _chineseLetterCitiesData;
}

- (void)handleCityName
{
    __weak HCShell *_weakSelf = self;
    dispatch_async(dispatch_queue_create("handleCityName", NULL), ^{
        for (NSDictionary *cityData in _citiesData) {
            NSString *keyName = [[cityData allKeys] firstObject];
            if(![keyName containsString:@"热门"]){
                NSArray *cityNameArr = [cityData objectForKey:keyName];
                for(NSString *cityName in cityNameArr){
                    if ([cityName length]) {
                        NSMutableDictionary *cityReference = [NSMutableDictionary dictionary];
                        NSMutableString *ms = [[NSMutableString alloc] initWithString:cityName];
                        if (CFStringTransform((__bridge CFMutableStringRef)ms, 0, kCFStringTransformMandarinLatin, NO)){
                            if (CFStringTransform((__bridge CFMutableStringRef)ms, 0,kCFStringTransformStripDiacritics, NO)){
                                if(ms && [ms length] > 0){
                                    NSArray *splitLetters = [ms componentsSeparatedByString:@" "];
                                    NSString *initialLetters = @"";
                                    NSString *initialWords = @"";
                                    for (int i = 0; i < [splitLetters count]; i++) {
                                        NSString *letter = splitLetters[i];
                                        NSString *initial = [letter substringToIndex:1];
                                        initialLetters = [initialLetters stringByAppendingString:initial];
                                        initialWords = [initialWords stringByAppendingString:letter];
                                    }
                                    [cityReference setValue:cityName forKey:@"chineseName"];
                                    [cityReference setValue:initialWords forKey:@"initialWords"];
                                    [cityReference setValue:initialLetters forKey:@"initialLetters"];
                                    
                                    [_weakSelf.chineseLetterCitiesData addObject:cityReference];
                                }
                            }
                        }
                    }
                }
            }
        }
    });
}


#pragma mark - tabbarController点击event事件
- (BOOL)tabBarController:(UITabBarController *)tabBarController shouldSelectViewController:(nonnull UIViewController *)viewController
{
    if ([tabBarController.viewControllers containsObject:viewController]) {
        NSInteger selectedIndex = [tabBarController.viewControllers indexOfObject:viewController];
        switch (selectedIndex) {
            case 0:
            {
                
            }
                break;
            case 1:
            {
                
            }
                break;
            case 2:
            {
                [[self rootViewController] openUrl:[NSURL URLWithString:@"present://root/nav/loginHomePage"]
                                                          animated:YES];
                return NO;
                
            }
                break;
            case 3:
            {
                
            }
                break;
            case 4:
            {

            }
                break;
            default:
                break;
        }
    }
    return YES;
}

- (void)tabBarController:(UITabBarController *)tabBarController didSelectViewController:(UIViewController *)viewController
{
    if ([tabBarController.viewControllers containsObject:viewController]) {
        NSInteger selectedIndex = [tabBarController.viewControllers indexOfObject:viewController];
        if(self.tabbarItemSelectedIndex!= selectedIndex){
            switch (selectedIndex) {
                case 0:
                {
                   
                }
                    break;
                case 1:
                {
                   
                }
                    break;
                case 2:
                {
                    
                    
                }
                    break;
                case 3:
                {
                   
                }
                    break;
                case 4:
                {
                    
                }
                    break;
                default:
                    break;
            }
            
        }
        self.tabbarItemSelectedIndex = selectedIndex;
    }
}

@end
