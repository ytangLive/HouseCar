//
//  HCHomePageViewController.m
//  HouseCar
//
//  Created by tangyin on 15/11/11.
//  Copyright © 2015年 ytang.com. All rights reserved.
//

#import "HCHomePageViewController.h"

@interface HCHomePageViewController ()
@property (weak, nonatomic) IBOutlet UILabel *cityLabel;
@property (strong, nonatomic) NSArray *citiesData;
@property (weak, nonatomic) IBOutlet HCSearchBarView *searchBarView;
@end

@implementation HCHomePageViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    
    [_tableDataController setContext:self.context];
    self.dataSource.delegate = self.tableDataController;
    self.tableDataController.delegate = self;
    self.tableDataController.tableDatadelegate = self;
    self.tableDataController.tableView.backgroundColor = TableViewBackgroundColor;
    [self.tableDataController topLoadingViewWithStyle:SFLoadingViewStyle_ActivityIndicatorStockAnimaiton];
    [self.tableDataController bottomLoadingViewWithStyle:SFLoadingViewStyle_ActivityIndicatorStockAnimaiton];
    
    self.tableDataController.dataUrlKey = @"carlist-interface";
    
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    
    if(_tableDataController){
        [_tableDataController reloadData];
    }
}

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    
    NSString *cityName = [(id<HCContext>)self.context focusValueForKey:@"cityName"];
    if(cityName && [cityName length] > 0){
        self.cityLabel.text = cityName;
    }

}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)positionTap:(id)sender {
    NSDictionary *pageInfo = @{@"parentType":@"HomePage"};
    
    [self openUrl:[NSURL URLWithString:@"root:///root/tab/cityList"
                         relativeToURL:self.url
                           queryValues:pageInfo] animated:YES];
}

- (IBAction)doAction:(id)sender
{
    NSString * actionName =[sender actionName];
    NSDictionary *pageInfo = nil;
    
    if([actionName isEqualToString:@"search"]){
        [self openUrl:[NSURL URLWithString:[sender userInfo]
                             relativeToURL:self.url
                               queryValues:pageInfo] animated:YES callBackDelegate:self];
    }else{
        [self openUrl:[NSURL URLWithString:[sender userInfo]
                             relativeToURL:self.url
                               queryValues:pageInfo] animated:YES];
    }
}

- (void)vtTableDataController:(VTTableDataController *)dataController cell:(VTTableViewCell *)cell doAction:(id<IVTAction>)action
{
    NSString * actionName =[action actionName];
    NSDictionary *pageInfo = nil;
    if([actionName isEqualToString:@"allActivities"]){
        
    }else  if([actionName isEqualToString:@"cellSelected"]){
        id dataItem = ((VTTableViewCell *) cell).dataItem;
      
        NSString *campID = [dataItem stringValueForKey:@"campID"];
        if(campID && [campID length] > 0){
            pageInfo = @{@"campID":campID};
        }
        
    }
    
    [self openUrl:[NSURL URLWithString:[action userInfo]
                         relativeToURL:self.url
                           queryValues:pageInfo] animated:YES ];

}

-(void) VTDataController:(VTDataController *)controller didFitalError:(NSError *)error
{
    
}

-(void) vtDataControllerDidLoaded:(VTDataController *)controller
{
    
}

#pragma mark - HCHomePageTableDataDelegate

- (void) HCHomePageTableDataClickedWithData:(NSDictionary *)data
{
    
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
