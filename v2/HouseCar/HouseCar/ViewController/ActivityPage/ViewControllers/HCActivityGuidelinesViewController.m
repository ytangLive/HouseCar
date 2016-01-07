//
//  HCActivityGuidelinesViewController.m
//  HouseCar
//
//  Created by tangyin on 16/1/5.
//  Copyright © 2016年 sina.com. All rights reserved.
//

#import "HCActivityGuidelinesViewController.h"

#define TextPadding 16.0
#define GapSpace 10
#define HorizontalSpace 10
#define VerticalSpace 15
#define LineViewHeight 1
#define BoldLineViewHeight 10
#define TitleFont [UIFont systemFontOfSize:14.0f]
#define ContentFont [UIFont systemFontOfSize:12.0f]

@interface HCActivityGuidelinesViewController ()

@property (nonatomic, strong)NSArray *dataObjects;

@end

@implementation HCActivityGuidelinesViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    
    NSDictionary *params = [self.url queryValues];
    NSString *jsonStr = [params stringValueForKey:@"activityGuidelines"];
    if(jsonStr){
        _dataObjects = [VTJSON decodeText:jsonStr];
    }
    
    _tableView.backgroundColor = DefaultBackgroundColor;
    _tableView.dataSource = self;
    _tableView.delegate = self;
    
    [_tableView reloadData];
    
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}


- (NSArray *)dataObjects
{
    if(!_dataObjects){
        _dataObjects = [[NSArray alloc] init];
    }
    return _dataObjects;
}

#pragma mark - table view delegate

-(NSInteger) tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    
    return [self.dataObjects count];
}

- (CGFloat) tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    id data = [self.dataObjects objectAtIndex:indexPath.row];
    
    if(data){

        NSString *title = [data stringValueForKey:@"title"];
        NSString *content = [data stringValueForKey:@"content"];
        CGSize titleSize = [title VTSizeWithFont:TitleFont constrainedToSize:CGSizeMake(CGFLOAT_MAX, CGFLOAT_MAX) lineBreakMode:NSLineBreakByClipping];
        
        NSDictionary *attributes = [self attributesWidthFont:ContentFont];
        CGSize contentSize = [self VTSizeWithAttributes:content constrainedToSize: CGSizeMake(KScreenWidth - 2 * HorizontalSpace - TextPadding, CGFLOAT_MAX) widthAttributes:attributes];
        
        
        CGFloat cellHeight = VerticalSpace + titleSize.height + LineViewHeight + contentSize.height + TextPadding + 2 * GapSpace + BoldLineViewHeight;
        
        return cellHeight;
        
    }else{
        return tableView.rowHeight;
    }
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString * identifier = @"HCActivityGuidelinesCell";
    
    UITableViewCell * cell = [tableView dequeueReusableCellWithIdentifier:identifier];
    
    if(cell == nil){
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:identifier];
        cell.selectionStyle = UITableViewCellSelectionStyleNone;
    }
    
    for (UIView *view in [cell.contentView subviews]) {
        [view removeFromSuperview];
    }
    
    id data = [self.dataObjects objectAtIndex:indexPath.row];

    NSString *title = [data stringValueForKey:@"title"];
    NSString *content = [data stringValueForKey:@"content"];
    CGSize titleSize = [title VTSizeWithFont:TitleFont constrainedToSize:CGSizeMake(CGFLOAT_MAX, CGFLOAT_MAX) lineBreakMode:NSLineBreakByClipping];
    
    UILabel *picLabel = [[UILabel alloc] initWithFrame:CGRectMake(HorizontalSpace, VerticalSpace, 17, 17)];
    picLabel.layer.cornerRadius = picLabel.width / 2.f;
    picLabel.clipsToBounds = YES;
    picLabel.backgroundColor = DefaultNavBarColor;
    picLabel.font = [UIFont boldSystemFontOfSize:12];
    picLabel.textColor = [UIColor whiteColor];
    picLabel.textAlignment = NSTextAlignmentCenter;
    if(indexPath.row < 2){
        picLabel.text = @"￥";
    }else{
        picLabel.text = @"⍜";
    }
    
    UILabel *titleLabel = [[UILabel alloc] initWithFrame:CGRectMake(picLabel.maxX + 5, VerticalSpace, titleSize.width, titleSize.height)];
    titleLabel.font = TitleFont;
    titleLabel.textColor = COLOR_HEX(0x666666);
    titleLabel.text = title;
    
    UIView *lineView = [UIView initLineWidthFrame:CGRectMake(HorizontalSpace, picLabel.maxY + GapSpace, KScreenWidth - 2 * HorizontalSpace, LineViewHeight)];
    
    NSDictionary *attributes = [self attributesWidthFont:ContentFont];
    
    CGSize contentSize = [self VTSizeWithAttributes:content constrainedToSize: CGSizeMake(KScreenWidth - 2 * HorizontalSpace - TextPadding, CGFLOAT_MAX) widthAttributes:attributes];
    
    UITextView *textView = [[UITextView alloc] initWithFrame:CGRectMake(HorizontalSpace, lineView.maxY, KScreenWidth - 2 * HorizontalSpace, contentSize.height + TextPadding)];
    textView.editable = NO;
    textView.scrollEnabled = NO;
    textView.font = ContentFont;
    textView.attributedText = [[NSAttributedString alloc] initWithString:content attributes:attributes];
    
    if(indexPath.row < 2){
        UIView *boldLineView = [[UIView alloc] initLineWithFrame:CGRectMake(0, textView.maxY + GapSpace, KScreenWidth, BoldLineViewHeight) color:DefaultBackgroundColor];
        [cell.contentView addSubview:boldLineView];
    }
    
    [cell.contentView addSubview:picLabel];
    [cell.contentView addSubview:titleLabel];
    [cell.contentView addSubview:lineView];
    [cell.contentView addSubview:textView];

    return cell;
    
}

- (NSDictionary *)attributesWidthFont:(UIFont *)font
{
    NSMutableParagraphStyle *paragraphStyle = [[NSMutableParagraphStyle alloc] init];
    paragraphStyle.lineSpacing = 8;// 字体的行间距
    paragraphStyle.lineBreakMode = NSLineBreakByCharWrapping;
    
    NSDictionary *attributes = @{
                                 NSFontAttributeName:font,
                                 NSForegroundColorAttributeName:COLOR_HEX(0x666666),
                                 NSParagraphStyleAttributeName:paragraphStyle
                                 };
    return attributes;
}

- (CGSize)VTSizeWithAttributes:(NSString *)text constrainedToSize:(CGSize)size widthAttributes:(NSDictionary*)attributes
{
    if ([[[UIDevice currentDevice] systemVersion] floatValue] < 9.0) {
        return [text boundingRectWithSize:size options:NSStringDrawingUsesLineFragmentOrigin attributes:attributes context:nil].size;
    }else{
        CGSize currentSize = [text boundingRectWithSize:size options:NSStringDrawingUsesLineFragmentOrigin attributes:attributes context:nil].size;
        CGSize adjustedSize = CGSizeMake(ceilf(currentSize.width), ceilf(currentSize.height));
        return adjustedSize;
    }
}

@end
