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

#include "pwr_dev.h"
#include "util.h"
#include "pwr_redfish_dev.h"

#include <stdio.h>
#include <sys/time.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <netdb.h>
#include <netinet/tcp.h>
#include <sys/socket.h>

#define ERR -1
#define SUCCESS 0

#define MAX_SIZE 4096

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
     
    DBGP("initialization string:%s\n",initstr);

    if( parse((char *)initstr, &entity, &host, &port, &node) != 0) {
        fprintf( stderr, "Error: Invalid hardware initialization string\n" );
        return (plugin_devops_t *)0x0;
     }    
     DBGP("Info: extracted initialization string. entity=%s, host=%s,port=%s,node=%s\n",entity,host,port,node);
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
    if ( (token = strtok(string,":")) == 0x0) {
        return ERR;
    }
    token_len = strlen(token);
    *entity = malloc(token_len+1);
    bzero(*entity, token_len+1);
    strncpy(*entity, token, token_len);
    if ( (token = strtok(0x0,":")) == 0x0) {
        return ERR;
    }
    
    token_len = strlen(token);
    *host = malloc(token_len+1);
    bzero(*host,token_len+1);
    strncpy(*host, token, token_len); 
    if ( (token = strtok(0x0,":")) == 0x0) {
        return ERR;
    }    
    token_len = strlen(token);
    *port = malloc(token_len+1);
    bzero(*port, token_len+1);
    strncpy(*port, token, token_len);
    if ( (token = strtok(0x0,":")) == 0x0) {
        return ERR;
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

    int keep_alive = 1;
    
    res = malloc(sizeof(struct addrinfo));
    bzero(res, sizeof(struct addrinfo));

    if( (k = getaddrinfo(p->host, p->port, 0x0, &res)) != 0) {
        return ERR;
    }

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return ERR;
    }

    if (connect(sockfd, res->ai_addr,res->ai_addrlen) < 0) {
        return ERR;
    }

    if ( setsockopt( sockfd, SOL_TCP, TCP_NODELAY, (char *) &enable, sizeof(int)) < 0 ) {
        close(sockfd);
        return ERR;
    }
    if ( setsockopt( sockfd, SOL_SOCKET, SO_KEEPALIVE, &keep_alive, sizeof(int) ) < 0 ) {
        close(sockfd);
        return ERR;
    }
    p->socket_fd = sockfd;
    free(res);
    return sockfd;
}


static pwr_fd_t redfish_dev_open( plugin_devops_t* ops, const char *openstr )
{
    
    pwr_redfish_fd_t *fd = malloc(sizeof(pwr_redfish_fd_t));    
    bzero( fd, sizeof(pwr_redfish_fd_t) );    
        
    pwr_redfish_dev_t *p = (pwr_redfish_dev_t *) ops->private_data;
    PWR_REDFISH_FD(fd)->dev = p;
    PWR_REDFISH_FD(fd)->dev_name = (char *) malloc(strlen(openstr)+1);
    strcpy(PWR_REDFISH_FD(fd)->dev_name, openstr);
    
    DBGP("Info: Connecting to redfish agent\n");

    if((fd->file_fd = redfish_connect(p)) < 0) {
        fprintf( stderr, "Error: Connecting to redfish agent failed.\n" );
        return (plugin_devops_t *)0x0;
    }
    return fd;
}

static int redfish_dev_read( pwr_fd_t fd, PWR_AttrName type, void* ptr, unsigned int length, PWR_Time* ts )
{
    int ret = 0, len =0, size = 0;
    char string[MAX_SIZE];
    char buf[MAX_SIZE], *a;
    double d, now;
    struct timeval tv;
    
    char *entity = PWR_REDFISH_FD(fd)->dev->entity;
    char *node = PWR_REDFISH_FD(fd)->dev->node;
    char *dev_name = PWR_REDFISH_FD(fd)->dev_name;
    int file_fd = PWR_REDFISH_FD(fd)->file_fd;
    
    a = (char *) attrNameToString(type);
    
    DBGP("Info: Reading the attribute %s from entity=%s, node=%s, device=%s\n", a, entity, node, dev_name);
    sprintf(string,"get:%s:%s:%s:%s;", entity, node, dev_name, a);
    if ((send(file_fd, string, strlen(string), 0)) < 0) {
        perror("redfish agent send:");
        return PWR_RET_FAILURE;
    }

    if ((recv(file_fd, buf, 20, 0)) < 0) {
        perror("redfish agent recv :");
        return PWR_RET_FAILURE;
    }
    d = strtod(buf,0x0);
    if ( d < 0 ) {
        return PWR_RET_FAILURE;
    }
    bcopy(&d, (double *)ptr, sizeof(double));
    
    gettimeofday(&tv,0x0);
    *ts = tv.tv_sec*1000000000ULL + tv.tv_usec*1000;

    return PWR_RET_SUCCESS;
}


static int redfish_dev_write( pwr_fd_t fd, PWR_AttrName type, void* ptr, unsigned int len )
{
    char *a;
    double now;
    char string[MAX_SIZE];
    char buf[MAX_SIZE];
    char *entity = PWR_REDFISH_FD(fd)->dev->entity;
    char *node = PWR_REDFISH_FD(fd)->dev->node;
    char *dev_name = PWR_REDFISH_FD(fd)->dev_name;
    int file_fd = PWR_REDFISH_FD(fd)->file_fd;
    char *command = (char *) ptr;
    a = (char *) attrNameToString(type);
    DBGP("Info: Writing %s to attribute %s on entity=%s, node=%s, device=%s\n", command, a, entity, node, dev_name);
    sprintf(string,"set:%s:%s:%s:%s:%s;", entity, node, dev_name, a,command);
    if ((send(file_fd, string, strlen(string), 0)) < 0) {
        perror("redfish agent send:");
        return PWR_RET_FAILURE;
    }

    if ((recv(file_fd, buf, 10, 0)) < 0) {
        perror("redfish agent recv :");
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
    int file_fd = PWR_REDFISH_FD(fd)->file_fd;
    DBGP("Info: Closing socket connection\n");
    close(file_fd);
    free(fd);
    return 0;
}
static int redfish_dev_final( plugin_devops_t *ops )
{
    DBGP("\n");
    free(ops->private_data);
    free(ops);
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
    gettimeofday(&tv,0x0);
    double value; 
    value = tv.tv_sec * 1000000000;
    value += tv.tv_usec * 1000;
    return value;
}

static double getTimeSec()
{
    return getTime() / 1000000000.0;
}

