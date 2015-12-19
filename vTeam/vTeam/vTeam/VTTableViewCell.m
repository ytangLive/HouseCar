//
//  VTTableViewCell.m
//  vTeam
//
//  Created by Zhang Hailong on 13-6-22.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#import "VTTableViewCell.h"

#import "IVTImageTask.h"

#import "UIView+Search.h"

#import "VTAction.h"

@implementation UITableView(VTTableViewCell)

-(void) applyDataOutlet{
    NSArray * cells = [self visibleCells];
    if([cells count]){
        for(VTTableViewCell * cell in cells){
            if([cell isKindOfClass:[VTTableViewCell class]]){
                [cell setDataItem:cell.dataItem];
            }
        }
    }
}

@end

@interface VTTableViewCell(){
    CALayer * _highlightedLayer;
}
@property(nonatomic,retain) CALayer * highlightedLayer;
@property(nonatomic,assign) BOOL isHighlighted;
@property(nonatomic,assign) BOOL isSelected;

@end


@implementation VTTableViewCell


@synthesize nibNameOrNil = _nibNameOrNil;
@synthesize nibBundleOrNil = _nibBundleOrNil;

@synthesize context = _context;
@synthesize dataOutletContainer = _dataOutletContainer;

@synthesize index = _index;
@synthesize userInfo = _userInfo;
@synthesize delegate = _delegate;
@synthesize dataItem = _dataItem;
@synthesize layoutContainer = _layoutContainer;
@synthesize dataSource = _dataSource;
@synthesize controller = _controller;
@synthesize highlightedLayer= _highlightedLayer;
@synthesize actionColor = _actionColor;


@synthesize styleContainers = _styleContainers;

-(void) dealloc{
    [_actionColor release];
    [_highlightedLayer removeFromSuperlayer];
    [_highlightedLayer release];
    [_dataItem release];
    [_userInfo release];
    [_styleContainers release];
    [_dataOutletContainer release];
    [_dataSource setDelegate:nil];
    [_dataSource release];
    [_nibNameOrNil release];
    [_nibBundleOrNil release];
    [super dealloc];
}

-(void)refreshHighlightedLayer
{
    if([self isHighlighted] || [self isSelected])
    {
        if(_highlightedLayer == nil){
            _highlightedLayer = [[CALayer alloc] init];
        }
        
        UIColor * actionColor = self.actionColor;
        
        if(actionColor == nil){
            _highlightedLayer.masksToBounds = YES;
            _highlightedLayer.backgroundColor = [actionColor CGColor];
            
            _highlightedLayer.frame = self.bounds;
            [_highlightedLayer removeFromSuperlayer];
            
        }
        else
        {
            _highlightedLayer.masksToBounds = YES;
            _highlightedLayer.backgroundColor = [actionColor CGColor];
            
            _highlightedLayer.frame = self.bounds;
            
            [self.layer addSublayer:_highlightedLayer];
        }
    }
    else
    {
        [_highlightedLayer removeFromSuperlayer];
    }
    
}

-(void) touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event{
    
    CGPoint location = [[touches anyObject] locationInView:self];
    
    [self touchesBegan:location];

    [super touchesBegan:touches withEvent:event];
}

-(void) touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event{
    
    CGPoint location = [[touches anyObject] locationInView:self];
    
    [self touchesEnded:location];
    
    [super touchesEnded:touches withEvent:event];
}

-(void) touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event{
    
    CGPoint location = [[touches anyObject] locationInView:self];
    
    [self touchesCancelled:location];
    
    [super touchesCancelled:touches withEvent:event];
}

-(void) touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event{
    
    CGPoint location = [[touches anyObject] locationInView:self];
    
    [self touchesMoved:location];
    
    [super touchesMoved:touches withEvent:event];
}

- (BOOL)touchesBegan:(CGPoint) location{
    CGSize size = self.frame.size;
    if(location.x >=0 && location.y >=0 && location.x < size.width && location.y < size.height){
        [self setHighlighted:YES];
    }
    return NO;
}

- (void)touchesMoved:(CGPoint) location{
    CGSize size = self.frame.size;
    if(location.x < 0 || location.y < 0 || location.x >= size.width || location.y >= size.height){
        [self setHighlighted:NO];
    }
}

- (void)touchesEnded:(CGPoint) location{
    [self setHighlighted:NO];
}

- (void)touchesCancelled:(CGPoint) location{
    [self setHighlighted:NO];
}


-(void)setActionColor:(UIColor *)actionColor
{
    if (_actionColor!=actionColor) {
        [actionColor retain];
        [_actionColor release];
        _actionColor = actionColor;
        
    }
}

-(void) setHighlighted:(BOOL)highlighted{
    [super setHighlighted:highlighted];
    self.isHighlighted = highlighted;
    [self refreshHighlightedLayer];
}

-(void) setSelected:(BOOL)selected{
    [super setSelected:selected];
    self.isSelected = selected;
    [self refreshHighlightedLayer];
}


+(id) tableViewCellWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil{
    UIViewController * viewController = [[[UIViewController alloc] initWithNibName:nibNameOrNil bundle:nibBundleOrNil] autorelease];
    
    UIView * view = [viewController view];
    
    if([view isKindOfClass:[VTTableViewCell class]]){
        
        return view;
    }
    
    return nil;
}


-(IBAction) doAction :(id)sender{
    if([sender conformsToProtocol:@protocol(IVTAction)]){
        if([_delegate respondsToSelector:@selector(vtTableViewCell:doAction:)]){
            [_delegate vtTableViewCell:self doAction:sender];
        }
    }
}

-(void) setDataItem:(id)dataItem{
    if(_dataItem != dataItem){
        [_dataItem release];
        _dataItem = [dataItem retain];
        [_dataOutletContainer applyDataOutlet:self];
        [self cancelDownloadImagesForView:self];
        [self loadImagesForView:self];
        
        dispatch_async(dispatch_get_main_queue(), ^{
            [self downloadImagesForView:self];
        });
        
        [_layoutContainer layout];
        [_dataSource cancel];
        [_dataSource reloadData];
    }
    else{
        [_dataOutletContainer applyDataOutlet:self];
        [_layoutContainer layout];
    }
}


-(void) vtDataSourceDidLoadedFromCache:(VTDataSource *) dataSource timestamp:(NSDate *) timestamp{
    [_dataOutletContainer applyDataOutlet:self];
    [self downloadImagesForView:self];
    [_layoutContainer layout];
}

-(void) vtDataSourceDidLoaded:(VTDataSource *) dataSource{
    [_dataOutletContainer applyDataOutlet:self];
    [self downloadImagesForView:self];
    [_layoutContainer layout];
}

-(void) vtDataSourceDidContentChanged:(VTDataSource *) dataSource{
    [_dataOutletContainer applyDataOutlet:self];
    [self downloadImagesForView:self];
    [_layoutContainer layout];
}

-(void) downloadImagesForView:(UIView *) view{
    NSArray * imageViews = [view searchViewForProtocol:@protocol(IVTImageTask)];
    for(id imageView in imageViews){
        if(![imageView isLoading] && ![imageView isLoaded]){
            [imageView setSource:self];
            [self.context handle:@protocol(IVTImageTask) task:imageView priority:0];
        }
    }
}

-(void) loadImagesForView:(UIView *) view{
    NSArray * imageViews = [view searchViewForProtocol:@protocol(IVTImageTask)];
    for(id imageView in imageViews){
        if([imageView isLoading]){
            [self.context cancelHandle:@protocol(IVTImageTask) task:imageView];
        }
        if(![imageView isLoaded]){
            [self.context handle:@protocol(IVTLocalImageTask) task:imageView priority:0];
        }
    }
}

-(void) cancelDownloadImagesForView:(UIView *) view{
    NSArray * imageViews = [view searchViewForProtocol:@protocol(IVTImageTask)];
    for(id imageView in imageViews){
        if([imageView isLoading]){
            [self.context cancelHandle:@protocol(IVTImageTask) task:imageView];
        }
    }
}

-(void) setContext:(id<IVTUIContext>)context{
    if(_context != context){
        _context = context;
        [_dataSource setContext:context];
        for(VTStyleOutletContainer * styleContainer in _styleContainers){
            [styleContainer setStyleSheet:[context styleSheet]];
        }
        [_layoutContainer layout];
    }
}

-(void) vtDOMView:(VTDOMView *) view doActionElement:(VTDOMElement *) element{
    if([element conformsToProtocol:@protocol(IVTAction)]){
        if([self.delegate respondsToSelector:@selector(vtTableViewCell:doAction:)]){
            [self.delegate vtTableViewCell:self doAction:element];
        }
    }
    else if([[element name] hasPrefix:@"a"] && [[element attributeValueForKey:@"href"] length]){
        VTAction * action  =[[[VTAction alloc] init] autorelease];
        [action setActionName:@"url"];
        [action setUserInfo:[element attributeValueForKey:@"href"]];
        if([self.delegate respondsToSelector:@selector(vtTableViewCell:doAction:)]){
            [self.delegate vtTableViewCell:self doAction:action];
        }
    }
}

-(VTStyleOutletContainer *) styleContainer{
    return [_styleContainers lastObject];
}

-(void) setStyleContainer:(VTStyleOutletContainer *)styleContainer{
    self.styleContainers = [NSArray arrayWithObject:styleContainer];
}

@end
