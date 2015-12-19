//
//  HCFilterMenusView.h
//  HouseCar
//
//  Created by tangyin on 15/11/25.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import <UIKit/UIKit.h>

@protocol HCFilterMenusViewDelegate <NSObject>

- (void)openCityList;

- (void)confirmSelectedItems:(NSDictionary *)selectedItems;

@end

@interface HCFilterMenusView : UIView <UITableViewDataSource, UITableViewDelegate>

@property (nonatomic, strong)VTTableView *tableView;

@property (nonatomic, strong)NSArray *filterMenus;
@property (nonatomic,strong) NSMutableDictionary *selectedItems;

@property (nonatomic, weak)id<HCFilterMenusViewDelegate> delegate;

@property(nonatomic,assign) id<IVTUIContext> context;

@end
