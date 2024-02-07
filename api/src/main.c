#include <stdlib.h>        /* exit() */
#include <stdio.h>         /* printf() family */
#include <unistd.h>        /* read(), close() etc. */
// #include <sys/types.h>     /* socket options */
#include <sys/socket.h>    /* socket() */
#include <arpa/inet.h>     /* struct sockaddr_in */
#include <string.h>        /* strlen() */

#define __DEBUG__ 1

#if __DEBUG__
# define DEBUG_PORT 8080
# define DEBUG_CLIDATA_BUFFSZ 9999
#endif


#define MAX_REQUEST_QUEUE 5


#define ERR_CODE_NO_BINDPORT 1
#define ERR_CODE_CREATE_APISOCK 2
#define ERR_CODE_APIBINDPORT_OUT_OF_RANGE 3
#define ERR_CODE_APIBIND_FAIL 4
#define ERR_CODE_APILISTEN_FAIL 5
#define ERR_CODE_ACCEPTSOCK_FAIL 6
#define ERR_CODE_ACCEPTSOCK_READ_FAIL 7
#define ERR_CODE_ACCEPTSOCK_CLOSE 8
#define ERR_CODE_APISOCK_CLOSE 9

/*
 * This is the main source file of a minimalized API for simulating basic
 * scenarios of async XHR http requests in vanilla JS.
 * For that, we need a streaming socket on 127.0.0.1:PORT
 */
int
main(int argc, char *argv[])
{
    char *debuginfo_str = "[ DEBUG_INFO ] \0";
    int api_bindport;


#if __DEBUG__
    api_bindport = DEBUG_PORT;
#else
    if (argc < 2) {
        dprintf(STDERR_FILENO, "No binding port for socket! [ 1000 - 65000 ]\n");
        exit(ERR_CODE_NO_BINDPORT);
    }

    api_bindport = atoi(argv[1]);
    if (api_bindport < 1000 || api_bindport > 65000) {
        dprintf(STDERR_FILENO, "Given port number is out of range! [ 1000 - 65000 ]\n");
        exit(ERR_CODE_APIBINDPORT_OUT_OF_RANGE);
    }
#endif


    int APISOCK = socket(AF_INET, SOCK_STREAM, 0);


    if (APISOCK < 0) {
        dprintf(STDERR_FILENO, "ERROR while creating API socket.\n");
        exit(ERR_CODE_CREATE_APISOCK);
    }


#if __DEBUG__
    dprintf(STDOUT_FILENO, "%sAPISOCK created.\n", debuginfo_str);
#endif

    /*
        TODO
        setsockopt
    */


    struct sockaddr_in api_address;
    api_address.sin_family = AF_INET;
    api_address.sin_addr.s_addr = INADDR_ANY;
    api_address.sin_port = htons(api_bindport);



    int api_bind = bind(APISOCK, (struct sockaddr *)&api_address, sizeof(api_address));

#if __DEBUG__
    dprintf(STDOUT_FILENO, "%sINADDR_ANY binded to APISOCK.\n", debuginfo_str);
#endif

    if (api_bind < 0) {
        dprintf(STDERR_FILENO, "Binding api_address to APISOCK has failed.\n");
        exit(ERR_CODE_APIBIND_FAIL);
    }


    int api_listen = listen(APISOCK, MAX_REQUEST_QUEUE);
    if (api_listen < 0) {
        dprintf(STDERR_FILENO, "APISOCK listen has failed.\n");
        exit(ERR_CODE_APILISTEN_FAIL);
    }

#if __DEBUG__
    dprintf(STDOUT_FILENO, "%sAPISOCK listens incoming connection requests.\n", debuginfo_str);
#endif



// TODO    while (1) {
// TODO        struct sockaddr_in client_address;
// TODO        int client_addrlen = sizeof(client_address);
// TODO        int ACCEPTSOCK = accept(APISOCK, (struct sockaddr *)&client_address, (socklen_t *)&client_addrlen);
// TODO
// TODO        if (ACCEPTSOCK < 0) {
// TODO            perror("Connection request to APISOCK has failed.");
// TODO            exit(ERR_CODE_ACCEPTSOCK_FAIL);
// TODO        }
// TODO
// TODO        char buff[1024] = {0};
// TODO        int acceptsock_readval = read(ACCEPTSOCK, buff, sizeof(buff));
// TODO        if (acceptsock_readval < 0) {
// TODO            perror("Error while ACCEPTSOCK is reading from client.");
// TODO            exit(ERR_CODE_ACCEPTSOCK_READ_FAIL);
// TODO        }
// TODO
// TODO        printf("Received request from client:\n%s\n", buff);
// TODO
// TODO        const char *http_response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 12\r\n\r\nHello, World!";
// TODO        int send_result = send(ACCEPTSOCK, http_response, strlen(http_response), 0);
// TODO
// TODO        if (send_result < 0) {
// TODO            perror("Failed to send response to client.");
// TODO            exit(ERR_CODE_SEND_FAIL);
// TODO        }
// TODO
// TODO        close(ACCEPTSOCK);
// TODO    }

// ! /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    // JSON
    // const char *http_header_response = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: 30\r\n\r\n\0";

    // CORS
    const char *http_header_response = "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\nAccess-Control-Allow-Methods: GET, POST, PUT, DELETE\r\nAccess-Control-Allow-Headers: Origin, X-Requested-With, Content-Type, Accept\r\nContent-Type: text/plain\r\nContent-Length: 30\r\n\r\n\0";
    const char *http_response_body = "{ \"message\": \"wagabond capi\" }\r\n\0";
    size_t blocklen = strlen(http_header_response) + strlen(http_response_body) + 1;
    char *block = (char *) malloc(blocklen);
    strcat(block, http_header_response);
    strcat(block, http_response_body);


    while (1) {
        struct sockaddr_in cli_address;
        size_t cli_addrlen = sizeof(cli_address);
        int ACCEPTSOCK = accept(APISOCK, (struct sockaddr *)&cli_address,  (socklen_t *)&cli_addrlen);

        if (ACCEPTSOCK < 0) {
            dprintf(STDERR_FILENO, "Connection request from client to APISOCK has failed.\n");
            exit(ERR_CODE_ACCEPTSOCK_FAIL);
        }

#if __DEBUG__
        dprintf(STDOUT_FILENO, "%sAccept socket ACCEPTSOCK created successfully.\n", debuginfo_str);
#endif
        char buff[DEBUG_CLIDATA_BUFFSZ] = { 0 };
        int acceptsock_readval = read(ACCEPTSOCK, buff, DEBUG_CLIDATA_BUFFSZ);
        if (acceptsock_readval < 0) {
            dprintf(STDOUT_FILENO, "Error while ACCEPTSOCK is reading from client.\n");
            exit(ERR_CODE_ACCEPTSOCK_READ_FAIL);
        }

        
#if __DEBUG__
        dprintf(STDOUT_FILENO, "%sACCEPTSOCK Buffer from Client data\n================\n%s================\n", debuginfo_str, buff);
#endif

        if (strncmp(buff, "GET / HTTP/1.1", 14) != 0) {
            dprintf(STDERR_FILENO, "Requested another location than '/', skipping.\n");
            close(ACCEPTSOCK);
            continue ;
        }


        printf("%s\n", block);
        int a = send(ACCEPTSOCK, block, blocklen, 0);
        if (a < 0) {
            dprintf(STDERR_FILENO, "send error\n");
        }
        


        int close_acceptsock = close(ACCEPTSOCK);
        if (close_acceptsock < 0) {
            dprintf(STDERR_FILENO, "ACCEPTSOCK close failed.\n");
            exit(ERR_CODE_ACCEPTSOCK_CLOSE);
        }
#if __DEBUG__
        dprintf(STDOUT_FILENO, "%sACCEPTSOCK closed successfully.\n", debuginfo_str);
#endif
    }



    free(block);

// !     // accept creates a new socket!
// !     size_t api_addrlen = sizeof(api_address);
// ! 
// !     int ACCEPTSOCK = accept(APISOCK, (struct sockaddr *)&api_address, (socklen_t *)&api_addrlen);
// !     if (ACCEPTSOCK < 0) {
// !         dprintf(STDERR_FILENO, "Connection request to APISOCK has failed.\n");
// !         exit(ERR_CODE_ACCEPTSOCK_FAIL);
// !     }
// ! 
// ! #if __DEBUG__
// !     dprintf(STDOUT_FILENO, "%sAccept socket ACCEPTSOCK created successfully.\n", debuginfo_str);
// ! #endif
// ! 
// !     char buff[DEBUG_CLIDATA_BUFFSZ] = { 0 };
// !     int acceptsock_readval = read(ACCEPTSOCK, buff, DEBUG_CLIDATA_BUFFSZ);
// !     if (acceptsock_readval < 0) {
// !         dprintf(STDOUT_FILENO, "Error while ACCEPTSOCK is reading from client.\n");
// !         exit(ERR_CODE_ACCEPTSOCK_READ_FAIL);
// !     }
// ! 
// ! #if __DEBUG__
// !     dprintf(STDOUT_FILENO, "%sACCEPTSOCK Buffer from Client data\n================\n%s================\n", debuginfo_str, buff);
// ! #endif
// ! 
// ! 
// !     const char *http_header_response = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: 30\r\n\r\n\0";
// !     const char *http_response_body = "{ \"message\": \"Hello, world!\" }\r\n\0";
// !     size_t blocklen = strlen(http_header_response) + strlen(http_response_body) + 1;
// !     char *block = (char *) malloc(blocklen);
// ! 
// !     strcat(block, http_header_response);
// !     strcat(block, http_response_body);
// !     printf("%s\n", block);
// !     int a = send(ACCEPTSOCK, block, blocklen, 0);
// !     if (a < 0) {
// !         dprintf(STDERR_FILENO, "send error\n");
// !     }
// !     free(block);
// ! 
// ! 
// !     int close_acceptsock = close(ACCEPTSOCK);
// !     if (close_acceptsock < 0) {
// !         dprintf(STDERR_FILENO, "ACCEPTSOCK close failed.\n");
// !         exit(ERR_CODE_ACCEPTSOCK_CLOSE);
// !     }
// ! #if __DEBUG__
// !     dprintf(STDOUT_FILENO, "%sACCEPTSOCK closed successfully.\n", debuginfo_str);
// ! #endif

// ! /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    int close_apisock = close(APISOCK);
    if (close_apisock < 0) {
        dprintf(STDERR_FILENO, "APISOCK close failed.\n");
        exit(ERR_CODE_APISOCK_CLOSE);
    }
#if __DEBUG__
    dprintf(STDOUT_FILENO, "%sAPISOCK closed successfully.\n", debuginfo_str);
#endif


    return (0);
}

