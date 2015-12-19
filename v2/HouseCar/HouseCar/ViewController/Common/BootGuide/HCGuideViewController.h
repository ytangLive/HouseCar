//
//  SFGuideViewController.h
//  VTTeamProject
//
//  Created by zhang hailong on 13-7-31.
//  Copyright (c) 2013年 ytang.com. All rights reserved.
//

#import "HCViewController.h"

@interface HCGuideViewController : HCViewController<UIScrollViewDelegate>

@property(nonatomic,strong) IBOutlet UIScrollView * scrollview;
@property(nonatomic,strong) IBOutlet UIView * containerView;
@property(nonatomic,strong) IBOutlet UIPageControl * pageControl;

- (IBAction)pageControlValueChanged:(id)sender;

-(IBAction) doFinishAction:(id)sender;


@end
