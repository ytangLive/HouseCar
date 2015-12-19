//
//  HCCalendarViewController.m
//  HouseCar
//
//  Created by tangyin on 15/12/3.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import "HCCalendarViewController.h"
#import "Kal.h"
#import <EventKit/EventKit.h>
#import <EventKitUI/EventKitUI.h>

@interface HCCalendarViewController () <UITableViewDelegate>
@property(nonatomic, weak) IBOutlet UIView *contentView;
@end

@implementation HCCalendarViewController
{
    KalViewController *kal;
    NSString *_parentType;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    
    NSDictionary *queryValues = [self.url queryValues];
    _parentType = [queryValues stringValueForKey:@"parentType"];
    
    NSDate *beginDate = [(id<HCContext>)self.context focusValueForKey:@"CarUserBeginDate"];
    NSDate *endDate = [(id<HCContext>)self.context focusValueForKey:@"CarUserEndDate"];
    
    beginDate = beginDate ? beginDate : [NSDate dateStartOfDay:[NSDate date]];
    endDate = endDate ? endDate : [NSDate dateStartOfDay:[[NSDate date] offsetDay:1]];
    
    kal = [[KalViewController alloc] initWithSelectionMode:KalSelectionModeRange];
    kal.beginDate = beginDate;
    kal.endDate = endDate;
    
    //kal.title = @"NativeCal";
    
    /*
     *    Kal Configuration
     *
     */
    kal.delegate = self;
    //dataSource = [[EventKitDataSource alloc] init];
    //kal.dataSource = dataSource;
    kal.minAvailableDate = [NSDate dateStartOfDay:[[NSDate date] offsetDay:0]];
    kal.maxAVailableDate = [kal.minAvailableDate offsetDay:30];
    [self.contentView addSubview:kal.view];
}

- (void)doAction:(id)sender
{
    NSString *actionName = [sender actionName];
    if([actionName isEqualToString:@"selected"]){
        [self openUrl:[NSURL URLWithString:@"."
                             relativeToURL:self.url
                               queryValues:nil] animated:YES ];
        
        if([_parentType isEqualToString:@"CarOrder"]){
            [(id<HCContext>)self.context setFocusValue:kal.beginDate forKey:@"CarUserBeginDate"];
            [(id<HCContext>)self.context setFocusValue:kal.endDate forKey:@"CarUserEndDate"];
        }
    }
    
    [super doAction:sender];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
