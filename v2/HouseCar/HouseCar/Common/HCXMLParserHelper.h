//
//  SFXMLParserHelper.h
//  VTTeamProject
//
//  Created by sinanews on 13-7-17.
//  Copyright (c) 2013年 ytang.com. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface HCXMLParserHelper : NSObject<NSXMLParserDelegate> {
    BOOL _ignoreElement;
}

@property (nonatomic,strong) NSMutableString *contentText;

-(BOOL)parse:(NSString*)strXML;

@end
