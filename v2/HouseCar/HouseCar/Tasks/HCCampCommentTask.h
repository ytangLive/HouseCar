//
//  HCCampCommentTask.h
//  HourseCar
//
//  Created by Tang yin on 16-3-31.
//  Copyright (c) 2016年 ytang.com. All rights reserved.
//

#import <vTeam/vTeam.h>

@protocol IHCCampCommentTask <IVTUplinkTask>

@property (nonatomic,retain) NSString *postID;      //营地id
@property (nonatomic,retain) NSString *userName;    //用户名
@property (nonatomic,retain) NSString *email;       //用户的邮箱地址
@property (nonatomic,retain) NSString *content;     //评论内容

@end

@interface HCCampCommentTask : VTUplinkTask<IHCCampCommentTask>

@end
