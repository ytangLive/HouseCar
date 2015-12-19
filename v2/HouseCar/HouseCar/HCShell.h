/**
 *  SFShell.h
 *  VTTeamProject
 *
 *  Created by zhang hailong on 13-7-5.
 *  Copyright ytang.com. 2013年. All rights reserved.
 */


#import <UIKit/UIKit.h>
#import <vTeam/vTeam.h>
#import "HCContext.h"

@interface HCShell : VTShell<UIApplicationDelegate,HCContext,UIAlertViewDelegate
,IVTUplinkTaskDelegate> {
}

@property (strong, nonatomic) IBOutlet UIWindow *window;

-(id) initWithBundle:(NSBundle *) bundle;

@end
