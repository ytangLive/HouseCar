//
//  HCTableFocusViewController.m
//  SinaFinance
//
//  Created by shieh fabo on 14-9-11.
//  Copyright (c) 2014年 sina.com. All rights reserved.
//

#import "HCTableFocusViewController.h"

#define DefaultCount 3

@interface HCTableFocusViewController ()

@end

@implementation HCTableFocusViewController


- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    self.dataSource.context = self.context;
    [self loadCoreView];
    [self layoutCoreView];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

-(void)dealloc
{
    [self.focusCell stopAutoPage];
    self.dataSource.delegate = nil;
}

- (NSUInteger)focusCount
{
    if(!_focusCount){
        _focusCount = DefaultCount;
    }
    return _focusCount;
}

-(void)loadCoreView
{
    if (self.focusCell==nil) {
        UIViewController * viewController = [[UIViewController alloc] initWithNibName:self.focusNibNameOrNil bundle:self.nibBundleOrNil] ;
        
        UIView * view = [viewController view];
        self.focusCell = (HCFocusCell*)view;
        self.focusCell.context = self.context;
        self.focusCell.srcKey = self.cellsrcKey;
        self.focusCell.titleKey = self.celltitleKey;
        self.focusCell.delegate = self;
        
    }
}

-(void)layoutCoreView
{
    CGSize fitScreenSize = CGSizeMake(CGRectGetWidth([UIScreen mainScreen].bounds), 168*CGRectGetWidth([UIScreen mainScreen].bounds)/320);
    
    CGRect fitBounds = self.focusCell.frame;
    fitBounds.size = fitScreenSize;
    self.view.frame = fitBounds;
    self.focusCell.frame = fitBounds;
    [self.focusView addSubview:self.focusCell];
}

-(void)reloadData
{
    [self.dataSource reloadData];
}

-(void)refreshData
{
    [self.dataSource reloadData];
}

-(void)afterLoaded:(NSError *)error
{
    if (!error) {
        NSArray* dataObjects = self.dataSource.dataObjects;
        NSMutableArray *tFocusArray = [NSMutableArray array];
        
        for (int i = 0; i < [dataObjects count]; i ++) {
            if(i > (self.focusCount - 1)){
                break;
            }
            NSDictionary *dataObject = dataObjects[i];
            NSString *imageUrl = [dataObject stringValueForKeyPath:@"thumbnail_images.full.url"];
            if(!imageUrl){
                imageUrl = [dataObject stringValueForKeyPath:@"thumbnail_images.large.url"] ? [dataObject stringValueForKeyPath:@"thumbnail_images.large.url"] : @"";
            }
            NSDictionary *focusObject = @{@"title":[dataObject stringValueForKey:@"title"],@"p_url": imageUrl};
            [tFocusArray addObject:focusObject];
        }
        self.focusCell.datas = tFocusArray;
        
        [self.focusCell reloadData];
        
        if ([self.delegate conformsToProtocol:@protocol(HCTableFocusViewProtocol)]&&[self.delegate respondsToSelector:@selector(HCTableFocusCellDownloaded:error:)]) {
            [self.delegate HCTableFocusCellDownloaded:dataObjects error:nil];
        }
        
    }else{
        if ([self.delegate conformsToProtocol:@protocol(HCTableFocusViewProtocol)]&&[self.delegate respondsToSelector:@selector(HCTableFocusCellDownloaded:error:)]) {
            [self.delegate HCTableFocusCellDownloaded:nil error:error];
        }
    }
}

#pragma mark ======datasource delegate=======
-(void) vtDataSourceWillLoading:(VTDataSource *) dataSource
{
    
}

-(void) vtDataSourceDidLoadedFromCache:(VTDataSource *) dataSource timestamp:(NSDate *) timestamp
{
    [self afterLoaded:nil];
}

-(void) vtDataSourceDidLoaded:(VTDataSource *) dataSource
{
    [self afterLoaded:nil];
    
}

-(void) vtDataSource:(VTDataSource *) dataSource didFitalError:(NSError *) error
{
    [self afterLoaded:error];
}

#pragma mark ======cell protocol=======
-(void)cellDataDidClickedWithData:(NSDictionary* )data
{
    if ([self.delegate conformsToProtocol:@protocol(HCTableFocusViewProtocol)]) {
        [self.delegate HCTableFocusCellClickedWithData:data];
    }
    
}
@end
