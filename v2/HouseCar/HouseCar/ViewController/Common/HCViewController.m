/**
 *  SFViewController.m
 *  VTTeamProject
 *
 *  Created by zhang hailong on 13-7-5.
 *  Copyright ytang.com. 2013年. All rights reserved.
 */


#import "HCViewController.h"

#import <QuartzCore/QuartzCore.h>



@interface HCViewController ()

@end


@implementation HCViewController

@synthesize focusButton = _focusButton;
@synthesize isTipHidden = _isTipHidden;

-(void) dealloc{
    
    NSLog(@"%@ dealloc",NSStringFromClass([self class]));

    
}

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
#ifdef __IPHONE_7_0
        if([self respondsToSelector:@selector(setAutomaticallyAdjustsScrollViewInsets:)]){
            [self setAutomaticallyAdjustsScrollViewInsets:NO];
        }
#endif
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view.

}


- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

-(void) helpTapGestureAction:(UITapGestureRecognizer *) gestureRecognizer {
    
    NSString * key = [NSString stringWithFormat:@"%@_Help",[self.config stringValueForKeyPath:@"help.key"]];
    
    NSUserDefaults * userDefaults = [NSUserDefaults standardUserDefaults];
    
    [userDefaults setValue:@true forKey:key];
    
    [userDefaults synchronize];
    
    UIView * v = [gestureRecognizer view];
    
    [UIView animateWithDuration:0.3 animations:^{
        
        [v setAlpha:0.0f];

    } completion:^(BOOL finished) {
       
        [v removeFromSuperview];
        
    }];
    
}

-(void) viewDidAppear:(BOOL)animated{
    [super viewDidAppear:animated];
    
    if ([self.context respondsToSelector:@selector(setFocusValue:forKey:)]) {
        [self.context setFocusValue:@"true" forKey:@"SFLaunchOverFlagKeywordsKey"];
    }
    
    if(!self.isTipHidden)
    {
        id help = [self.config objectValueForKey:@"help"];
        
        NSString * key = [NSString stringWithFormat:@"%@_Help",[self.config stringValueForKeyPath:@"help.key"]];
        
        NSUserDefaults * userDefaults = [NSUserDefaults standardUserDefaults];
        
        if(help && ! [[userDefaults valueForKey:key] boolValue] ){
            
            NSString * image = [help stringValueForKey:@"image"];
            
            CGSize screenSize = [[UIScreen mainScreen] bounds].size;
            
            if (screenSize.height == 667){
                image = [help stringValueForKeyPath:@"iPhone6.image" defaultValue:image];
            }else if(screenSize.height >= 568 ){
                image = [help stringValueForKeyPath:@"iPhone5.image" defaultValue:image];
            }
            
            if(image){
                
                UIWindow * window = [self.view window];
                
                UIImageView * imageView = [[UIImageView alloc] initWithFrame:window.bounds];
                
                [imageView setAutoresizingMask:UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight];
                
                [imageView setContentMode:UIViewContentModeScaleAspectFill];
                
                [imageView setClipsToBounds:YES];
                
                [imageView setImage:[UIImage imageNamed:image]];
                
                [imageView setAlpha:0.0f];
                
                UITapGestureRecognizer * tapGestureRecognizer = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(helpTapGestureAction:)];
                
                [tapGestureRecognizer setNumberOfTapsRequired:1];
                [tapGestureRecognizer setNumberOfTouchesRequired:1];
                
                [imageView setUserInteractionEnabled:YES];
                
                [imageView addGestureRecognizer:tapGestureRecognizer];
                
                imageView.tag = HELP_IMAGEVIEW_TAG;
                UIView *lastImageView = [window viewWithTag:HELP_IMAGEVIEW_TAG];
                if (lastImageView && [lastImageView isKindOfClass:[UIImageView class]]) {
                    [lastImageView removeFromSuperview];
                }
                
                [window addSubview:imageView];
                
                [UIView animateWithDuration:0.3 animations:^{
                    
                    [imageView setAlpha:1.0f];
                    
                }];
            }
        }
        else if(help){
            UIWindow * window = [self.view window];
            UIView *lastImageView = [window viewWithTag:HELP_IMAGEVIEW_TAG];
            if (lastImageView && [lastImageView isKindOfClass:[UIImageView class]]) {
                [lastImageView removeFromSuperview];
            }
        }

    }
    
}

-(UIBarButtonItem *) leftButtonItem{
    return self.navigationItem.leftBarButtonItem;
}

-(void) setLeftButtonItem:(UIBarButtonItem *)leftButtonItem{
    self.navigationItem.leftBarButtonItem = leftButtonItem;
}

-(UIBarButtonItem *) rightButtonItem{
    return self.navigationItem.rightBarButtonItem;
}

-(void) setRightButtonItem:(UIBarButtonItem *)rightButtonItem{
    self.navigationItem.rightBarButtonItem = rightButtonItem;
}

-(UIButton *) leftButton{
    return (UIButton *) self.navigationItem.leftBarButtonItem.customView;
}

-(void) setLeftButton:(UIButton *)leftButton{
    if(leftButton == nil){
        self.navigationItem.leftBarButtonItem = nil;
    }
    else{
        self.navigationItem.leftBarButtonItem = [[UIBarButtonItem alloc] initWithCustomView:leftButton];
    }
}

-(UIButton *) rightButton{
    return (UIButton *) self.navigationItem.rightBarButtonItem.customView;
}

-(void) setRightButton:(UIButton *)rightButton{
    if(rightButton == nil){
        self.navigationItem.rightBarButtonItem = nil;
    }
    else{
        self.navigationItem.rightBarButtonItem = [[UIBarButtonItem alloc] initWithCustomView:rightButton];
    }
}

-(void) setFocusButton:(UIButton *)focusButton{
    if(_focusButton != focusButton){
        if([_focusButton respondsToSelector:@selector(setSelected:)]){
            [_focusButton setSelected:NO];
        }
        _focusButton = focusButton;
        if([_focusButton respondsToSelector:@selector(setSelected:)]){
            [_focusButton setSelected:YES];
        }
    }
}

@end
