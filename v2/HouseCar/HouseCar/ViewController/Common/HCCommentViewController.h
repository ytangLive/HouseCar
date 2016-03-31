//
//  HCCommentViewController.h
//  HourseCar
//
//  Created by Tang yin on 16-3-31.
//  Copyright (c) 2016年 ytang.com. All rights reserved.
//

#import <vTeam/vTeam.h>
#import "HCCampCommentTask.h"

typedef int HCCommentType;

enum {
    HCCommentTypeCamp,          //营地
    HCCommentTypeActivity,      //活动
};

@interface HCCommentViewController : VTViewController<VTKeyboardControllerDelegate,IVTUplinkTaskDelegate,UITextViewDelegate>
@property (strong, nonatomic) IBOutlet VTKeyboardController *keyboardController;
@property (strong, nonatomic) IBOutlet UIView *contentView;
@property (strong, nonatomic) IBOutlet UITextView *cmtTextView;
@property (strong, nonatomic) IBOutlet VTButton *sendBtn;

@property (assign, nonatomic) HCCommentType commentType;
@property (strong, nonatomic) NSDictionary* userInfo;

@property (strong, nonatomic) id<IVTUplinkTask> refCmtTask;

- (IBAction)sendAction:(id)sender;

@end
