//
//  HCActivityDetailDataSource.m
//  HouseCar
//
//  Created by tangyin on 15/12/24.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import "HCActivityDetailDataSource.h"

@implementation HCActivityDetailDataSource

- (void)loadResultsData:(id)resultsData
{
    [super loadResultsData:resultsData];
    
    NSArray *resultPosts = [resultsData arrayValueForKey:@"posts"];
    if(resultPosts && [resultPosts count] > 0){
        NSDictionary *resultPost = [resultPosts lastObject];
        NSString *fullImgUrl = [resultPost stringValueForKeyPath:@"thumbnail_images.full.url"];
        NSString *title = [resultPost stringValueForKey:@"title"];
        NSDictionary *headerViewDic = @{@"title":title,@"imgUrl":fullImgUrl};
        self.headerViewDic = headerViewDic;
    }
    
}

- (NSMutableArray *)attendUsers
{
    if(!_attendUsers){
        _attendUsers = [[NSMutableArray alloc] init];
        NSMutableDictionary *userInfo = [NSMutableDictionary dictionary];
        [userInfo setValue:@"50" forKey:@"totalUser"];
        [userInfo setValue:@"6" forKey:@"attendUser"];
        [userInfo setValue:[NSArray arrayWithObjects:@{@"userID":@"222",@"pUrl":@""},@{@"userID":@"222",@"pUrl":@""},@{@"userID":@"222",@"pUrl":@""},@{@"userID":@"222",@"pUrl":@""},@{@"userID":@"222",@"pUrl":@""},@{@"userID":@"222",@"pUrl":@""},@{@"userID":@"222",@"pUrl":@""}, nil] forKey:@"usersInfo"];
        [_attendUsers addObject:userInfo];
    }
    return _attendUsers;
}

- (NSMutableArray *)locationInfos
{
    if(!_locationInfos){
        _locationInfos = [[NSMutableArray alloc] init];
        NSMutableDictionary *locationInfo = [NSMutableDictionary dictionary];
        [locationInfo setValue:@"北京市海淀区" forKey:@"location"];
        [_locationInfos addObject:locationInfo];
    }
    return _locationInfos;
}

- (NSMutableArray *)attendInfos
{
    if(!_attendInfos){
        _attendInfos = [[NSMutableArray alloc] init];
        NSMutableDictionary *attendInfo = [NSMutableDictionary dictionary];
        [attendInfo setValue:@"2015年12月10日—2016年3月15日" forKey:@"activityDate"];
        [attendInfo setValue:@"2015年11月10日—2016年1月15日" forKey:@"attendDate"];
        [attendInfo setValue:@"￥10000/人" forKey:@"activityCost"];
        [_attendInfos addObject:attendInfo];
    }
    return _attendInfos;
}

- (NSMutableArray *)travelRouteInfos
{
    if(!_travelRouteInfos){
        _travelRouteInfos = [[NSMutableArray alloc] init];
        [_travelRouteInfos addObject:@{@"title":@"D1—D2", @"describe":@"搭乘国航航班直接飞往墨尔本。抵达后，一起前去提车点提车，并就地讲解自驾注意事项，手台使用方法等。" ,@"imageInfo":@[@"http://elephantrv.cn-hangzhou.aliapp.com/wp-content/uploads/2015/12/1-140429111HB49.jpg",@"http://elephantrv.cn-hangzhou.aliapp.com/wp-content/uploads/2015/12/1-140429111HB49.jpg"]}];
        [_travelRouteInfos addObject:@{@"title":@"D3：墨尔本——菲利普岛（约140公里2小时）", @"describe":@"驾车前往菲利普岛，它又称企鹅岛是墨尔本最著名的野生动物岛。" ,@"imageInfo":@[@"http://elephantrv.cn-hangzhou.aliapp.com/wp-content/uploads/2015/12/1-140429111HB49.jpg"]}];
        [_travelRouteInfos addObject:@{@"title":@"D4 墨尔本—悉尼", @"describe":@"抵达后，专车接机。前往海鲜市场“鱼市场”，品尝生猛海鲜（费用自理）。之后前往澳大利亚的象征,——悉尼歌剧院（外观）。您还会在此看到悉尼大桥。随后游览皇家植物园。接着游览风光旖旎的双湾和玫瑰湾。随后我们将游览悉尼的海滩之王——著名的邦迪海滩。" ,@"imageInfo":@[@"http://elephantrv.cn-hangzhou.aliapp.com/wp-content/uploads/2015/12/1-140429111HB49.jpg",@"http://elephantrv.cn-hangzhou.aliapp.com/wp-content/uploads/2015/12/1-140429111HB49.jpg"]}];
    }
    return _travelRouteInfos;
}

- (NSMutableArray *)costActivityInfos
{
    if(!_costActivityInfos){
        _costActivityInfos = [[NSMutableArray alloc] init];
        [_costActivityInfos addObject:@{@"title":@"费用包含",@"content":@"1、护照费用。/n2、驾照公证费（客人自行在当地公证处英文公证即可，约￥150-200/份）/n3、全程加油费用、停车费；/n4、全程午餐、晚餐费用、超市食材采购费；/n5、儿童座椅、额外驾驶员费用等因个人需求产生的费用。/n6、旅游费用不包括旅游者因违约、自身过错、自由活动期间内行为或自身疾病引起的人身和财产损失。"}];
        [_costActivityInfos addObject:@{@"title":@"费用不含",@"content":@"1、护照费用。/n2、驾照公证费（客人自行在当地公证处英文公证即可，约￥150-200/份）/n3、全程加油费用、停车费；/n4、全程午餐、晚餐费用、超市食材采购费；/n5、儿童座椅、额外驾驶员费用等因个人需求产生的费用。/n6、旅游费用不包括旅游者因违约、自身过错、自由活动期间内行为或自身疾病引起的人身和财产损失。"}];
        [_costActivityInfos addObject:@{@"title":@"活动须知",@"content":@"1、团队报价按每台车床位核算，晚间入住男性一辆车，女性一辆车。团队机票一经开出，不得更改，不得签转，不得退票；另飞行时间、车程时间、船程时间以当日实际所用时间为准。/n2、如遇国家或航空公司政策性调整机票、燃油税价格，按调整后的实际价格结算。当地购物时请慎重考虑，把握好质量与价格，请务必保存好所有的购物票据，若购物点提供发票，请索要。/n3、是否给予签证或签注、是否准予出入境，是使领馆及有关部门的权力，如因游客自身原因或因提供材料存在问题不能及时办理签证或签注，以及被有关部门拒发签证或签注，不准出入境而影响行程的，签证费及其他费用损失由游客自行承担。/n4、为了不耽误您的行程，请您在国际航班起飞前180分钟到达机场办理登机以及出入境相关手续；如涉及海外国内段行程，请您在航班起飞前60分钟到达机场办理登机手续。/n5、出团通知最晚于出团前3个工作日发送，若能提前确定，我们将会第一时间通知您。/n6、团队游览中不允许擅自离团（自由活动除外），如有不便敬请谅解。"}];
    }
    return _costActivityInfos;
}

@end
