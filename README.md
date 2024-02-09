# NOTES

## Example HTTP Header - G**gle Chrome
```
GET / HTTP/1.1
Host: 127.0.0.1:8080
Connection: keep-alive
Cache-Control: max-age=0
sec-ch-ua: "Not A(Brand";v="99", "Google Chrome";v="121", "Chromium";v="121"
sec-ch-ua-mobile: ?0
sec-ch-ua-platform: "Windows"
Upgrade-Insecure-Requests: 1
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/121.0.0.0 Safari/537.36
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7
Sec-Fetch-Site: none
Sec-Fetch-Mode: navigate
Sec-Fetch-User: ?1
Sec-Fetch-Dest: document
Accept-Encoding: gzip, deflate, br
Accept-Language: de-DE,de;q=0.9,en-US;q=0.8,en;q=0.7
```

## Makefile
```sh
make all        # release build
make debug      # debug build
make clean      # remove output files
make run        # run executable only
make re         # clean all
make redebug    # clean debug run
```



## Variables


```c

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
#define ERR_CODE_ACCEPTSOCK_CLOSE 10
#define ERR_CODE_APISOCK_CLOSE 11
#define ERR_CODE_MALLOC_FAIL 12


int api_bindport;
int APISOCK = socket(AF_INET, SOCK_STREAM, 0);
struct sockaddr_in api_address;
    api_address.sin_family = AF_INET;
    api_address.sin_addr.s_addr = INADDR_ANY;
    api_address.sin_port = htons(api_bindport);
int api_bind = bind(APISOCK, (struct sockaddr *)&api_address, sizeof(api_address));
int api_listen = listen(APISOCK, MAX_REQUEST_QUEUE);
char http_header_response[] = "HTTP/1.1 200 OK\r\n ... { \"message\": \"wagabond capi\" }\r\n\0";
size_t response_len = strlen(http_header_response);



struct sockaddr_in cli_address;
size_t cli_addrlen = sizeof(cli_address);
int ACCEPTSOCK;
int accept_setopt;
char *DYN_buff = (char *) malloc(DEBUG_CLIDATA_BUFFSZ);

int acceptsock_readval;
int a;
int close_acceptsock;
struct linger acceptsock_ling;
    acceptsock_ling.l_onoff = 1;
    acceptsock_ling.l_linger = 0;

int requested_urllength;
char *DYN_requested_url;

while (1)
{
    ACCEPTSOCK = accept(APISOCK, (struct sockaddr *)&cli_address,  (socklen_t *)&cli_addrlen);
    accept_setopt = setsockopt(ACCEPTSOCK, SOL_SOCKET, SO_LINGER, &acceptsock_ling, sizeof(struct linger));
    acceptsock_readval = read(ACCEPTSOCK, DYN_buff, DEBUG_CLIDATA_BUFFSZ);

    requested_urllength = find_url_index(DYN_buff, " HTTP/1.1\r\n");
    DYN_requested_url = (char *) malloc(sizeof(char) * i);

    a = send(ACCEPTSOCK, block, blocklen, 0);
    close_acceptsock = close(ACCEPTSOCK);
}

int close_apisock = close(APISOCK);


```

#### _**“We make an idol of our fear and that idol we call God.”**_