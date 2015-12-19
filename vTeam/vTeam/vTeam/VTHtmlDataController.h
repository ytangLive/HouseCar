//
//  VTHtmlDataController.h
//  vTeam
//
//  Created by shenyu on 13-9-2.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#import <vTeam/vTeam.h>

@interface VTHtmlDataController : VTDataController <UIWebViewDelegate>

@property(nonatomic,retain) IBOutlet UIWebView * webView;

- (void)setShowShadows:(BOOL)bShow;
//记录webView在startLoading时的contenSize以及contentOffset
- (void)vtHtmlDataContrllerSaveStartLoadingContentOffsetAndContentSizeWithWebView:(UIWebView *)webView;
//webViewFinishLoading时计算当前应该的offset偏移，用于改变webView字体大小改变时调用，跟vtHtmlDataContrllerSaveStartLoadingContentOffsetAndContentSizeWithWebView一起使用
- (void)vtHtmlDataContrllerSetFinishLoadingContentOffsetWithWebView:(UIWebView *)webView;

@end
