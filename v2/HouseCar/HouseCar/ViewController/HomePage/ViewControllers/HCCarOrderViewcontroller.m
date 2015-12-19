//
//  HCCarOrderViewcontroller.m
//  HouseCar
//
//  Created by tangyin on 15/12/1.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import "HCCarOrderViewcontroller.h"
#import "HCFocusCell.h"

@interface HCCarOrderViewcontroller ()
 @property (nonatomic) UIEdgeInsets tableViewContentInset;
@end

@implementation HCCarOrderViewcontroller

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    
    NSDictionary *params = [self.url queryValues];
    NSString *jsonStr = [params stringValueForKey:@"detailInfo"];
    NSDictionary *detailInfo = nil;
    if(jsonStr){
        detailInfo = [VTJSON decodeText:jsonStr];
    }
    
    [_tableDataController setContext:self.context];
    self.dataSource.delegate = self.tableDataController;
    self.tableDataController.delegate = self;
    self.tableDataController.tableDatadelegate = self;
    self.tableDataController.detailInfo = detailInfo;
    
    self.tableDataController.dataUrlKey = @"carlist-interface";
    self.tableDataController.tableView.backgroundColor = DefaultBackgroundColor;
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(keyboardWillShown:) name:UIKeyboardWillShowNotification object:nil];
    
    [[NSNotificationCenter defaultCenter]  addObserver:self selector:@selector(keyboardWillHidden:) name:UIKeyboardWillHideNotification object:nil];
    
    _tableViewContentInset = _tableDataController.tableView.contentInset;
    
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    
    if(![_tableDataController.focusController.dataSource isLoading] && ![_tableDataController.focusController.dataSource isLoaded]){
        [_tableDataController reloadData];
    }else{
        [[_tableDataController tableView] reloadData];
    }
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark - VTTableDataControllerDelegate

- (void)vtTableDataController:(VTTableDataController *)dataController cell:(VTTableViewCell *)cell doAction:(id<IVTAction>)action
{
    
}

#pragma mark - HCCarBaseTableDataDelegate

- (void)HCCarOrderTableDataClickedWithData:(NSDictionary *)data
{
    
}

- (void)orderDaysViewTap:(id)sender
{
    NSDictionary *pageInfo = @{@"parentType":@"CarOrder"};
    
    [self openUrl:[NSURL URLWithString:@"carOrder/calendarSelect"
                         relativeToURL:self.url
                           queryValues:pageInfo] animated:YES];
}

- (void)carTakeWayViewTap:(id)sender
{
    
}

#pragma mark - KeyboardNotification

- (void) keyboardWillShown:(NSNotification *)notif
{

    NSDictionary *info = [notif userInfo];
    NSValue *value = [info objectForKey:UIKeyboardFrameBeginUserInfoKey];
    CGRect keyboardRect = [value CGRectValue];
    CGSize keyboardSize = keyboardRect.size;
    
    if(_tableDataController.tableView){
        [_tableDataController.tableView setContentInset:UIEdgeInsetsMake(_tableViewContentInset.top, _tableViewContentInset.left, keyboardSize.height, _tableViewContentInset.right)];
    }
    
    
}
- (void) keyboardWillHidden:(NSNotification *)notif
{
    if(_tableDataController.tableView){
        [_tableDataController.tableView setContentInset:_tableViewContentInset];
    }
}


//#pragma mark -
//- (void)doneButton:(UIButton *)btn{
//    //    NSLog(@"kongyu");
//    [self.txt resignFirstResponder];
//}

#pragma mark - submit Order

- (IBAction)submitOrder:(id)sender
{
    
}

@end
