//
//  SFXMLParserHelper.m
//  VTTeamProject
//
//  Created by sinanews on 13-7-17.
//  Copyright (c) 2013年 ytang.com. All rights reserved.
//

#import "HCXMLParserHelper.h"

@implementation HCXMLParserHelper

-(BOOL)parse:(NSString*)strXML
{
    NSXMLParser *xmlParser = [[NSXMLParser alloc] initWithData:[strXML dataUsingEncoding:NSUTF8StringEncoding]];
    xmlParser.delegate = self;
    return [xmlParser parse];
}

// 开始解析document.
- (void)parserDidStartDocument:(NSXMLParser *)parser
{
    self.contentText = [NSMutableString stringWithCapacity:1024];
}

// 成功的解析完毕.
- (void)parserDidEndDocument:(NSXMLParser *)parser
{
    
}

// 遇到元素的开始标签
- (void)parser:(NSXMLParser *)parser didStartElement:(NSString *)elementName namespaceURI:(NSString *)namespaceURI qualifiedName:(NSString *)qName attributes:(NSDictionary *)attributeDict
{
    if ([elementName isEqualToString:@"table"]) {
        _ignoreElement = YES;
    }
}

//遇到标签包含内容字符
- (void)parser:(NSXMLParser *)parser foundCharacters:(NSString *)string
{
    if (!_ignoreElement) {
        [_contentText appendString:string];
    }
}

// 遇到元素的结束标签
- (void)parser:(NSXMLParser *)parser didEndElement:(NSString *)elementName namespaceURI:(NSString *)namespaceURI qualifiedName:(NSString *)qName
{
    if ([elementName isEqualToString:@"table"]) {
        _ignoreElement = NO;
    }
}

//有严重错误，停止解析
- (void)parser:(NSXMLParser *)parser parseErrorOccurred:(NSError *)parseError
{

}

@end
