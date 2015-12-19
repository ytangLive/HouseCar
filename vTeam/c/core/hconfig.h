/*
 * hconfig.h
 *
 *  Created on: 2010-8-27
 *      Author: zhanghailong
 */

#ifndef HCONFIG_H_
#define HCONFIG_H_

#include "htypes.h"
#include "hmem.h"

#ifdef __cplusplus
extern "C" {
#endif
    
void hclibrary_init(void);
    
#ifdef __cplusplus
}
#endif

#define MAX_PATH        2048
#define MAX_READ_BUF    20480

#ifndef READONLY
#define READONLY const
#endif

#define HANDLER(h) struct __##h##__ { long unused;};  typedef struct __##h##__ * h;
#define DEC_HANDLER(h)  struct __##h##__

#ifdef __SYMBIAN32__

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <netdb.h>
#include <sys/socket.h>
#include <utime.h>
#include <zlib.h>
#include <pthread.h>
#include <stdarg.h>

#define hsocket_t	int
#define closesocket(a) close(a)

#define HAS_PTHREAD

#elif defined(__MACOS__)

#pragma pack(4)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>
#include <netinet/in.h>
#include <dns.h>
#include <dns_util.h>
#include <resolv.h>
#include <sys/stat.h>
#include <utime.h>
#include <zlib.h>
#include <stdarg.h>
#include <dlfcn.h> 
#include <assert.h>
#include <fcntl.h>
#include <netdb.h>
#include <errno.h>
#include <math.h>
#include <net/if.h>
#include <net/if_dl.h>
#include <sys/ioctl.h>
#include <sys/sockio.h>
#include <net/ethernet.h>
#include <sys/types.h>
#ifdef __OPENSSL__
#include <openssl/ssl.h>
#include <openssl/crypto.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#endif

#define closesocket(a) close(a)
#define hsocket_t	int

#define EXPORT

#define UNUSED(a) (void)a;

#define HAS_PTHREAD

#define BITS_LOW
//#define BITS_HIGH

#ifndef PATH_MAX
#define PATH_MAX    2048
#endif

#ifndef PATH_SPLIT
#define PATH_SPLIT  '/'
#endif

#elif defined(__WIN32__)

#pragma pack(4)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/utime.h>
#include <memory.h>
#include <windows.h>
#include <winsock.h>
#include <io.h>
#include "zlib/zlib.h"
#include <stdarg.h>
#include <assert.h>
#include <io.h>

#define mkdir(a,b) mkdir(a)

#define hsocket_t	int

#define sleep(a) Sleep((a) * 1000)
#define usleep(a) Sleep(a)

#define atoll atol

#define BITS_LOW

typedef long socklen_t;

#ifndef STDIN_FILENO
#define STDIN_FILENO 0
#endif

#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif

#ifndef PATH_MAX
#define PATH_MAX    1024
#endif

#ifndef PATH_SPLIT
#define PATH_SPLIT  '\\'
#endif

#elif defined __LINUX__

#pragma pack(4)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <utime.h>
#include <zlib.h>
#include <stdarg.h>
#include <dlfcn.h> 
#include <assert.h>
#include <fcntl.h>
#include <netdb.h>
#include <errno.h>
#include <math.h>
#include <net/if.h>
#ifdef __OPENSSL__
#include <openssl/ssl.h>
#include <openssl/crypto.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#endif

#define closesocket(a) close(a)
#define hsocket_t	int

#define EXPORT

#define UNUSED(a) (void)a;

#define HAS_PTHREAD

#define BITS_LOW
//#define BITS_HIGH

#ifndef PATH_MAX
#define PATH_MAX    1024
#endif

#ifndef PATH_SPLIT
#define PATH_SPLIT  '/'
#endif

#ifndef MIN
#define	MIN(a,b) (((a)<(b))?(a):(b))
#endif /* MIN */
#ifndef MAX
#define	MAX(a,b) (((a)>(b))?(a):(b))
#endif	/* MAX */


#elif defined __QT__


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <utime.h>
#include <zlib.h>
#include <stdarg.h>
#include <dlfcn.h>
#include <assert.h>
#include <netdb.h>
#include <fcntl.h>



#define closesocket(a) close(a)
#define hsocket_t	int

#define EXPORT

#define HAS_PTHREAD

#elif defined(__ANDROID__)

#pragma pack(4)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>
#include <netinet/in.h>
#include <resolv.h>
#include <sys/stat.h>
#include <utime.h>
#include <zlib.h>
#include <stdarg.h>
#include <dlfcn.h> 
#include <assert.h>
#include <fcntl.h>
#include <netdb.h>
#include <math.h>
#include <errno.h>
#ifdef __OPENSSL__
#include <openssl/ssl.h>
#include <openssl/crypto.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#endif

#define closesocket(a) close(a)
#define hsocket_t	int

#define EXPORT

#define UNUSED(a) (void)a;

#define HAS_PTHREAD

#define BITS_LOW
//#define BITS_HIGH

//#define hatoi atoi

#ifndef PATH_MAX
#define PATH_MAX    1024
#endif

#ifndef PATH_SPLIT
#define PATH_SPLIT  '/'
#endif

#ifndef MIN
#define MIN(a,b)	((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a,b)	((a) > (b) ? (a) : (b))
#endif

#define log2(a)		(log(a) / log(2.0))

#endif




#endif /* HCONFIG_H_ */
