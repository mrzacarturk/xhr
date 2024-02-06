#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#define __DEBUG__ 0

#define ERR_CODE_NO_PORT 1
#define ERR_CODE_CREATE_APISOCK 2
#define ERR_CODE_CLOSE_APISOCK 3
#define ERR_CODE_APIBINDPORT_OUT_OF_RANGE 4
#define ERR_CODE_APIBIND_FAIL 5

/*
 * This is the main source file of a minimalized API for simulating basic
 * scenarios of async XHR http requests in vanilla JS.
 * For that, we need a streaming socket on localhost:[ARGV1]
 *
 */

int
main(int argc, char *argv[] , char *envp)
{
    if (argc < 2) {
        dprintf(STDERR_FILENO, "No binding port for socket! [ 1000 - 65000 ]\n");
        exit(ERR_CODE_NO_PORT);
    }

    const int api_bind_port = (const int) atoi(argv[1]);
    if (api_bind_port < 1000 || api_bind_port > 65000) {
        dprintf(STDERR_FILENO, "Given port number is out of range! [ 1000 - 65000 ]\n");
        exit(ERR_CODE_APIBINDPORT_OUT_OF_RANGE);
    }

    int APISOCK = socket(AF_INET, SOCK_STREAM, 0);
    if (APISOCK < 0) {
        dprintf(STDERR_FILENO, "ERROR while creating API socket.\n");
        exit(ERR_CODE_CREATE_APISOCK);
    }

    struct sockaddr_in api_address;
    api_address.sin_family = AF_INET;
    api_address.sin_addr.s_addr = INADDR_ANY;
    api_address.sin_port = htons(api_bind_port);

//     if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
//         perror("bind failed");
//         exit(EXIT_FAILURE);
//     }

    int api_bind = bind(APISOCK, (struct sockaddr *)&api_address, sizeof(api_address));
    if (api_bind < 0) {
        dprintf(STDERR_FILENO, "API bind failed.\n");

        exit(ERR_CODE_APIBIND_FAIL);
    }

    int sock_close = close(APISOCK);
    if (sock_close < 0) {
        dprintf(STDERR_FILENO, "ERROR while closing API socket.\n");
        exit(ERR_CODE_CLOSE_APISOCK);
    }

#if __DEBUG__
    dprintf(STDOUT_FILENO, "Socked opened and then closed successfully.\n");
#endif
    return (0);
}

