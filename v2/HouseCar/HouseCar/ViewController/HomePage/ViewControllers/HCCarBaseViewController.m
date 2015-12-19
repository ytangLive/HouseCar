//
//  HCCarBookViewController.m
//  HouseCar
//
//  Created by tangyin on 15/11/23.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import "HCCarBaseViewController.h"
#import "HCFilterMenusView.h"
#import "HCSearchBarView.h"

@interface HCCarBaseViewController () <HCFilterMenusViewDelegate, HCSearchBarViewViewDelegate>
@property (weak, nonatomic) IBOutlet UILabel *titleLabel;
@property (weak, nonatomic) IBOutlet UIDatePicker *datePicker;
@property (weak, nonatomic) IBOutlet HCSearchBarView *searchBar;
@end

@implementation HCCarBaseViewController
{
    NSString *_pageType;
    HCFilterMenusView *_searchMenu;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    NSDictionary *params = [self.url queryValues];
    _pageType  = [params stringValueForKey:@"pageType"];
    self.searchBar.hidden = YES;
    
    if([_pageType isEqualToString:@"carBook"]){
        
        self.titleLabel.text = @"房车预定";
        
        [self initSearchMenuWithData:[NSArray arrayWithObjects:@{@"classifyID":@"TCDD",@"classifyName":@"提车地点"},@{@"classifyID":@"TCFL",@"classifyName":@"分类"},@{@"classifyID":@"FCLX",@"classifyName":@"房车类型"}, nil]];
        
    }else if([_pageType isEqualToString:@"campPlay"]){
        
        self.titleLabel.text = @"";
        self.searchBar.hidden = NO;
        
        [self initSearchMenuWithData:[NSArray arrayWithObjects:@{@"classifyID":@"City",@"classifyName":@"城市"},@{@"classifyID":@"Theme",@"classifyName":@"主题"},@{@"classifyID":@"PlayWay",@"classifyName":@"玩法"}, nil]];
        
        UITableViewCell *headerCell = [self getHeaderCellWidthTitle:@"营地游玩"];
        self.tableDataController.headerCells = @[headerCell];
        
    }else if([_pageType isEqualToString:@"travelGuide"]){
        self.titleLabel.text = @"游记攻略";
    }else if([_pageType isEqualToString:@"orderManage"]){
        self.titleLabel.text = @"订单管理";
    }
    
    
    [_tableDataController setContext:self.context];
    self.dataSource.delegate = self.tableDataController;
    self.tableDataController.delegate = self;
    self.tableDataController.tableDatadelegate = self;
    self.tableDataController.pageType = _pageType;
    
    self.tableDataController.dataUrlKey = @"carlist-interface";
    
    _datePicker.backgroundColor = DefaultBackgroundColor;
    
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
    if([actionName isEqualToString:@"takeCar"]){
        if(_datePicker.hidden){
            _datePicker.hidden = NO;
            [_datePicker setDate:self.tableDataController.takeDate];
            UIView *backgroundView = [[UIView alloc] initWithFrame:self.view.frame];
            backgroundView.backgroundColor = [UIColor blackColor];
            backgroundView.alpha = 0.5;
            //标记为点击取车按钮打开的时间控件
            _datePicker.tag = 101;
            [backgroundView addGestureRecognizer:[[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(viewTapped:)]];
            [[self.tableDataController.tableView superview] insertSubview:backgroundView belowSubview:_datePicker];
        }
    }else if([actionName isEqualToString:@"returnCar"]){
        if(_datePicker.hidden){
            _datePicker.hidden = NO;
            [_datePicker setDate:self.tableDataController.returnDate];
            UIView *backgroundView = [[UIView alloc] initWithFrame:self.view.frame];
            backgroundView.backgroundColor = [UIColor blackColor];
            backgroundView.alpha = 0.5;
            //标记为点击还车按钮打开的时间控件
            _datePicker.tag = 102;
            [backgroundView addGestureRecognizer:[[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(viewTapped:)]];
            [[self.tableDataController.tableView superview] insertSubview:backgroundView belowSubview:_datePicker];
        }
    }else if([actionName isEqualToString:@"filterCar"]){
        
        [self showSearchMenu];
        
    }else  if([actionName isEqualToString:@"cellSelected"]){
        id dataItem = ((VTTableViewCell *) cell).dataItem;
        NSDictionary *pageInfo = nil;
        if([_pageType isEqualToString:@"carBook"]){
            
            NSDictionary *detailInfo = @{@"title":[dataItem stringValueForKey:@"title"], @"custom_fields":[dataItem dictionaryValueForKey:@"custom_fields"]};
            pageInfo = @{@"detailInfo":[VTJSON encodeObject:detailInfo],@"htmlContent" : [dataItem stringValueForKey:@"content"]};
            
        }else if([_pageType isEqualToString:@"campPlay"]){
            
            
            
        }
        
        [self openUrl:[NSURL URLWithString:[action userInfo]
                                         relativeToURL:self.url
                                           queryValues:pageInfo] animated:YES];
        
    }
    
}

#pragma mark - ViewAction

- (IBAction)datePickerValueChanged:(id)sender {
    NSDate *pickerDate = [sender date];
    if([pickerDate compare:_tableDataController.defaultDate] == NSOrderedAscending){
        if(_datePicker.tag == 101){
            [sender setDate:_tableDataController.takeDate];
        }else if(_datePicker.tag == 102){
            [sender setDate:_tableDataController.returnDate];
        }
    }
}

- (void)viewTapped:(UITapGestureRecognizer *)gesture
{
    if(gesture.view){
        [gesture.view removeFromSuperview];
        _datePicker.hidden = YES;
        NSDate *selectedDate = _datePicker.date;
        if(_datePicker.tag == 101){
             [_tableDataController setTakeDate:selectedDate];
        }else if(_datePicker.tag == 102){
             [_tableDataController setReturnDate:selectedDate];
        }
        [_tableDataController.tableView reloadData];
    }
}

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

#pragma mark - HCCarBaseTableDataDelegate

- (void)HCCarBaseTableDataClickedWithData:(NSDictionary *)data
{
    
}

#pragma mark - HCFilterMenusViewDelegate

- (void)confirmSelectedItems:(NSDictionary *)selectedItems
{
    
}

- (void)openCityList
{
    NSDictionary *pageInfo = @{@"parentType":@"CartBook"};
    [self openUrl:[NSURL URLWithString:@"present://root/cityList"
                         relativeToURL:self.url
                           queryValues:pageInfo] animated:YES];
}


- (UITableViewCell *)getHeaderCellWidthTitle:(NSString *)title
{
    UITableViewCell *headerCell = [[UITableViewCell alloc] initLineWithFrame:CGRectMake(0, 0, KScreenWidth, 50) color:[UIColor whiteColor]];
    CGRect lineRect = CGRectMake(10, 16, 6, 20);
    UIView *lineView = [UIView initTitleLineWidthFrame:lineRect];
    UIFont *labelFont = [UIFont boldSystemFontOfSize:14];
    CGSize textSize = [title VTSizeWithFont:labelFont constrainedToSize:CGSizeMake(CGFLOAT_MAX, lineRect.size.height) lineBreakMode:NSLineBreakByWordWrapping];
    UILabel *titleLabel = [[UILabel alloc] initWithFrame:CGRectMake(CGRectGetMaxX(lineRect) + 5, lineRect.origin.y, textSize.width, lineRect.size.height)];
    titleLabel.font = labelFont;
    titleLabel.lineBreakMode = NSLineBreakByWordWrapping;
    titleLabel.textColor = COLOR_HEX(0x666666);
    titleLabel.text = title;
    [headerCell.contentView addSubview:lineView];
    [headerCell.contentView addSubview:titleLabel];
    
    return headerCell;
}

@end
