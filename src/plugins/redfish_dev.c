/*
* Copyright 2017 Hewlett Packard Enterprise Development LP
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
* 1. Redistributions of source code must retain the above copyright notice, 
* this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice, this
* list of conditions and the following disclaimer in the documentation and/or other
* materials provided with the distribution.
* 3. Neither the name of the copyright holder nor the names of its contributors may
* be used to endorse or promote products derived from this software without specific
* prior written permission.
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,THE IMPLIED WARRANTIES 
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
* SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED 
* TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR 
* BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,WHETHER IN 
* CONTRACT,STRICT LIABILITY,OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY 
* WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#include <sys/timeb.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <poll.h>
#include <sys/types.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include "pwr_dev.h"
#include "util.h"
#include "redfish_dev.h"
#include <pwr/util.h>

#define ERR_NO -1
#define SUCCESS 0

typedef struct {
        char *attr_name;
} pwr_redfish_attr;
#define PWR_DEV_ATTR(X) ((pwr_redfish_attr *)(X))

typedef struct {
    char *device_name;
    pwr_redfish_attr *attr; 
} pwr_redfish_device;
#define PWR_R_DEVICE(X) ((pwr_redfish_device *)(X))

typedef struct {
    char *entity;    
    char *host;
    char *port;
    char *node;
    int socket_fd;
} pwr_redfish_dev_t;
#define PWR_REDFISH_DEV(X) ((pwr_redfish_dev_t *)(X))

typedef struct {
    pwr_redfish_dev_t *dev;
    char *dev_name;
    int file_fd;
} pwr_redfish_fd_t;
#define PWR_REDFISH_FD(X) ((pwr_redfish_fd_t *)(X))


static plugin_devops_t devOps = {
    .open   = redfish_dev_open,
    .close  = redfish_dev_close,
    .read   = redfish_dev_read,
    .write  = redfish_dev_write,
    .readv  = redfish_dev_readv,
    .writev = redfish_dev_writev,
    .time   = redfish_dev_time,
    .clear  = redfish_dev_clear,
        .log_start = redfish_dev_log_start,
        .log_stop = redfish_dev_log_stop,
        .get_samples = redfish_dev_get_samples,

};


typedef struct {
    int fd, cfd;
    unsigned short local;
    char* agent_addr;
    unsigned short agent_port;

} redfish_context;
#define REDFISH_CNTX(X) ((redfish_context *)(X))



static plugin_devops_t* redfish_dev_init( const char *initstr )
{
    char *entity, *host, *node, *port;
    static int socket;
          
    plugin_devops_t *dev = malloc( sizeof(devOps) );
    *dev = devOps;
     
    DBGP("initstr='%s'\n",initstr);

    if( parse(initstr, &entity, &host, &port, &node) != 0) {
        return (plugin_devops_t *)NULL;
     }    
     pwr_redfish_dev_t *p = malloc( sizeof(pwr_redfish_dev_t ) );
     bzero( p, sizeof(pwr_redfish_dev_t) );
     p->entity = entity;
     p->host = host;
     p->port = port;
     p->node = node;
     dev->private_data = p; 
    
    return dev;

}


int parse(char *string, char **entity, char **host, char **port, char **node) {

    char *token;
    unsigned long int token_len; 
    if ( (token = strtok(string,":")) == NULL) {
        return ERR_NO;
    }
    token_len = strlen(token);
    *entity = malloc(token_len+1);
    bzero(*entity, token_len+1);
    strncpy(*entity, token, token_len);
    if ( (token = strtok(NULL,":")) == NULL) {
        return ERR_NO;
    }
    
    token_len = strlen(token);
    *host = malloc(token_len+1);
    bzero(*host,token_len+1);
    strncpy(*host, token, token_len); 
    if ( (token = strtok(NULL,":")) == NULL) {
        return ERR_NO;
    }    
    token_len = strlen(token);
    *port = malloc(token_len+1);
    bzero(*port, token_len+1);
    strncpy(*port, token, token_len);
    if ( (token = strtok(NULL,":")) == NULL) {
        return ERR_NO;
    }    
    token_len = strlen(token);
    *node = malloc(token_len+1);
    bzero(*node, token_len+1);
    strncpy(*node, token, token_len);
    return SUCCESS;
}


int redfish_connect(pwr_redfish_dev_t *p) 
{
    int sockfd = 0;    
    int k;
    struct sockaddr_in serv_addr;
    struct addrinfo *res;
    int enable = 1;
    res = malloc(sizeof(struct addrinfo));
    bzero(res, sizeof(struct addrinfo));

    if( (k = getaddrinfo(p->host, p->port, NULL, &res)) != 0) {
        perror("error:");
        return ERR_NO;
    }

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        assert("socket creation failed\n");
        return ERR_NO;
    }

    if (connect(sockfd, res->ai_addr,res->ai_addrlen) < 0) {
        perror("connect :");
        assert("Connection failed\n");
        return ERR_NO;
       }
    setsockopt( sockfd, SOL_TCP, TCP_NODELAY, (char *) &enable, sizeof(int) );
    p->socket_fd = sockfd;
    return sockfd;
}



static pwr_fd_t redfish_dev_open( plugin_devops_t* ops, const char *openstr )
{
    
    char string[200];

    pwr_redfish_fd_t *fd = malloc(sizeof(pwr_redfish_fd_t));    
    bzero( fd, sizeof(pwr_redfish_fd_t) );    
        
    pwr_redfish_dev_t *p = (pwr_redfish_dev_t *) ops->private_data;
    PWR_REDFISH_FD(fd)->dev = p;
    PWR_REDFISH_FD(fd)->dev_name = (char *) malloc(strlen(openstr)+1);
    strcpy(PWR_REDFISH_FD(fd)->dev_name, openstr);
    
    DBGP("Device Name=%s\n", PWR_REDFISH_FD(fd)->dev_name);

    if((fd->file_fd = redfish_connect(p)) < 0) {
        return (plugin_devops_t *)NULL;
    }
    return fd;
}

static int redfish_dev_read( pwr_fd_t fd, PWR_AttrName type, void* ptr, unsigned int length, PWR_Time* ts )
{
    int ret = 0, len =0, size = 0;
    char string[200];
    char buf[200], *p ,*a;
    double d, now;
    struct timeval tv;
    
    char *entity = PWR_REDFISH_FD(fd)->dev->entity;
    char *node = PWR_REDFISH_FD(fd)->dev->node;
    char *dev_name = PWR_REDFISH_FD(fd)->dev_name;
    int file_fd = PWR_REDFISH_FD(fd)->file_fd;
    
    now = getTimeSec();
    p = buf;
    bzero(p,200);


    a = attrNameToString(type);
    sprintf(string,"get:%s:%s:%s:%s;", entity, node, dev_name, a);
    if ((send(file_fd, string, strlen(string), NULL)) < 0) {
        perror("send:");
        return PWR_RET_FAILURE;
    }

    if ((recv(file_fd, p, 20, NULL)) < 0) {
        perror("recv :");
        return PWR_RET_FAILURE;
    }
    d = strtod(p,NULL);
    if ( d < 0 ) {
        return PWR_RET_FAILURE;
    }
    bcopy(&d, (double *)ptr, sizeof(double));

    *ts = getTime();

    return PWR_RET_SUCCESS;
}


static int redfish_dev_write( pwr_fd_t fd, PWR_AttrName type, void* ptr, unsigned int len )
{
    char *a;
    double now;
    char string[200];
    char buf[200];
    char *entity = PWR_REDFISH_FD(fd)->dev->entity;
    char *node = PWR_REDFISH_FD(fd)->dev->node;
    char *dev_name = PWR_REDFISH_FD(fd)->dev_name;
    int file_fd = PWR_REDFISH_FD(fd)->file_fd;
    char *command = (char *) ptr;
    now = getTimeSec();
    a = attrNameToString(type);
    sprintf(string,"set:%s:%s:%s:%s:%s;", entity, node, dev_name, a,command);
    if ((send(file_fd, string, strlen(string), NULL)) < 0) {
        perror("send:");
        return PWR_RET_FAILURE;
    }

    if ((recv(file_fd, buf, 10, NULL)) < 0) {
        perror("recv :");
        return PWR_RET_FAILURE;
    }

    return PWR_RET_SUCCESS;
}

static int redfish_dev_readv( pwr_fd_t fd, unsigned int arraysize, const PWR_AttrName attrs[],
        void* values, PWR_Time ts[], int status[] )
{

    unsigned int i;

    for( i = 0; i < arraysize; i++ )
        status[i] = redfish_dev_read( fd, attrs[i], (double *)values+i, sizeof(double), ts+i );

    return PWR_RET_SUCCESS;
}


static int redfish_dev_writev( pwr_fd_t fd, unsigned int arraysize, const PWR_AttrName attrs[],
        void* values, int status[] )
{
    unsigned int i;
    for( i = 0; i < arraysize; i++ )
        status[i] = redfish_dev_write( fd, attrs[i], (double *)values+i, sizeof(double) );
    return PWR_RET_SUCCESS;

}

static int redfish_dev_close( pwr_fd_t fd )
{
    DBGP("\n");
    return 0;
}
static int redfish_dev_final( plugin_devops_t *ops )
{
    DBGP("\n");
    free(ops->private_data);
    return 0;
}

static int redfish_dev_time(pwr_fd_t fd, PWR_Time *timestamp )
{
    double value;
    DBGP( "Info: reading time from Redfish device\n" );
    return redfish_dev_read( fd, PWR_ATTR_POWER, &value, sizeof(double), timestamp );;
}

static int redfish_dev_clear(pwr_fd_t fd) {
    DBGP("\n");
    return PWR_RET_FAILURE;
}

static int redfish_dev_log_start(pwr_fd_t fd, PWR_AttrName name)
{
    DBGP("\n");
    return PWR_RET_FAILURE;

}

static int redfish_dev_log_stop(pwr_fd_t fd, PWR_AttrName name)
{
    DBGP("\n");
    return PWR_RET_FAILURE;

}

static int redfish_dev_get_samples( pwr_fd_t fd, PWR_AttrName name,
            PWR_Time* ts, double period, unsigned int* nSamples, void* buf )

{
    DBGP("period=%f samples=%d\n",period,*nSamples);
    return PWR_RET_FAILURE;
}

static plugin_dev_t dev = {
    .init   = redfish_dev_init, 
    .final  = redfish_dev_final,
};

plugin_dev_t* getDev() {
    return &dev;
}

static double getTime() {
    struct timeval tv;
    gettimeofday(&tv,NULL);
    double value; 
    value = tv.tv_sec * 1000000000;
    value += tv.tv_usec * 1000;
    return value;
}

static double getTimeSec()
{
    return getTime() / 1000000000.0;
}

