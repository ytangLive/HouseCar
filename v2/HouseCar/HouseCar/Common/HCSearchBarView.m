//
//  SC_GoodsSearchView.m
//  HouseCar
//
//  Created by a on 14-11-25.
//  Copyright (c) 2014年 ytang.com. All rights reserved.
//

#import "HCSearchBarView.h"

@interface HCSearchBarView() <UITextFieldDelegate>

@end

@implementation HCSearchBarView
{
    CGFloat viewHeight;
    BOOL _textIsClear;
}

- (instancetype)initWithCoder:(NSCoder *)aDecoder
{
    self = [super initWithCoder:aDecoder];
    if(self){
        self.backgroundColor = [UIColor clearColor];
    }
    return self;
}

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        viewHeight = frame.size.height;
        CGFloat topSpace = 6.0f;
        if(!IOS7_OR_LATER){
            topSpace = -2.0f;
        }
        _textField = [[UITextField alloc] initWithFrame:CGRectMake(0, topSpace, frame.size.width, frame.size.height)];
        [self initView];
    }
    return self;
}

- (void)awakeFromNib
{
    viewHeight = self.frame.size.height;
    if(!_textField){
        CGFloat topSpace = 6.0f;
        if(!IOS7_OR_LATER){
            topSpace = -2.0f;
        }
        _textField = [[UITextField alloc] initWithFrame:CGRectMake(0, topSpace, self.frame.size.width, self.frame.size.height)];
        [self initView];
    }
}

-(void)initView
{
    _textIsClear = YES;
    
    _textField.contentVerticalAlignment = UIControlContentVerticalAlignmentCenter;
    _textField.textAlignment=NSTextAlignmentLeft;
    _textField.clearButtonMode=UITextFieldViewModeWhileEditing;
    _textField.font=[UIFont systemFontOfSize:15];
    _textField.placeholder=@"请输入关键字";
    [_textField setValue:[UIColor whiteColor] forKeyPath:@"_placeholderLabel.textColor"];
    [_textField setValue:[UIFont systemFontOfSize:14] forKeyPath:@"_placeholderLabel.font"];
    _textField.textColor = [UIColor whiteColor];
    _textField.backgroundColor = [UIColor colorWithWhite:1.0 alpha:0.3];
    _textField.layer.masksToBounds = YES;
    _textField.layer.cornerRadius = 5.0;
    //输入框背景
    _textField.leftView = [self getLeftView];
    _textField.leftViewMode = UITextFieldViewModeAlways;
    _textField.returnKeyType=UIReturnKeySearch;
    _textField.delegate=self;
    [self addSubview:_textField];
    
}

#pragma -mark UITextFieldDelegate

- (void)textFieldDidBeginEditing:(UITextField *)textField
{
    if(_textIsClear){
        if(_delegate && [_delegate respondsToSelector:@selector(textFieldDidBeginEditing:)]){
            [self.delegate textFieldDidBeginEditing:textField];
        }
    }
}

- (void)textFieldDidEndEditing:(UITextField *)textField
{
    _textIsClear = YES;
}

- (BOOL)textFieldShouldClear:(UITextField *)textField
{
    textField.text = @"";
    _textIsClear = NO;
    return YES;
}

//点击搜索
- (BOOL)textFieldShouldReturn:(UITextField *)textField
{
    [self searchBtnClick];
    
    return YES;
}

- (void)setFrame:(CGRect)frame
{
    [super setFrame:frame];
    _textField.frame = CGRectMake(0, 6, frame.size.width, 29);
}

-(void)searchBtnClick
{
    [_textField resignFirstResponder];
    if (_delegate && [_delegate respondsToSelector:@selector(searchBtnClick:searchKey:)]) {
        [_delegate searchBtnClick:self searchKey:[_textField.text stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]]];
    }
}

-(UIView *)getLeftView
{
    UIView *leftView = [[UIImageView alloc] initWithFrame:CGRectMake(0, 0, 32, viewHeight)];
    leftView.backgroundColor = [UIColor clearColor];
    leftView.userInteractionEnabled = YES;
    
    //放大镜图片
    UIImageView *magnifier=[[UIImageView alloc] initWithFrame:CGRectMake(0, 0, 17, 17)];
    magnifier.center = leftView.center;
    magnifier.backgroundColor=[UIColor clearColor];
    magnifier.image=[UIImage imageNamed:@"search_icon"];
    [leftView addSubview:magnifier];
    
    return leftView;
}

@end
