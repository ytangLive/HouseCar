//
//  VTDOMLabel.m
//  vTeam
//
//  Created by zhang hailong on 13-9-3.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#import "VTDOMLabel.h"

#import "VTDOMDocument.h"
#import "VTDOMParse.h"
#import "VTDOMElement+Frame.h"
#import "VTDOMElement+Render.h"
#import "VTDOMElement+Layout.h"

@interface VTDOMLabel(){

}
@property(nonatomic,retain) VTDOMDocument * document;
@property(nonatomic,retain) NSMutableDictionary * statusDict;

-(void) loadDocumentHtml:(NSString *) html;

@end

@implementation VTDOMLabel

@synthesize document = _document;

-(void) dealloc{
    [_document release];
    [super dealloc];
}

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
    }
    return self;
}

-(id) initWithCoder:(NSCoder *)aDecoder{
    if((self = [super initWithCoder:aDecoder])){
        [self loadDocumentHtml:self.text];
    }
    return self;
}

/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect
{
    // Drawing code
}
*/

- (void)drawTextInRect:(CGRect)rect{
    
    CGContextRef ctx = UIGraphicsGetCurrentContext();
    
    CGContextSetTextMatrix(ctx , CGAffineTransformIdentity);
    
    [_document.rootElement render:_document.rootElement.frame context:ctx];
    
}

- (void) setText:(NSString *)text{
    [super setText:text];
    
    [self loadDocumentHtml:text];
    [self loadExtraSetup];
    [self setNeedsDisplay];
}

-(void) setFrame:(CGRect)frame{
    [super setFrame:frame];
    [_document.rootElement layout:self.bounds.size];
    [self setNeedsDisplay];
}

-(void) loadDocumentHtml:(NSString *) html{
    
    if(html){
        VTDOMDocument * document = [[VTDOMDocument alloc] init];
        
        VTDOMParse * parse = [[VTDOMParse alloc] init];
        
        [parse parseHTML:html toDocument:document];
        
        [document.rootElement layout:self.bounds.size];
        
        [self setDocument:document];
        
        [document release];
        
        [parse release];
    }
    else {
        [self setDocument:nil];
    }
    
}

-(void)loadExtraSetup
{
    NSArray* statusKeys = [self.statusDict allKeys];
    for (NSString* oneKey in statusKeys) {
        NSString* oneValue = [self.statusDict valueForKey:oneKey];
        VTDOMElement * element = [self.document elementById:oneKey];
        [element setAttributeValue:oneValue forKey:@"status"];
    }
}

-(void)setStatus:(NSString*)status byId:(NSString*)idstring
{
    if (self.statusDict==nil) {
        self.statusDict = [NSMutableDictionary dictionaryWithCapacity:0];
    }
    if (status) {
        [self.statusDict setValue:status forKey:idstring];
    }
    else
    {
        [self.statusDict removeObjectForKey:idstring];
    }
    [self setText:self.text];
}

@end
