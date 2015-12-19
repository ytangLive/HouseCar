/**
 *  SFViewController.h
 *  VTTeamProject
 *
 *  Created by zhang hailong on 13-7-5.
 *  Copyright ytang.com. 2013年. All rights reserved.
 */

#import <vTeam/vTeam.h>

#define HELP_IMAGEVIEW_TAG   1578

@interface HCViewController : VTViewController

@property(nonatomic,strong) IBOutlet UIBarButtonItem * leftButtonItem;
@property(nonatomic,strong) IBOutlet UIBarButtonItem * rightButtonItem;
@property(nonatomic,strong) IBOutlet UIButton * leftButton;
@property(nonatomic,strong) IBOutlet UIButton * rightButton;
@property(nonatomic,strong) id focusButton;
@property(nonatomic,assign) BOOL isTipHidden;

@end
