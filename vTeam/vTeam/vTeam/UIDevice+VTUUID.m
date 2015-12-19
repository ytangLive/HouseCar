//
//  UIDevice+VTUUID.m
//  vTeam
//
//  Created by zhang hailong on 13-6-9.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#import "UIDevice+VTUUID.h"

#import "NSString+VTMD5String.h"

#include <sys/socket.h> // Per msqr
#include <sys/sysctl.h>
#include <net/if.h>
#include <net/if_dl.h>
#import <arpa/inet.h>
#import <netdb.h>
#import <net/if.h>
#import <ifaddrs.h>
#import <unistd.h>
#import <dlfcn.h>
#include <uuid/uuid.h>

#import <SystemConfiguration/SystemConfiguration.h>


@implementation UIDevice (VTUUID)

SCNetworkConnectionFlags VTConnectionFlags = 0;
SCNetworkReachabilityRef VTReachability = nil;

-(NSString * ) vtUniqueIdentifier{

    static NSString * uniqueIdentifier = nil;
    
    if(uniqueIdentifier == nil){
        
        NSBundle * bundle = [NSBundle mainBundle];
        
        NSMutableDictionary * query = [NSMutableDictionary dictionaryWithObjectsAndKeys:(NSString *)kSecClassGenericPassword,(NSString *)kSecClass
                                , @"vtUniqueIdentifier",(NSString *) kSecAttrAccount
                                , [bundle bundleIdentifier],(NSString *) kSecAttrService
                                , @"vtUniqueIdentifier",(NSString *) kSecAttrLabel
                                , [NSNumber numberWithBool:YES],(NSString *) kSecReturnData
                                , nil];
        
        NSData * data = nil;
        
        OSStatus status = SecItemCopyMatching((CFDictionaryRef)query,(CFTypeRef *) & data);
        
        [data autorelease];
        
        if(status != noErr){
            SecItemDelete((CFDictionaryRef) query);
            data = nil;
        }
        
        if([data length] == 32){
            uniqueIdentifier = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
        }
        else{
            
            if([self respondsToSelector:@selector(identifierForVendor)]){
                uniqueIdentifier = [[[[self identifierForVendor] UUIDString] vtMD5String] retain];
            }
            
            if(uniqueIdentifier == nil){
                uuid_t uu = {0};
                uuid_string_t suuid;
                uuid_generate(uu);
                uuid_unparse(uu, suuid);
                uniqueIdentifier = [[[NSData dataWithBytes:suuid length:sizeof(suuid)] vtMD5String] retain];
            }
            
            data = [uniqueIdentifier dataUsingEncoding:NSUTF8StringEncoding];
            
            [query removeObjectForKey:(NSString *)kSecReturnData];
            [query setValue:data forKey:(NSString *)kSecValueData];
            
            SecItemAdd( (CFDictionaryRef) query, nil);
        }
    }
    
    return uniqueIdentifier;
}

-(NSString *) MACAddress{
    
    int					mib[6];
	size_t				len;
	char				*buf;
	unsigned char		*ptr;
	struct if_msghdr	*ifm;
	struct sockaddr_dl	*sdl;
	
	mib[0] = CTL_NET;
	mib[1] = AF_ROUTE;
	mib[2] = 0;
	mib[3] = AF_LINK;
	mib[4] = NET_RT_IFLIST;
	
	if ((mib[5] = if_nametoindex("en0")) == 0) {
		printf("Error: if_nametoindex error\n");
		return NULL;
	}
	
	if (sysctl(mib, 6, NULL, &len, NULL, 0) < 0) {
		printf("Error: sysctl, take 1\n");
		return NULL;
	}
	
	if ((buf = malloc(len)) == NULL) {
		printf("Could not allocate memory. error!\n");
		return NULL;
	}
	
	if (sysctl(mib, 6, buf, &len, NULL, 0) < 0) {
		printf("Error: sysctl, take 2");
		return NULL;
	}
	
	ifm = (struct if_msghdr *)buf;
	sdl = (struct sockaddr_dl *)(ifm + 1);
	ptr = (unsigned char *)LLADDR(sdl);
	// NSString *outstring = [NSString stringWithFormat:@"%02x:%02x:%02x:%02x:%02x:%02x", *ptr, *(ptr+1), *(ptr+2), *(ptr+3), *(ptr+4), *(ptr+5)];
	NSString *outstring = [NSString stringWithFormat:@"%02x%02x%02x%02x%02x%02x", *ptr, *(ptr+1), *(ptr+2), *(ptr+3), *(ptr+4), *(ptr+5)];
	free(buf);
	return [outstring uppercaseString];
}

- (NSString *) WIFIAddress
{
	BOOL success;
	struct ifaddrs * addrs;
	const struct ifaddrs * cursor;
	
	success = getifaddrs(&addrs) == 0;
	if (success) {
		cursor = addrs;
		while (cursor != NULL) {
			// the second test keeps from picking up the loopback address
			if (cursor->ifa_addr->sa_family == AF_INET && (cursor->ifa_flags & IFF_LOOPBACK) == 0)
			{
				NSString *name = [NSString stringWithUTF8String:cursor->ifa_name];
				if ([name isEqualToString:@"en0"])  // Wi-Fi adapter
                    freeifaddrs(addrs);
					return [NSString stringWithUTF8String:inet_ntoa(((struct sockaddr_in *)cursor->ifa_addr)->sin_addr)];
			}
			cursor = cursor->ifa_next;
		}
		freeifaddrs(addrs);
	}
	return nil;
}

- (void) pingReachabilityInternal
{
	if (!VTReachability)
	{
		BOOL ignoresAdHocWiFi = NO;
		struct sockaddr_in ipAddress;
		bzero(&ipAddress, sizeof(ipAddress));
		ipAddress.sin_len = sizeof(ipAddress);
		ipAddress.sin_family = AF_INET;
		ipAddress.sin_addr.s_addr = htonl(ignoresAdHocWiFi ? INADDR_ANY : IN_LINKLOCALNETNUM);
        
		/* Can also create zero addy
		 struct sockaddr_in zeroAddress;
		 bzero(&zeroAddress, sizeof(zeroAddress));
		 zeroAddress.sin_len = sizeof(zeroAddress);
		 zeroAddress.sin_family = AF_INET; */
		
		VTReachability = SCNetworkReachabilityCreateWithAddress(kCFAllocatorDefault, (struct sockaddr *)&ipAddress);
	}
	
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        SCNetworkReachabilityGetFlags(VTReachability, &VTConnectionFlags);
    });
}

- (BOOL) isNetworkAvailable
{
	[self pingReachabilityInternal];
	BOOL isReachable = ((VTConnectionFlags & kSCNetworkFlagsReachable) != 0);
    BOOL needsConnection = ((VTConnectionFlags & kSCNetworkFlagsConnectionRequired) != 0);
    return (isReachable && !needsConnection) ? YES : NO;
}

- (BOOL) isActiveWWAN
{
	if (![self isNetworkAvailable]) return NO;
	return ((VTConnectionFlags & kSCNetworkReachabilityFlagsIsWWAN) != 0);
}

-(BOOL) isActiveWLAN{
    return [self WIFIAddress] != nil;
}

- (NSString *) platform
{
    size_t size;
    sysctlbyname("hw.machine", NULL, &size, NULL, 0);
    char *machine = malloc(size);
    sysctlbyname("hw.machine", machine, &size, NULL, 0);
    NSString *platform = [NSString stringWithCString:machine encoding:NSUTF8StringEncoding];
    free(machine);
    return platform;
}

- (NSString *)getReturnPlat:(NSString *)platform
{
    // http://theiphonewiki.com/wiki/Models
    if ([platform isEqualToString:@"iPhone1,1"])    return @"iPhone 2G";
    if ([platform isEqualToString:@"iPhone1,2"])    return @"iPhone 3G";
    if ([platform isEqualToString:@"iPhone2,1"])    return @"iPhone 3GS";
    if ([platform isEqualToString:@"iPhone3,1"])    return @"iPhone 4";
    if ([platform isEqualToString:@"iPhone3,2"])    return @"iPhone 4";
    if ([platform isEqualToString:@"iPhone3,3"])    return @"iPhone 4 (CDMA)";
    if ([platform isEqualToString:@"iPhone4,1"])    return @"iPhone 4S";
    if ([platform isEqualToString:@"iPhone5,1"])    return @"iPhone 5";
    if ([platform isEqualToString:@"iPhone5,2"])    return @"iPhone 5 (GSM+CDMA)";
    if ([platform isEqualToString:@"iPhone5,3"])    return @"iPhone 5C";
    if ([platform isEqualToString:@"iPhone5,4"])    return @"iPhone 5C (Global)";
    if ([platform isEqualToString:@"iPhone6,1"])    return @"iPhone 5S";
    if ([platform isEqualToString:@"iPhone6,2"])    return @"iPhone 5S (Global)";
    
    if ([platform isEqualToString:@"iPod1,1"])      return @"iPod Touch (1 Gen)";
    if ([platform isEqualToString:@"iPod2,1"])      return @"iPod Touch (2 Gen)";
    if ([platform isEqualToString:@"iPod3,1"])      return @"iPod Touch (3 Gen)";
    if ([platform isEqualToString:@"iPod4,1"])      return @"iPod Touch (4 Gen)";
    if ([platform isEqualToString:@"iPod5,1"])      return @"iPod Touch (5 Gen)";
    
    if ([platform isEqualToString:@"iPad1,1"])      return @"iPad";
    if ([platform isEqualToString:@"iPad1,2"])      return @"iPad 3G";
    if ([platform isEqualToString:@"iPad2,1"])      return @"iPad 2 (WiFi)";
    if ([platform isEqualToString:@"iPad2,2"])      return @"iPad 2";
    if ([platform isEqualToString:@"iPad2,3"])      return @"iPad 2 (CDMA)";
    if ([platform isEqualToString:@"iPad2,4"])      return @"iPad 2";
    if ([platform isEqualToString:@"iPad2,5"])      return @"iPad Mini (WiFi)";
    if ([platform isEqualToString:@"iPad2,6"])      return @"iPad Mini";
    if ([platform isEqualToString:@"iPad2,7"])      return @"iPad Mini (GSM+CDMA)";
    if ([platform isEqualToString:@"iPad3,1"])      return @"iPad 3 (WiFi)";
    if ([platform isEqualToString:@"iPad3,2"])      return @"iPad 3 (GSM+CDMA)";
    if ([platform isEqualToString:@"iPad3,3"])      return @"iPad 3";
    if ([platform isEqualToString:@"iPad3,4"])      return @"iPad 4 (WiFi)";
    if ([platform isEqualToString:@"iPad3,5"])      return @"iPad 4";
    if ([platform isEqualToString:@"iPad3,6"])      return @"iPad 4 (GSM+CDMA)";
    
    if ([platform isEqualToString:@"i386"])         return @"Simulator";
    if ([platform isEqualToString:@"x86_64"])       return @"Simulator";
    return platform;
}

- (NSString *) standardPlat
{
    return [self getReturnPlat:[self platform]];
}

@end
