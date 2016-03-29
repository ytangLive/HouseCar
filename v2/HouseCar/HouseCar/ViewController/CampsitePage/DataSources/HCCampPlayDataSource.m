//
//  HCCampPlayDataSource.m
//  HouseCar
//
//  Created by tangyin on 15/12/8.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import "HCCampPlayDataSource.h"
#import "NSString+HCHelper.h"

@implementation HCCampPlayDataSource

- (void)setCampID:(NSString *)campID
{
    _campID = campID;
    
    if(_campID){
        [self.queryValues setValue:_campID forKey:@"id"];
    }
}

- (void)loadResultsData:(id)resultsData
{
    [super loadResultsData:resultsData];
    
    [self handleResultsData];
}

- (void)handleResultsData
{
    if(self.dataObjects && [self.dataObjects count] > 0){
        
        NSString *campPlaying = [self customFieldsDataForKey:self.dataObject fieldsSubKey:@"camp_main_playing"];
        NSString *campMainSubject = [self customFieldsDataForKey:self.dataObject fieldsSubKey:@"camp_main_subject"];
        NSString *location = [self customFieldsDataForKey:self.dataObject fieldsSubKey:@"location"];
        
        
        NSDictionary *locationDic = @{@"title":@"所在地",@"content":location};
        NSDictionary *themeDic = @{@"title":@"主题",@"content":campMainSubject};
        NSDictionary *playDic = @{@"title":@"玩法",@"content":campPlaying};
        [self.contentArr addObject:locationDic];
        [self.contentArr addObject:themeDic];
        [self.contentArr addObject:playDic];
        
        //评论
        
        NSArray *comments = [self.dataObject arrayValueForKey:@"comments"];
        if(comments && [comments count] > 0){
            for (NSDictionary *comment in comments) {
                NSString *userID = [comment stringValueForKey:@"id"];
                NSString *headImage = [comment stringValueForKey:@"url" defaultValue:@""];
                NSString *nickName = [comment stringValueForKey:@"name" defaultValue:@""];
                NSString *content = [[comment stringValueForKey:@"content" defaultValue:@""] stringFilterScanner];
                NSString *commentDate = [comment stringValueForKey:@"date" defaultValue:@""];
                NSDictionary *commentDic = @{@"userID":userID,
                                             @"headImage":headImage,
                                             @"nickname":nickName,
                                             @"comment":content,
                                             @"time":commentDate};
                
                [self.commentArr addObject:commentDic];
            }
        }
    }
}

#pragma mark - Property

- (NSMutableArray *)contentArr
{
    if(!_contentArr){
        _contentArr = [NSMutableArray array];
    }
    return _contentArr;
}

- (NSMutableArray *)commentArr
{
    if(!_commentArr){
        _commentArr = [NSMutableArray array];
    }
    return _commentArr;
}

@end
