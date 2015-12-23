//
//  HCSearchViewController.m
//  HouseCar
//
//  Created by tangyin on 15/12/11.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import "HCSearchViewController.h"
#import "HCHistorySearchAddTask.h"

@interface HCSearchViewController ()

@end

@implementation HCSearchViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    
    [_tableDataController setContext:self.context];
    self.dataSource.delegate = self.tableDataController;
    self.tableDataController.delegate = self;
    
    UITapGestureRecognizer *gesture = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(hiddenKeyBoard:)];
    gesture.cancelsTouchesInView = NO;
    [self.view addGestureRecognizer:gesture];
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

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    
    [_searchBarView.textField becomeFirstResponder];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)doAction:(id)sender
{
    NSString *actionName = [sender actionName];
    if([actionName isEqualToString:@"search"]){
        
    }else{
        
    }
    [super doAction:sender];
}

- (void)searchResult:(NSString *)searchKey
{
    NSString *searchNewKey = [searchKey stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
    if(searchNewKey){
        HCHistorySearchAddTask *addTask = [[HCHistorySearchAddTask alloc] init];
        addTask.searchKey = searchNewKey;
        
        [self.context handle:@protocol(IHCHistorySearchAddTask) task:addTask priority:0];
        
//        NSDictionary *queryValues = @{@"searchKey":searchNewKey};
//        [self openUrl:[NSURL URLWithString:@"root:///root/tab/searchpage"
//                             relativeToURL:self.url
//                               queryValues:queryValues] animated:YES ];
    }
}

- (void)hiddenKeyBoard:(UITapGestureRecognizer *)gesture
{
    [_searchBarView.textField resignFirstResponder];
}

#pragma mark - HCSearchBarViewViewDelegate

- (void)searchBtnClick:(HCSearchBarView *)sender searchKey:(NSString *)searchStr
{
    [self searchResult:searchStr];
}

@end
