//
//  VTDomStyleController.h
//  vTeam
//
//  Created by shieh fabo on 14-10-8.
//  Copyright (c) 2014年 hailong.org. All rights reserved.
//

#import <Foundation/Foundation.h>

@class VTStyleController;

@interface VTDomStyleController : NSObject
@property(nonatomic,retain) IBOutletCollection(NSString) NSArray * styles;

-(id)initWithCSSName:(NSString*)cssName bundle:(NSBundle*)bundle;

-(id)addObjectFromDomStyleController:(VTDomStyleController*)controller;

-(VTStyleController*)styleController;

@end
