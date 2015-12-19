//
//  SFGuideViewController.m
//  VTTeamProject
//
//  Created by zhang hailong on 13-7-31.
//  Copyright (c) 2013年 ytang.com. All rights reserved.
//

#import "HCGuideViewController.h"

@interface HCGuideViewController ()
{
    NSInteger curPage;
}

@property (weak, nonatomic) IBOutlet UIView *lastView;

@end

@implementation HCGuideViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
}

-(void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    int pages = round(self.scrollview.contentSize.width/self.scrollview.bounds.size.width);
    self.pageControl.numberOfPages = pages;
    [self setCurrentPage:0];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)scrollViewDidScroll:(UIScrollView *)scrollView{
    
    if(scrollView.contentOffset.x + scrollView.bounds.size.width > scrollView.contentSize.width){
        [scrollView setDelegate:nil];
        [self doFinishAction:nil];
    }
    else
    {
        CGRect bounds = scrollView.bounds;
        int page = (scrollView.contentOffset.x+CGRectGetWidth(bounds)/4) / (CGRectGetWidth(bounds));
        if (page<self.pageControl.numberOfPages) {
            if (page!=curPage) {
                curPage = page;
                self.pageControl.currentPage = page;
            }
        }
    }
}

-(IBAction) doFinishAction:(id)sender{
    [(id<HCContext>)self.context setGuided:YES];
    [(id<HCContext>)self.context toRootViewController];
}

- (void)viewDidUnload {
    [super viewDidUnload];
}


- (void)pageControlValueChanged:(id)sender {
    
    NSInteger page = self.pageControl.currentPage;
    
    if (curPage!=page) {
        CGRect bounds = self.scrollview.bounds;
        curPage = page;
        [self.scrollview setContentOffset:CGPointMake(CGRectGetWidth(bounds) * page, 0) animated:YES];
    }
    
}

-(void)setScrollViewPage:(NSInteger)page
{
    CGRect bounds = self.scrollview.bounds;
    self.scrollview.contentOffset = CGPointMake(page*CGRectGetWidth(bounds), 0) ;
}

-(void)setCurrentPage:(NSInteger)page
{
    curPage = page;
    [self setScrollViewPage:page];
    self.pageControl.currentPage = page;
}


@end
