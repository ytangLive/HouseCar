//
//  HCCampHomePageViewController.m
//  HouseCar
//
//  Created by tangyin on 15/12/8.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import "HCCampHomePageViewController.h"
#import "HCFilterMenusView.h"
#import "HCSearchBarView.h"

@interface HCCampHomePageViewController () <HCFilterMenusViewDelegate, HCSearchBarViewViewDelegate,HCMenusSwitchViewDelegate>

@property (weak, nonatomic) IBOutlet UILabel *titleLabel;

@end

@implementation HCCampHomePageViewController

{
    NSString *_pageType;
    HCFilterMenusView *_searchMenu;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    
    self.titleLabel.text = @"";
    
    [self initSearchMenuWithData:[NSArray arrayWithObjects:@{@"classifyID":@"City",@"classifyName":@"城市"},@{@"classifyID":@"Theme",@"classifyName":@"主题"},@{@"classifyID":@"PlayWay",@"classifyName":@"玩法"}, nil]];
    
//    UITableViewCell *headerCell = [self getHeaderCellWidthTitle:@"营地游玩"];
//    self.tableDataController.headerCells = @[headerCell];
    
    
    [_tableDataController setContext:self.context];
    self.dataSource.delegate = self.tableDataController;
    self.tableDataController.delegate = self;
    self.tableDataController.tableDatadelegate = self;
    self.tableDataController.tableView.backgroundColor = DefaultBackgroundColor;
    
    self.tableDataController.dataUrlKey = @"carlist-interface";
    
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
        id dataItem = ((VTTableViewCell *) cell).dataItem;
        NSDictionary *pageInfo = nil;
        if([_pageType isEqualToString:@"carBook"]){
            
            NSDictionary *detailInfo = @{@"title":[dataItem stringValueForKey:@"title"], @"custom_fields":[dataItem dictionaryValueForKey:@"custom_fields"]};
            pageInfo = @{@"detailInfo":[VTJSON encodeObject:detailInfo],@"htmlContent" : [dataItem stringValueForKey:@"content"]};
            
        }else if([_pageType isEqualToString:@"campPlay"]){
            NSString *campID = [dataItem stringValueForKey:@"campID"];
            NSString *campTitle = [dataItem stringValueForKey:@"title"];
            if(campID && [campID length] > 0){
                pageInfo = @{@"campID":campID, @"campTitle":campTitle};
            }
            
        }
        
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
                                                                      KScreenWidth - 80.0f,
                                                                      self.view.frame.size.height)];
    _searchMenu.autoresizingMask = UIViewAutoresizingFlexibleHeight;
    _searchMenu.delegate = self;
    [_searchMenu setContext:self.context];
    _searchMenu.hidden = YES;
    _searchMenu.filterMenus = menuData;
    [self.view addSubview:_searchMenu];
}

#pragma mark - HCCarBaseTableDataDelegate

- (void)HCCampHomeTableDataClickedWithData:(NSDictionary *)data
{
    
}

#pragma mark - HCFilterMenusViewDelegate

- (void)confirmSelectedItems:(NSDictionary *)selectedItems
{
    
}

- (void)openCityList
{
    NSDictionary *pageInfo = @{@"parentType":@"CampPage"};
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


- (UITableViewCell *)getHeaderCellWidthTitle:(NSString *)title
{
    UITableViewCell *headerCell = [[UITableViewCell alloc] initLineWithFrame:CGRectMake(0, 0, KScreenWidth, 50) color:[UIColor clearColor]];
    CGRect lineRect = CGRectMake(15, 16, 5, 20);
    UIView *lineView = [UIView initTitleLineWidthFrame:lineRect];
    UIFont *labelFont = [UIFont systemFontOfSize:15];
    CGSize textSize = [title VTSizeWithFont:labelFont constrainedToSize:CGSizeMake(CGFLOAT_MAX, lineRect.size.height) lineBreakMode:NSLineBreakByWordWrapping];
    UILabel *titleLabel = [[UILabel alloc] initWithFrame:CGRectMake(CGRectGetMaxX(lineRect) + 6, lineRect.origin.y, textSize.width, lineRect.size.height)];
    titleLabel.font = labelFont;
    titleLabel.lineBreakMode = NSLineBreakByWordWrapping;
    titleLabel.textColor = COLOR_HEX(0x666666);
    titleLabel.text = title;
    [headerCell.contentView addSubview:lineView];
    [headerCell.contentView addSubview:titleLabel];
    
    return headerCell;
}

- (void)menusDoAction:(NSUInteger)buttonIndex
{
    //[_tableDataController reloadData];
}

@end
