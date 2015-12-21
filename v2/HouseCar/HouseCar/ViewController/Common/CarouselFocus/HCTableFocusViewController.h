//
//  SFInfoVer2TableFocusViewController.h
//  SinaFinance
//
//  Created by shieh fabo on 14-9-11.
//  Copyright (c) 2014年 sina.com. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "HCFocusCell.h"

@protocol HCTableFocusViewProtocol <NSObject>

-(void)HCTableFocusCellClickedWithData:(NSDictionary*)data;
-(void)HCTableFocusCellDownloaded:(NSArray*)data error:(NSError *)error;
@end

@interface HCTableFocusViewController : VTViewController<HCFocusCellCellProtocol>

@property (weak, nonatomic) IBOutlet UIView *focusView;
@property (strong, nonatomic) IBOutlet VTURLDataSource *dataSource;

@property (strong, nonatomic) NSString* focusNibNameOrNil;
@property (strong, nonatomic) NSBundle* nibBundleOrNil;
//自定义焦点图数量
@property (nonatomic) NSUInteger focusCount;

@property (strong, nonatomic)  HCFocusCell *focusCell;
@property (strong, nonatomic)  NSMutableArray *zxCellArray;

@property (strong, nonatomic) IBOutlet NSString* focusKey;
@property (strong, nonatomic) IBOutlet NSString* zxKey;

@property (strong, nonatomic) NSString *cellsrcKey;
@property (strong, nonatomic) NSString *celltitleKey;

@property (weak, nonatomic) id<HCTableFocusViewProtocol> delegate;

-(void)reloadData;
-(void)refreshData;

@end
