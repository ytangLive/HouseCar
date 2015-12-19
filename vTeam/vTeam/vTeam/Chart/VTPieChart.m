//
//  VTPieChart.m
//  vTeam
//
//  Created by zhang hailong on 14-1-8.
//  Copyright (c) 2014年 hailong.org. All rights reserved.
//

#import "VTPieChart.h"

#import "VTChartArc.h"
#import "VTChartTipLabel.h"

@implementation VTPieChart

@synthesize titleReader = _titleReader;
@synthesize valueReader = _valueReader;
@synthesize dataItemsReader = _dataItemsReader;
@synthesize colorReader = _colorReader;
@synthesize totalValueReader = _totalValueReader;
@synthesize borderColorReader = _borderColorReader;
@synthesize borderWidth = _borderWidth;

-(void) dealloc{
    [_titleReader release];
    [_valueReader release];
    [_dataItemsReader release];
    [_colorReader release];
    [_totalValueReader release];
    [_borderColorReader release];
    [_font release];
    [_titleColor release];
    [_lineColor release];
    [super dealloc];
}

-(void) reloadData{
    
    [self removeAllComponents];
    
    NSArray * dataItems = [_dataItemsReader dataItemsValue:self.dataSource] ;
    
    double totalValue = [_totalValueReader doubleValue:self.dataSource];
    
    if(_totalValueReader == nil){
        for(id dataItem in dataItems){
            totalValue += [_valueReader doubleValue:dataItem];
        }
    }
    
    if(totalValue == 0.0){
        return;
    }
    
    CGFloat angle = M_PI * 2.0;
    CGFloat centerAngle = angle;
    
    CGSize innerSize = self.size;
    
    innerSize.width -= 40;
    innerSize.height -= 40;
    
    CGFloat radius = MIN(innerSize.width, innerSize.height) / 2.0f;
    
    //CGPoint position =  CGPointMake(self.size.width / 2.0, self.size.height / 2.0);
    CGPoint position =  CGPointMake(self.size.width / 2.0 + _arcOffsetPosion.x, self.size.height / 2.0 + _arcOffsetPosion.y);
    
    NSEnumerator * enumDataItem = [dataItems objectEnumerator];
    
    id dataItem = nil;
    
    CGFloat minAngle =  10.0 * M_PI / 180.0;
    
    while((dataItem = [enumDataItem nextObject])){
        
        double v = [_valueReader doubleValue:dataItem];
        
        VTChartArc * arc = [[VTChartArc alloc] init];
        
        arc.position = position;
        arc.radius = radius;
        arc.endAngle = angle;
        arc.startAngle = angle - M_PI * 2.0 * v / totalValue;
        arc.backgroundColor = [_colorReader colorValue:dataItem];
        arc.borderWidth = _borderWidth;
        arc.borderColor = [_borderColorReader colorValue:dataItem];
        
        [self addComponent:arc];
        
        NSString * title = [_titleReader stringValue:dataItem];
        
        if(title){
            
            VTChartTipLabel * tipLabel = [[VTChartTipLabel alloc] init];
            
            [tipLabel setFont:_font];
            [tipLabel setTitleColor:_titleColor];
            [tipLabel setLineColor:_lineColor];
            [tipLabel setLineWidth:1];
            [tipLabel setTitle:title];

            CGFloat angle = [arc angle];

            if(centerAngle - angle < minAngle){
                
                angle = centerAngle - minAngle;
                
            }
            
            CGPoint p = [arc vertex];
            
            CGFloat dis = 10;
            
            if(p.x >= position.x){
                
                [tipLabel setPosition:CGPointMake(position.x + cosf(angle) * ( dis + radius), position.y + sinf(angle) * (dis +radius))];
                [tipLabel setAnchor:CGPointMake(0, 0.5)];
                [tipLabel setToPosition:p];
                [tipLabel setPadding:UIEdgeInsetsMake(0, 6, 0, 0)];
            }
            else{
                
                [tipLabel setPosition:CGPointMake(position.x + cosf(angle) * ( dis + radius), position.y + sinf(angle) * ( dis + radius))];
                [tipLabel setAnchor:CGPointMake(1.0, 0.5)];
                [tipLabel setToPosition:p];
                [tipLabel setPadding:UIEdgeInsetsMake(0, 0, 0, 6)];
            }
            
            [tipLabel sizeToFit];
            
            [self addComponent:tipLabel];
            
            [tipLabel release];
            
        }

        if(arc.endAngle - arc.startAngle < minAngle){
            centerAngle -= minAngle;
        }
        else{
            centerAngle -= arc.endAngle - arc.startAngle;
        }
        
        angle = arc.startAngle;
        
        [arc release];
    }
    
}

-(void) setAnimationValue:(double)animationValue{
    [super setAnimationValue:animationValue];
    for(id<IVTChartComponent> component in [self components]){
        [component setAnimationValue:animationValue];
    }
}

@end
