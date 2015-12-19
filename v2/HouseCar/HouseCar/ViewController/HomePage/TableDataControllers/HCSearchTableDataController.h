//
//  HCSearchTableDataController.h
//  HouseCar
//
//  Created by tangyin on 15/12/11.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import "HCTableDataController.h"

typedef NS_ENUM(NSInteger,TableSectionType){
    TableSectionTypeHot,//热门搜索section
    TableSectionTypeHistory,//历史搜索Section
};


@interface HCSearchTableDataControllerSection : NSObject

@property(nonatomic,retain) IBOutlet UITableViewCell * headerViewCell;
@property(nonatomic,assign) NSInteger index;

@end

@interface HCSearchTableDataController : HCTableDataController

@property(nonatomic,retain) IBOutletCollection(HCSearchTableDataControllerSection) NSArray * sections;

@end
