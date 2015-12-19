//
//  HCCommonWebViewController.m
//  HouseCar
//
//  Created by tangyin on 15/12/1.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import "HCCarBaseDetailViewController.h"
#import "HCJavaScriptCallBack.h"
#import "EasyJSWebView.h"

@interface HCCarBaseDetailViewController ()<UIWebViewDelegate>

@property (weak, nonatomic) IBOutlet EasyJSWebView *webView;

@end

@implementation HCCarBaseDetailViewController
{
    NSString *_detailInfo;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    
    NSDictionary *params = [self.url queryValues];
   _detailInfo = [params stringValueForKey:@"detailInfo"];
    
    NSString *htmlContent = [params stringValueForKey:@"htmlContent"];
    
    _webView.backgroundColor = DefaultBackgroundColor;
    _webView.delegate = self;
    [_webView setScalesPageToFit:YES];
    HCJavaScriptCallBack *javaScriptCallBack = [[HCJavaScriptCallBack alloc] initWithSender:self];
    [_webView addJavascriptInterfaces:javaScriptCallBack WithName:@"jsCallback"];
    //[_webView loadRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:_requestUrl]]];
    if(htmlContent){
       // NSString * newHtmlcontent = [NSString stringWithFormat:@"<div id=\"webview_content_wrapper\">%@</div>", contentHtml];
        [_webView loadHTMLString:htmlContent baseURL:[[NSBundle mainBundle] bundleURL]];
    }
    [self.view addSubview:_webView];

}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)doAction:(id)sender
{
    
    NSString * actionName =[sender actionName];
    NSDictionary *pageInfo = nil;
    if([actionName isEqualToString:@"carBook"]){
        
        pageInfo = @{@"pageType":@"carBook",@"detailInfo":_detailInfo};
        
        [self openUrl:[NSURL URLWithString:[sender userInfo]
                             relativeToURL:self.url
                               queryValues:pageInfo] animated:YES];
        
    }else if([actionName isEqualToString:@"shareInfo"]){
        [UMSocialSnsService presentSnsIconSheetView:self
                                             appKey:UMAppKey
                                          shareText:@"你要分享的文字"
                                         shareImage:[UIImage imageNamed:@"icon.png"]
                                    shareToSnsNames:[NSArray arrayWithObjects:UMShareToSina,UMShareToWechatSession,UMShareToQQ,nil]
                                           delegate:nil];
    }else{
        [self openUrl:[NSURL URLWithString:[sender userInfo]
                             relativeToURL:self.url
                               queryValues:pageInfo] animated:YES];
    }
}

#pragma mark - Property

- (BOOL)webView:(UIWebView *)webView shouldStartLoadWithRequest:(NSURLRequest *)request navigationType:(UIWebViewNavigationType)navigationType
{
    return YES;
}

- (void)webViewDidFinishLoad:(UIWebView *)webView
{
    NSString *title = [_webView stringByEvaluatingJavaScriptFromString:@"document.title"];
    _titleBar.titleLabel.text = title;
    
   
}

- (void)webViewDidStartLoad:(UIWebView *)webView
{
    
}

@end
