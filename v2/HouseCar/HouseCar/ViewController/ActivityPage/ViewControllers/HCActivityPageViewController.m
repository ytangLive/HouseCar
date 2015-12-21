//
//  HCActivityPageViewController.m
//  HouseCar
//
//  Created by tangyin on 15/12/8.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import "HCActivityPageViewController.h"
#import "HCFilterMenusView.h"
#import "HCSearchBarView.h"

@interface HCActivityPageViewController () <HCFilterMenusViewDelegate, HCSearchBarViewViewDelegate>

@end

@implementation HCActivityPageViewController
{
    HCFilterMenusView *_searchMenu;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    
    [self initSearchMenuWithData:[NSArray arrayWithObjects:@{@"classifyID":@"City",@"classifyName":@"城市"},@{@"classifyID":@"Theme",@"classifyName":@"活动类型"}, nil]];
    
    [_tableDataController setContext:self.context];
    self.dataSource.delegate = self.tableDataController;
    self.tableDataController.delegate = self;
    self.tableDataController.tableView.backgroundColor = DefaultBackgroundColor;

}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    
    if(![_tableDataController.dataSource isLoading] && ![_tableDataController.dataSource isLoaded]){
        [_tableDataController reloadData];
    }else{
        [[_tableDataController tableView] reloadData];
    }
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)doAction:(id)sender
{
    [super doAction:sender];
    
    NSString * actionName =[sender actionName];
    
    if([actionName isEqualToString:@"filterCar"]){
        
        [self showSearchMenu];
    }
}

#pragma mark - VTTableDataControllerDelegate

- (void)vtTableDataController:(VTTableDataController *)dataController cell:(VTTableViewCell *)cell doAction:(id<IVTAction>)action
{
    NSString * actionName =[action actionName];
    if([actionName isEqualToString:@"filterCar"]){
        
        [self showSearchMenu];
        
    }else  if([actionName isEqualToString:@"cellSelected"]){
        NSDictionary *pageInfo = nil;
        
        [self openUrl:[NSURL URLWithString:[action userInfo]
                             relativeToURL:self.url
                               queryValues:pageInfo] animated:YES];
        
    }
    
}

#pragma mark - ViewAction

- (void)showSearchMenu
{
    if(_searchMenu.hidden){
        _searchMenu.hidden = NO;
        [_searchMenu.tableView reloadData];
        UIView *belowView = [[UIView alloc] initWithFrame:CGRectMake(0, KOffOriginalY,
                                                                     self.view.bounds.size.width, self.view.bounds.size.height)];
        belowView.backgroundColor = [UIColor blackColor];
        belowView.alpha = 0.5;
        belowView.tag = 9999;
        [belowView addGestureRecognizer:[[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(hiddenSearchMenu)]];
        [self.view insertSubview:belowView belowSubview:_searchMenu];
        [UIView animateWithDuration:0.3 animations:^{
            _searchMenu.frame = CGRectMake(100 ,KOffOriginalY , _searchMenu.frame.size.width, _searchMenu.frame.size.height);
        } completion:NULL];
    }
}

- (void)hiddenSearchMenu
{
    if(!_searchMenu.hidden){
        [UIView animateWithDuration:0.3 animations:^{
            _searchMenu.frame = CGRectMake(KScreenWidth ,KOffOriginalY , _searchMenu.frame.size.width, _searchMenu.frame.size.height);
        } completion:^(BOOL finished) {
            _searchMenu.hidden = YES;
            UIView *belowView = [self.view viewWithTag:9999];
            [belowView removeFromSuperview];
        }];
    }
}

- (void)initSearchMenuWithData:(NSArray *)menuData
{
    _searchMenu = [[HCFilterMenusView alloc] initWithFrame:CGRectMake(KScreenWidth ,KOffOriginalY ,
                                                                      KScreenWidth - 100.0f,
                                                                      self.view.frame.size.height)];
    _searchMenu.autoresizingMask = UIViewAutoresizingFlexibleHeight;
    _searchMenu.delegate = self;
    [_searchMenu setContext:self.context];
    _searchMenu.hidden = YES;
    _searchMenu.filterMenus = menuData;
    [self.view addSubview:_searchMenu];
}

#pragma mark - HCFilterMenusViewDelegate

- (void)confirmSelectedItems:(NSDictionary *)selectedItems
{
    
}

- (void)openCityList
{
    NSDictionary *pageInfo = @{@"parentType":@"ActivityPage"};
    [self openUrl:[NSURL URLWithString:@"present://root/cityList"
                         relativeToURL:self.url
                           queryValues:pageInfo] animated:YES];
}

#pragma mark - HCSearchBarViewViewDelegate

- (void)textFieldDidBeginEditing:(UITextField *)textField
{
    [textField resignFirstResponder];
    [self openUrl:[NSURL URLWithString:@"root:///root/tab/searchpage"
                         relativeToURL:self.url
                           queryValues:nil] animated:YES ];
}

@end