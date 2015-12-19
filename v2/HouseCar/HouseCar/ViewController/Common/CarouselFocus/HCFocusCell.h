//
//  HCFocusCell.h
//  SinaFinance
//
//  Created by shieh fabo on 14-9-12.
//  Copyright (c) 2014年 sina.com. All rights reserved.
//

#import <vTeam/vTeam.h>

@protocol HCFocusCellCellProtocol <NSObject>

-(void)cellDataDidClickedWithData:(NSDictionary* )data;

@end

@interface HCFocusCell : VTTableViewCell

@property (weak, nonatomic) IBOutlet UIPageControl *pageControl;
@property (weak, nonatomic) IBOutlet UIScrollView *scrollview;
@property (weak, nonatomic) IBOutlet UILabel *title;


@property (strong, nonatomic) NSString *srcKey;
@property (strong, nonatomic) NSString *titleKey;

@property (weak, nonatomic) id<HCFocusCellCellProtocol> delegate;


@property (strong, nonatomic)  NSArray *datas;

-(void)reloadData;

- (IBAction)pageControlValueChanged:(id)sender;


- (IBAction)imageClicked:(id)sender;

-(void)stopAutoPage;


@end


