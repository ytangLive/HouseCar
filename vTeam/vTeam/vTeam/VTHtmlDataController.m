//
//  VTHtmlDataController.m
//  vTeam
//
//  Created by shenyue on 13-9-2.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#import "VTHtmlDataController.h"

@interface VTHtmlDataController()

@property(nonatomic, assign)CGPoint webViewStartLoadingPoint;//记录startLoading时webView的point
@property(nonatomic, assign)CGSize webViewStartLoadingContentSize;//记录startLoading时webView的contentOffset

@end

@implementation VTHtmlDataController
@synthesize webView = _webView;

-(void)dealloc
{
    [_webView setDelegate:nil];
    [_webView stopLoading];
    [_webView release];
    [super dealloc];
}

- (void)setShowShadows:(BOOL)bShow;
{
	// Reference: http://stackoverflow.com/questions/1074320/remove-uiwebview-shadow
    
	// 遍历UIScrollView隐藏阴影.
	for (UIView *inScrollView in [_webView subviews])
	{
		if ([inScrollView isKindOfClass:[UIImageView class]])
		{
			inScrollView.hidden = !bShow;
		}
	}
    
    for (UIView *inScrollView in [[_webView subviews][0] subviews])
	{
		if ([inScrollView isKindOfClass:[UIImageView class]])
		{
			inScrollView.hidden = !bShow;
		}
	}
}

- (void)vtHtmlDataContrllerSaveStartLoadingContentOffsetAndContentSizeWithWebView:(UIWebView *)webView
{
    self.webViewStartLoadingContentSize = webView.scrollView.contentSize;
    self.webViewStartLoadingPoint = webView.scrollView.contentOffset;
}

- (void)vtHtmlDataContrllerSetFinishLoadingContentOffsetWithWebView:(UIWebView *)webView
{
    if (self.webViewStartLoadingPoint.y != 0 && webView.scrollView.contentSize.height > webView.scrollView.frame.size.height) {
        CGFloat contentOffSetRatio = webView.scrollView.contentSize.height / self.webViewStartLoadingContentSize.height;
        CGFloat setScrollContentOffset = self.webViewStartLoadingPoint.y * contentOffSetRatio;
        if (setScrollContentOffset + webView.scrollView.frame.size.height > webView.scrollView.contentSize.height) {
                setScrollContentOffset = webView.scrollView.contentSize.height - webView.scrollView.frame.size.height;
        }
        webView.scrollView.contentOffset = CGPointMake(webView.scrollView.contentOffset.x, setScrollContentOffset);
    }
}

@end
