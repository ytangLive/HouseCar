//
//  HCActivityListViewController.m
//  HouseCar
//
//  Created by tangyin on 16/1/12.
//  Copyright © 2016年 sina.com. All rights reserved.
//

#import "HCActivityListViewController.h"
#import "HCFilterMenusView.h"
#import "HCSearchBarView.h"

@interface HCActivityListViewController ()<HCFilterMenusViewDelegate, HCSearchBarViewViewDelegate>

@end

@implementation HCActivityListViewController
{
    HCFilterMenusView *_searchMenu;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    
    [self initSearchMenuWithData:[NSArray arrayWithObjects:@{@"classifyID":@"City",@"classifyName":@"城市"},@{@"classifyID":@"Theme",@"classifyName":@"活动类型"}, nil]];
    
    [_tableDataController setContext:self.context];
    _tableDataController.delegate = self;
    _dataSource.delegate = _tableDataController;
    _tableDataController.tableView.backgroundColor = DefaultBackgroundColor;
    [_tableDataController topLoadingViewWithStyle:SFLoadingViewStyle_ActivityIndicatorStockAnimaiton];
    [_tableDataController bottomLoadingViewWithStyle:SFLoadingViewStyle_ActivityIndicatorStockAnimaiton];
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
    NSString * actionName =[sender actionName];
    
    if([actionName isEqualToString:@"filterCar"]){
        
        [self showSearchMenu];
    }else{
        [self openUrl:[NSURL URLWithString:[sender userInfo]
                             relativeToURL:self.url
                               queryValues:nil] animated:YES];
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
        
        [self openUrl:[NSURL URLWithString:@"root:///root/tab/activityList/activityDetail"
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
        UIView *belowView = [[UIView alloc] initWithFrame:CGRectMake(0, 0,
                                                                     self.view.bounds.size.width, self.view.bounds.size.height)];
        belowView.backgroundColor = [UIColor blackColor];
        belowView.alpha = 0.5;
        belowView.tag = 9999;
        [belowView addGestureRecognizer:[[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(hiddenSearchMenu)]];
        [self.view insertSubview:belowView belowSubview:_searchMenu];
        [UIView animateWithDuration:0.3 animations:^{
            _searchMenu.frame = CGRectMake(80 ,0 , _searchMenu.frame.size.width, _searchMenu.frame.size.height);
        } completion:NULL];
    }
}

- (void)hiddenSearchMenu
{
    if(!_searchMenu.hidden){
        [UIView animateWithDuration:0.3 animations:^{
            _searchMenu.frame = CGRectMake(KScreenWidth ,0 , _searchMenu.frame.size.width, _searchMenu.frame.size.height);
        } completion:^(BOOL finished) {
            _searchMenu.hidden = YES;
            UIView *belowView = [self.view viewWithTag:9999];
            [belowView removeFromSuperview];
        }];
    }
}

- (void)initSearchMenuWithData:(NSArray *)menuData
{
    _searchMenu = [[HCFilterMenusView alloc] initWithFrame:CGRectMake(KScreenWidth ,0 ,
                                                                      KScreenWidth - 80,
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
