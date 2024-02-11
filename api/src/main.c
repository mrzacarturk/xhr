#include <stdlib.h>           /* exit() */
#include <stdio.h>            /* printf() family */
#include <unistd.h>           /* read(), close() etc. */
// #include <sys/types.h>     /* socket options */
#include <sys/socket.h>       /* socket() */
#include <arpa/inet.h>        /* struct sockaddr_in */
#include <stdarg.h>           /* variadic */
#include <string.h>           /* str....() */



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
#define ERR_CODE_ACCEPTSOCK_SETLINGER_FAIL 7
#define ERR_CODE_ACCEPTSOCK_READ_FAIL 8
#define ERR_CODE_ACCEPTSOCK_SEND_FAIL 9
#define ERR_CODE_ACCEPTSOCK_CLOSE_FAIL 10
#define ERR_CODE_APISOCK_CLOSE 11
#define ERR_CODE_MALLOC_FAIL 12

void
utils_putchar(int fd, char c)
{
    write(fd, &c, 1);
}

size_t
utils_strlen(const char *str)
{
    size_t len = 0;
    while (*(str + len)) {
        len++;
    }

    return (len);
}

void
utils_putstr(int fd, const char *str)
{
    write(fd, str, utils_strlen(str));
}

void
utils_putnbr(int fd, int nb)
{
    if (nb < 0) {
        utils_putchar(fd, '-');
        if (nb == ~0x7FFFFFFF) {
            nb = -147483648;
            utils_putchar(fd, '2');
        }
        nb *= -1;
    }
    if (nb >= 10) {
        utils_putnbr(fd, (nb / 10));
        nb = (nb % 10);
    }
    if (nb < 10) {
        utils_putchar(fd, (nb + 48));
    }
}


void *
utils_memset(void *dest, int c, size_t n)
{
    while (n--) {
        *((unsigned char *)dest + n) = c;
    }
    return (dest);
}

void
message(char *fmt, ...)
{
    va_list ap;
    char c;
    char *s;
    int d;

    va_start(ap, fmt);
    while (*fmt) {
        if (*fmt == '~') {
            fmt++;
            if (*fmt == 'E') {
                utils_putstr(STDERR_FILENO, "\033[0;30m[ ERROR ]\033[0m");
            } else if (*fmt == 'W') {
                utils_putstr(STDOUT_FILENO, "\033[0;31m[ WARNING ]\033[0m");
            } else if (*fmt == 'S') {
                utils_putstr(STDOUT_FILENO, "\033[0;32m[ SUCCESS ]\033[0m");
            } else if (*fmt == 'D') {
                utils_putstr(STDOUT_FILENO, "\033[0;33m[ DEBUG ]\033[0m");
            } else if (*fmt == 's') {
                s = va_arg(ap, char *);
                utils_putstr(STDOUT_FILENO, s);
            } else if (*fmt == 'd') {
                d = va_arg(ap, int);
                utils_putnbr(STDOUT_FILENO, d);
            } else if (*fmt == 'c') {
                c = va_arg(ap, int);
                utils_putchar(STDOUT_FILENO, c);
            }
        } else {
                utils_putchar(STDOUT_FILENO, *fmt);
            }
        fmt++;
    }

    va_end(ap);
}

int
utils_strncmp(const char *a, const char *b, size_t n)
{
    size_t i = 0;
    while (*(a + i) && (*(a + i) == *(b + i)) && i < n) {
        i++;
    }

    if (i == n) {
        return (0);
    }

    return (*(a + i) - *(b + i));
}

int
find_url_index(char req[], char *search)
{
    size_t i = 0;
    size_t j;
    while (req[i]) {
        j = 0;
        while (req[i + j] == *(search + j) && *(search + j)) {
            j++;
        }
        if (*(search + j) == 0) {
            return (i);
        }
        i++;
    }
    return (0);
}










/*
 * This is the main source file of a minimalized API for simulating basic
 * scenarios of async XHR http requests in vanilla JS.
 * For that, we need a streaming socket on 127.0.0.1:PORT
 */
int
main(int argc, char *argv[])
{

    int api_bindport;

#if __DEBUG__
    api_bindport = DEBUG_PORT;
    message("~D Port set to ~d\n", DEBUG_PORT);
#else
    if (argc < 2) {
        message("~E CAPI needs a port number to run properly.\n");
        exit(ERR_CODE_NO_BINDPORT);
    }

    api_bindport = atoi(argv[1]);

    if (api_bindport < 1000 || api_bindport > 65000) {
        message("~E Given port number is out of range! [ 1000 - 65000 ]\n");
        exit(ERR_CODE_APIBINDPORT_OUT_OF_RANGE);
    }
#endif


    int APISOCK = socket(AF_INET, SOCK_STREAM, 0);
    if (APISOCK < 0) {
        message("~E CAPI socket can not be created.\n");
        exit(ERR_CODE_CREATE_APISOCK);
    }


#if __DEBUG__
    message("~D APISOCK created.\n");
#endif

    /*
        TODO
        set APISOCK options via setsockopt
    */


    struct sockaddr_in api_address;
    api_address.sin_family = AF_INET;
    api_address.sin_addr.s_addr = INADDR_ANY;
    api_address.sin_port = htons(api_bindport);


    int api_bind = bind(APISOCK, (struct sockaddr *)&api_address, sizeof(api_address));
    if (api_bind < 0) {
        message("~E Binding api_address to APISOCK has failed.\n");
        exit(ERR_CODE_APIBIND_FAIL);
    }

#if __DEBUG__
    message("~D sin_family AF_INET sin_addr.s_addr INADDR_ANY sin_port ~d binded to APISOCK.\n", DEBUG_PORT);
#endif




    int api_listen = listen(APISOCK, MAX_REQUEST_QUEUE);
    if (api_listen < 0) {
        message("~E APISOCK listen has failed.\n");
        exit(ERR_CODE_APILISTEN_FAIL);
    }

#if __DEBUG__
    message("~D APISOCK listens incoming connection requests.\n");
#endif




    // JSON
    // const char *http_header_response = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: 30\r\n\r\n\0";

    // CORS HEADER RESPONSE + RESPONSE BODY
    char http_header_response[] = "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\nAccess-Control-Allow-Methods: GET, POST, PUT, DELETE\r\nAccess-Control-Allow-Headers: Origin, X-Requested-With, Content-Type, Accept\r\nContent-Type: text/plain\r\nContent-Length: 30\r\n\r\n{ \"message\": \"wagabond capi\" }\r\n";
    size_t response_len = strlen(http_header_response);









    struct sockaddr_in cli_address;
    size_t cli_addrlen = sizeof(cli_address);
    int ACCEPTSOCK;
    int accept_setopt;
    char *DYN_buff = (char *) malloc(DEBUG_CLIDATA_BUFFSZ);

    if (DYN_buff == NULL) {
        message("~E Malloc failed for <<<<char *DYN_buff>>>>\n");
        exit(ERR_CODE_MALLOC_FAIL);
    }

    int acceptsock_readval;
    int a;
    int close_acceptsock;

// !    struct linger ling;
// !    ling.l_onoff = 1;    // Beklemeyi etkinlestir
// !    ling.l_linger = 0;   // Bekleme suresi (saniye cinsinden), sifir degeri, soketi hemen kapatir.


    struct linger acceptsock_ling;
    acceptsock_ling.l_onoff = 1;
    acceptsock_ling.l_linger = 0;

#if __DEBUG__
    message("~D ACCEPTSOCK linger settings l_onoff ~d l_linger ~d\n", 1, 0);
#endif

    int requested_urllength;
    char *DYN_requested_url;


    while (1) {
        ACCEPTSOCK = accept(APISOCK, (struct sockaddr *)&cli_address,  (socklen_t *)&cli_addrlen);

        if (ACCEPTSOCK < 0) {
            message("~E Connection request from client to APISOCK has failed.\n");
            exit(ERR_CODE_ACCEPTSOCK_FAIL);
        }

        // ! SO_LINGER secenegi aktiflestiriliyor.
        accept_setopt = setsockopt(ACCEPTSOCK, SOL_SOCKET, SO_LINGER, &acceptsock_ling, sizeof(struct linger));
        if (accept_setopt < 0) {
            message("~E Accept socket set options linger failed.\n");
            exit(ERR_CODE_ACCEPTSOCK_SETLINGER_FAIL);
        }
#if __DEBUG__
    message("~D ACCEPTSOCK accept_setopt SO_LINGER settings has been set.\n");
#endif




#if __DEBUG__
        message("~D ACCEPTSOCK created successfully.\n");
#endif
        // ! memset fail is impossible.
        memset(DYN_buff, 0, DEBUG_CLIDATA_BUFFSZ);
        acceptsock_readval = read(ACCEPTSOCK, DYN_buff, DEBUG_CLIDATA_BUFFSZ);
        if (acceptsock_readval < 0) {
            message("~E Error while ACCEPTSOCK is reading buffer from client.\n");
            exit(ERR_CODE_ACCEPTSOCK_READ_FAIL);
        }

#if __DEBUG__
    message("~D read success, ACCEPTSOCK gets into DYN_buff.\n");
#endif


        requested_urllength = find_url_index(DYN_buff, " HTTP/1.1\r\n");
        DYN_requested_url = (char *) malloc(sizeof(char) * requested_urllength);
        strncpy(DYN_requested_url, DYN_buff, requested_urllength);

        message("~S Requested <<<~s>>>.\n", DYN_requested_url);


// ! Rotalama sistemi !! Bu kisim ileride degistirilecek.
// !         if (strncmp(DYN_buff, "GET / HTTP/1.1", 14) != 0) {
// !             close_acceptsock = close(ACCEPTSOCK);
// !             if (close_acceptsock < 0) {
// !                 message("~E ACCEPTSOCK close failed.\n");
// !                 exit(ERR_CODE_ACCEPTSOCK_CLOSE_FAIL);
// !             }
// !             if (strncmp(DYN_buff, "GET /break", 10) == 0) {
// !                 message("~W Requested <~s>. Breaking the law !\n", DYN_requested_url);
// !                 break ;
// !             }
// !             continue ;
// !         }

// TODO
// TODO get ve post method'larina gore aksiyon burada alinacak. duruma gore send ile istenilen veri gonderilecek.
// TODO

        free(DYN_requested_url);

#if __DEBUG__
        message("~D ACCEPTSOCK Buffer from Client data\n~s", DYN_buff);
#endif


        a = send(ACCEPTSOCK, http_header_response, response_len, 0);
        if (a < 0) {
            message("~E send error\n");
            exit(ERR_CODE_ACCEPTSOCK_SEND_FAIL);
        }

#if __DEBUG__
    message("~S http_header_response sended to ACCEPTSOCK.\n");
#endif


        close_acceptsock = close(ACCEPTSOCK);
        if (close_acceptsock < 0) {
            message("~E ACCEPTSOCK close failed.\n");
            exit(ERR_CODE_ACCEPTSOCK_CLOSE_FAIL);
        }
#if __DEBUG__
        message("~D ACCEPTSOCK closed successfully.\n");
#endif
        message("~S~S~S\n");
    }

    free(DYN_buff);








    int close_apisock = close(APISOCK);
    if (close_apisock < 0) {
        message("~E APISOCK close failed.\n");
        exit(ERR_CODE_APISOCK_CLOSE);
    }
#if __DEBUG__
    message("~D APISOCK closed successfully.\n");
#endif


    return (0);
}
