//
//  VTDOMImageElement.h
//  vTeam
//
//  Created by zhang hailong on 13-8-14.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#import <vTeam/VTDOMElement.h>

#import <vTeam/IVTImageTask.h>

@interface VTDOMImageElement : VTDOMElement<IVTLocalImageTask>

@property(nonatomic,retain) UIImage * image;
@property(nonatomic,retain) UIImage * defaultImage;
@property(nonatomic,readonly,getter = isPreload) BOOL preload;
@property(nonatomic,retain) NSString * stretch;

@end
