//
//  VTTableViewCell.h
//  vTeam
//
//  Created by Zhang Hailong on 13-6-22.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#import <UIKit/UIKit.h>

#import <vTeam/VTStyleOutletContainer.h>
#import <vTeam/VTDataOutletContainer.h>
#import <vTeam/IVTUIContext.h>
#import <vTeam/IVTAction.h>
#import <vTeam/VTLayoutContainer.h>
#import <vTeam/VTDataSource.h>

#import <vTeam/VTDOMView.h>

@interface UITableView (VTTableViewCell)

-(void) applyDataOutlet;

@end

@interface VTTableViewCell : UITableViewCell<VTDOMViewDelegate>

@property(nonatomic,readonly) NSString * nibNameOrNil;
@property(nonatomic,readonly) NSBundle * nibBundleOrNil;

@property(nonatomic,retain) id userInfo;
@property(nonatomic,assign) NSInteger index;

@property(nonatomic,assign) id controller;
@property(nonatomic,assign) id<IVTUIContext> context;
@property(nonatomic,retain) IBOutlet VTStyleOutletContainer * styleContainer;
@property(nonatomic,retain) IBOutletCollection(VTStyleOutletContainer) NSArray * styleContainers;
@property(nonatomic,retain) IBOutlet VTDataOutletContainer * dataOutletContainer;
@property(nonatomic,retain) IBOutlet VTLayoutContainer * layoutContainer;
@property(nonatomic,assign) IBOutlet id delegate;
@property(nonatomic,retain) id dataItem;
@property(nonatomic,retain) UIColor * actionColor;


@property(nonatomic,retain) IBOutlet VTDataSource * dataSource;

-(IBAction) doAction :(id)sender;

-(void) downloadImagesForView:(UIView *) view;

-(void) loadImagesForView:(UIView *) view;

-(void) cancelDownloadImagesForView:(UIView *) view;

+(id) tableViewCellWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil;

@end

@protocol VTTableViewCellDelegate

@optional

-(void) vtTableViewCell:(VTTableViewCell *) tableViewCell doAction:(id) action;

@end