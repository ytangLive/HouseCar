//
//  HCCampPlayDataSource.m
//  HouseCar
//
//  Created by tangyin on 15/12/8.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import "HCCampPlayDataSource.h"

@implementation HCCampPlayDataSource

- (void)loadResultsData:(id)resultsData
{
    [super loadResultsData:resultsData];
    
    
}

#pragma mark - Property

- (NSMutableArray *)contentArr
{
    if(!_contentArr){
        _contentArr = [NSMutableArray array];
        NSDictionary *locationDic = @{@"title":@"所在地",@"content":@"北京市海淀区"};
        NSDictionary *themeDic = @{@"title":@"主题",@"content":@"乡村型"};
        NSDictionary *playDic = @{@"title":@"玩法",@"content":@"游泳 烧烤 住宿 酒吧 摄影"};
        [_contentArr addObject:locationDic];
        [_contentArr addObject:themeDic];
        [_contentArr addObject:playDic];
    }
    return _contentArr;
}

- (NSMutableArray *)commentArr
{
    if(!_commentArr){
        _commentArr = [NSMutableArray array];
        NSDictionary *commentDic = @{@"headImage":@"",@"nickname":@"seed",@"comment":@"好玩吗",@"time":@"2015.12.10 09:00"};
        NSDictionary *commentDic1 = @{@"headImage":@"",@"nickname":@"seed1",@"comment":@"不错",@"time":@"2015.12.10 09:09"};
        [_commentArr addObject:commentDic];
        [_commentArr addObject:commentDic1];
    }
    return _commentArr;
}

@end
