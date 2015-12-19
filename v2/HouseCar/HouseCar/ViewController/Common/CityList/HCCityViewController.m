//
//  HCCityViewController.m
//  HouseCar
//
//  Created by tangyin on 15/11/17.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import "HCCityViewController.h"
#import "HCNavigationTitleBar.h"
#import "HCSearchBarView.h"
#import "HCHomePageViewController.h"

@interface HCCityViewController () <HCSearchBarViewViewDelegate>
@property (weak, nonatomic) IBOutlet UILabel *currentCityTitle;
@property (nonatomic, strong) NSArray *sectionTitles;
@property (nonatomic, strong) NSArray *chineseLetterCities;
@property (nonatomic, strong) NSMutableArray *searchData;
@property (nonatomic, strong) NSArray *searchDataKeys;
@property (nonatomic) BOOL isSearch;
@end

@implementation HCCityViewController
{
    NSString *_parentType;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    
    NSDictionary *queryValues = [self.url queryValues];
    _parentType = [queryValues stringValueForKey:@"parentType"];
    
    _isSearch = NO;
    
    NSArray *citiesData = [(id<HCContext>)self.context citiesData];
    
    if(citiesData && [citiesData count] > 0){
        self.cities = citiesData;
        
        self.cityKeys = [self keysCities:_cities isSectionTitle:NO];
        self.sectionTitles = [self keysCities:_cities isSectionTitle:YES];
        
        self.chineseLetterCities = [(id<HCContext>)self.context chineseLetterCitiesData];
    }
    
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
}

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark - Property

- (NSMutableArray *)searchData
{
    if(!_searchData){
        _searchData = [NSMutableArray array];
    }
    return _searchData;
}


#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    // Return the number of sections.
    if(_isSearch){
        return [_searchData count];
    }else{
        return [_cities count];
    }
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    // Return the number of rows in the section.
    NSArray *citySection = [self citySectionAtIndex:section];
    return [citySection count];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *CellIdentifier = @"Cell";
    
    NSArray *citySection = [self citySectionAtIndex:indexPath.section];
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier];
    }
    
    // Configure the cell...
    cell.textLabel.font = [UIFont systemFontOfSize:15];
    cell.textLabel.text = [citySection objectAtIndex:indexPath.row];
    
    return cell;
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
    NSString *key = [_cityKeys objectAtIndex:section];
    if(_isSearch){
        key = [_searchDataKeys objectAtIndex:section];
    }
    return key;
}

- (NSArray *)sectionIndexTitlesForTableView:(UITableView *)tableView
{
    if(_isSearch){
        return nil;
    }else{
        return _sectionTitles;
    }
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    NSArray *citySection = [self citySectionAtIndex:indexPath.section];
    NSString *cityName = [citySection objectAtIndex:indexPath.row];
    [self openUrl:[NSURL URLWithString:@"."
                         relativeToURL:self.url
                           queryValues:nil] animated:YES ];
    
    if([_parentType isEqualToString:@"HomePage"]){
        [(id<HCContext>)self.context setFocusValue:cityName forKey:@"cityName"];
    }else if([_parentType isEqualToString:@"CartBook"]){
        [(id<HCContext>)self.context setFocusValue:cityName forKey:@"cartBookCity"];
    }
    
    [tableView deselectRowAtIndexPath:indexPath animated:YES];
}

#pragma mark - HCSearchBarViewViewDelegate

- (void)searchBtnClick:(HCSearchBarView *)sender searchKey:(NSString *)searchStr
{
    [self.searchData removeAllObjects];
    self.searchDataKeys = nil;
    
    if(searchStr && [searchStr length] > 0){
        _isSearch = YES;
        NSMutableDictionary *resultDic = [NSMutableDictionary dictionary];
        NSMutableArray *resultArr = [NSMutableArray array];
        for (NSDictionary *cityDic in self.chineseLetterCities) {
            NSString *chineseName = [cityDic objectForKey:@"chineseName"];
            if ([self IsChinese:searchStr]){
                NSRange chineseRange = [chineseName rangeOfString:searchStr options:NSCaseInsensitiveSearch];
                if(chineseRange.location != NSNotFound){
                    [resultArr addObject:chineseName];
                }
            }else{
                NSString *initialWords = [cityDic objectForKey:@"initialWords"];
                NSString *initialLetters = [cityDic objectForKey:@"initialLetters"];
                if([initialWords hasPrefix:[searchStr lowercaseString]] || [initialLetters isEqualToString:[searchStr lowercaseString]]){
                    [resultArr addObject:chineseName];
                }
            }
        }
        if([resultArr count]){
            [resultDic setValue:resultArr forKey:@"搜索结果"];
        }else{
            [resultDic setValue:resultArr forKey:@"无结果"];
        }
        [self.searchData addObject:resultDic];
        self.searchDataKeys = [self keysCities:_searchData isSectionTitle:NO];
    }else{
        _isSearch = NO;
    }
    [_tableView reloadData];
}

#pragma mark - handle data method

- (NSArray *)keysCities:(NSArray *)citiesData isSectionTitle:(BOOL)flag
{
    NSMutableArray *array = [NSMutableArray array];
    if(citiesData){
        for (NSDictionary *cityDic in citiesData) {
            NSArray *keys = [cityDic allKeys];
            [array addObject:[keys firstObject]];
            if(flag && [[keys firstObject] containsString:@"热门"]){
                [array removeObject:[keys firstObject]];
            }
        }
    }
    return array;
}

- (NSArray *)citySectionAtIndex:(NSInteger)section
{
    NSArray *citySection = nil;
    if(_isSearch){
        NSString *key = [_searchDataKeys objectAtIndex:section];
        NSDictionary *cityDic = [_searchData objectAtIndex:section];
        citySection = [cityDic objectForKey:key];
    }else{
        NSString *key = [_cityKeys objectAtIndex:section];
        NSDictionary *cityDic = [_cities objectAtIndex:section];
        citySection = [cityDic objectForKey:key];
    }
    return citySection;
}

-(BOOL)IsChinese:(NSString *)str {
    for(int i=0; i< [str length];i++){
        int a = [str characterAtIndex:i];
        if( a > 0x4e00 && a < 0x9fff){
            return YES;
        }
    }
    return NO;
}

@end
