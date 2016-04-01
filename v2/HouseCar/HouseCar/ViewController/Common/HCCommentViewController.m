//
//  HCCommentViewController.h
//  HourseCar
//
//  Created by Tang yin on 16-3-31.
//  Copyright (c) 2016年 ytang.com. All rights reserved.
//

#import "HCCommentViewController.h"
#import "HCAlertLoadingView.h"
#import "UIDevice+IPAddress.h"
#import "HCAlertView.h"
#import "HCSendStatusView.h"
#import "SVProgressHUD.h"

@interface HCCommentViewController ()
@property(nonatomic,strong) HCAlertLoadingView * loadingView;
@property(nonatomic,strong) HCSendStatusView *sendingStatusView;
@end

@implementation HCCommentViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

-(void)dealloc
{
    [_refCmtTask setDelegate:nil];
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    [_cmtTextView becomeFirstResponder];
    _sendBtn.enabled = 0;
    
    self.userInfo = [self.url queryValues];
    _commentType = [_userInfo intValueForKey:@"commentType"];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)viewDidUnload {
    [self setCmtTextView:nil];
    [self setContentView:nil];
    [self setKeyboardController:nil];
    [self setSendBtn:nil];
    [super viewDidUnload];
}

-(void) viewDidDisappear:(BOOL)animated{
    [super viewDidDisappear:animated];
    
    [_loadingView closeAnimated:animated];
    self.loadingView = nil;
}

- (HCSendStatusView *)sendingStatusView
{
    if(!_sendingStatusView){
        _sendingStatusView = [[HCSendStatusView alloc] initWithImage:@"sending.png" title:@"正在发送..."];
    }
    return _sendingStatusView;
}

-(void) vtKeyboardController:(VTKeyboardController * )controller willChangedFrame:(CGRect) frame
{
    [UIView beginAnimations:nil context:nil];
    [UIView setAnimationDuration:0.25];
    CGRect _selfFrame = self.view.frame;
    CGRect _contentFrame = _contentView.frame;
    _contentFrame.size.height = _selfFrame.size.height-frame.size.height-44;
    _contentView.frame = _contentFrame;
    [UIView commitAnimations];
}

-(void)doSubmitComment
{
    NSString *strContent = [_cmtTextView.text stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
    if (strContent.length > 0) {
        
        [_loadingView closeAnimated:NO];
        self.loadingView = [[HCAlertLoadingView alloc] init];
        [_loadingView showAnimated:YES];
        
        if (_commentType == HCCommentTypeCamp) {
            HCCampCommentTask * cmtTask = [[HCCampCommentTask alloc] init];
            [cmtTask setContent:strContent];
            [cmtTask setPostID:[_userInfo stringValueForKey:@"postID"]];
            [cmtTask setUserName:[_userInfo stringValueForKey:@"userName"]];
            [cmtTask setEmail:[_userInfo stringValueForKey:@"email"]];
            [cmtTask setDelegate:self];
            self.refCmtTask = cmtTask;
            
            [self.context handle:@protocol(IHCCampCommentTask) task:cmtTask priority:0];
        }
        else if (_commentType == HCCommentTypeActivity) {
//            SFBarCommentTask * cmtTask = [[SFBarCommentTask alloc] init];
//            [cmtTask setContent:strContent];
//            [cmtTask setBid:[_userInfo valueForKey:@"bid"]];
//            [cmtTask setTid:[_userInfo valueForKey:@"tid"]];
//            [cmtTask setIp:[[UIDevice currentDevice] getIPAddress]];
//            [cmtTask setDelegate:self];
//            self.refCmtTask = cmtTask;
//            
//            [self.context handle:@protocol(ISFBarCommentTask) task:cmtTask priority:0];
        }
        
        //[SVProgressHUD showProgress:1.2 status:@"正在发送..."];

        [self.sendingStatusView show:YES];
        
        [_cmtTextView resignFirstResponder];
        //[self openUrl:[NSURL URLWithString:@"." relativeToURL:self.url] animated:YES];
    }
    else {
        HCAlertView * alertView = [[HCAlertView alloc] initWithTitle:@"请检查评论内容"];
        [alertView showDuration:3.0];
    }
}

- (void)textViewDidChange:(UITextView *)textView
{
    if (_cmtTextView.text.length > 0) {
        _sendBtn.enabled = 1;
    }
    else {
        _sendBtn.enabled = 0;
    }
}

- (IBAction)sendAction:(id)sender
{
    [self doSubmitComment];
}

-(void) vtUploadTask:(id<IVTUplinkTask>) uplinkTask didSuccessResults:(id) results forTaskType:(Protocol *) taskType
{
    [_loadingView closeAnimated:YES];
    [self.sendingStatusView close:YES];
    self.loadingView = nil;
    [_cmtTextView resignFirstResponder];
    HCAlertView * alertView = [[HCAlertView alloc] initWithTitle:@"评论已发布"];
    [alertView showDuration:2.0];
    [self openUrl:[NSURL URLWithString:@"." relativeToURL:self.url] animated:YES];
}

-(void) vtUploadTask:(id<IVTUplinkTask>) uplinkTask didFailWithError:(NSError *)error forTaskType:(Protocol *)taskType
{
    [_loadingView closeAnimated:YES];
    [self.sendingStatusView close:YES];
    self.loadingView = nil;
    HCAlertView * alertView = [[HCAlertView alloc] initWithTitle:[NSString stringWithFormat:@"评论发布失败：%@",[error message]]];
    [alertView showDuration:2.0];
}

@end
