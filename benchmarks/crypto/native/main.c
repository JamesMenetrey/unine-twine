#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>

#include "wolfssl/ssl.h"
#include "wolfssl/wolfcrypt/settings.h"
#include "wolfssl/wolfcrypt/types.h"

#ifdef HAVE_WASI_SOCKET
#include "client-tls.h"
#include "server-tls.h"
#endif /* HAVE_WASI_SOCKET */

#ifdef HAVE_WOLFSSL_TEST
#include "wolfcrypt/test/test.h"
#endif /* HAVE_WOLFSSL_TEST */

#ifdef HAVE_WOLFSSL_BENCHMARK
#include "wolfcrypt/benchmark/benchmark.h"
#endif /* HAVE_WOLFSSL_BENCHMARK */

typedef struct func_args {
    int    argc;
    char** argv;
    int    return_code;
} func_args;

int main(int argc, char** argv) {
    func_args args = { 0 };

    /* only print off if no command line arguments were passed in */
	if (argc != 2 || strlen(argv[1]) != 2) {
		printf("Usage:\n"
#ifdef HAVE_WASI_SOCKET
               "\t-c Run a TLS client in enclave\n"
               "\t-s Run a TLS server in enclave\n"
#endif /* HAVE_WASI_SOCKET */

#ifdef HAVE_WOLFSSL_TEST
               "\t-t Run wolfCrypt tests\n"
#endif /* HAVE_WOLFSSL_TEST */

#ifdef HAVE_WOLFSSL_BENCHMARK
               "\t-b Run wolfCrypt benchmarks\n"
#endif /* HAVE_WOLFSSL_BENCHMARK */
               );
        return 0;
	}
    
    memset(&args,0,sizeof(args));

    switch(argv[1][1]) {

#ifdef HAVE_WASI_SOCKET
        case 'c':
            printf("Client Test:\n");
            client_connect();
            break;

        case 's':
            printf("Server Test:\n");
            server_connect();
            break;
#endif /* HAVE_WASI_SOCKET */

#ifdef HAVE_WOLFSSL_TEST
        case 't':
            printf("Crypt Test:\n");
            wolfcrypt_test(&args);
            printf("Crypt Test: Return code %d\n", args.return_code);
            break;
#endif /* HAVE_WOLFSSL_TEST */

#ifdef HAVE_WOLFSSL_BENCHMARK
       case 'b':
            benchmark_test(&args);
            break;
#endif /* HAVE_WOLFSSL_BENCHMARK */
        default:
            printf("Unrecognized option set!\n");
            break;
    }

    return 0;
}