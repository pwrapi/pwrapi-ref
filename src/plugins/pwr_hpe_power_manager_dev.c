/*
* Copyright 2018 Hewlett Packard Enterprise Development LP
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

#include <sys/timeb.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include "pwr_dev.h"
#include "util.h"
#include <xmlrpc-c/base.h>
#include <xmlrpc-c/client.h>
#include <hpe_power_manager_utilities.h>

static double getTime() {
    struct timeval tv;
    gettimeofday(&tv,NULL);
	double value; 
    value = tv.tv_sec * 1000000000;
    value += tv.tv_usec * 1000;
	return value;
}

void parseOpenStr(char * str, pwr_hpe_fd_t **data){
        char *token = strtok(str,":");
        if(token){
        	strcpy((*data)->type,token);
//       	 	printf("Type: %s\n",(*data)->type);
       	}
	token = strtok(NULL,":");
        if(token){
		if(strcmp((*data)->type,"node") == 0)
                	strcpy((*data)->node,token);
		if(strcmp((*data)->type,"sensor") == 0)
                	strcpy((*data)->node,token);
		if(strcmp((*data)->type,"pdu") == 0 || strcmp((*data)->type,"rack") == 0)
                        strcpy((*data)->rack_name,token);
		if(strcmp((*data)->type,"nodeset") == 0)
                        strcpy((*data)->nodeset,token);
		if(strcmp((*data)->type,"chassis") == 0)
                        strcpy((*data)->chassis_name,token);


//                printf("node: %s, rack_name: %s\n",(*data)->node,(*data)->rack_name);
        }
	token = strtok(NULL,":");
        if(token){
                strcpy((*data)->sensor_name,token);
//                printf("sensor_name: %s\n",(*data)->sensor_name);
        }
        return;
}


void parse(char *str, pwr_hpe_dev_t **data ){
  
	char *token = strtok(str,":");
        if(token){
        	strcpy((*data)->host,token);
//		printf("host: %s\n",(*data)->host);
	}
        token = strtok(NULL,":");
        if(token){
		strcpy((*data)->port,token);
//        	printf("port: %s\n",(*data)->port);
	}
        return;
}


static double getTimeSec()
{
	return getTime() / 1000000000.0;
}


static pwr_fd_t hpe_dev_open( plugin_devops_t* ops, const char *openstr )
{
	pwr_hpe_fd_t *fd = (pwr_hpe_fd_t *)malloc(sizeof(pwr_hpe_fd_t));   
	fd->dev = ops->private_data;
	char openstring[50];
	strcpy(openstring,openstr);	
	parseOpenStr(openstring,&fd);

	char * const clientName = "XML-RPC C Test Client";
    	char * const clientVersion = "1.0";
	xmlrpc_env_init(&(fd->env));
    	xmlrpc_client_init2(&(fd->env), XMLRPC_CLIENT_NO_FLAGS, clientName, clientVersion, NULL, 0);

	return fd;
}

static int hpe_dev_close( pwr_fd_t fd1 )

{
	pwr_hpe_fd_t *fd = (pwr_hpe_fd_t*)fd1;
	xmlrpc_env_clean(&(fd->env));



    DBGP("\n");
    free( fd1 );

    return 0;
}

static int hpe_dev_read( pwr_fd_t fd1, PWR_AttrName type, void* ptr, unsigned int len, PWR_Time* ts )
{

	int return_value = 0;
	pwr_hpe_fd_t *fd = (pwr_hpe_fd_t*)fd1;
//	printf("Reading:\nHost: %s \nPort: %s \nNode: %s \nURL: %s\n",fd->dev->host, fd->dev->port, fd->node,fd->dev->url);
	
	if(strcmp(fd->type,"node") == 0){
		if(type == PWR_ATTR_TEMP)
			return_value = thermal_report(ptr,fd);
		else if(type == PWR_ATTR_POWER)
                        return_value = node_power_report(ptr,2,fd);
                else if(type == PWR_ATTR_ENERGY)
                        return_value = node_power_report(ptr,0,fd);
		else if(type == PWR_ATTR_POWER_LIMIT_MAX)
			return_value = node_power_get_limit(ptr,fd);
	}
	else if (strcmp(fd->type,"pdu") == 0 || strcmp(fd->type,"rack") == 0){
		return_value = rack_or_pdu_report(ptr,fd,type);

	}
	else if (strcmp(fd->type,"chassis") == 0 ){
		return_value = chassis_report(ptr,fd,type);
	}
	else if (strcmp(fd->type,"nodeset") == 0){
                 if(type == PWR_ATTR_POWER)
                         return_value = nodeset_power_report(ptr,0,fd);
                 else if(type == PWR_ATTR_ENERGY)
                         return_value = nodeset_power_report(ptr,1,fd);
                 else if(type == PWR_ATTR_TEMP)
                         return_value = thermal_report(ptr,fd);
		 else if(type == PWR_ATTR_POWER_LIMIT_MAX){
                         DBGP("Get Max Power Limit not supported for nodeset");
			 return_value = 0;
		 }
         }
	else if (strcmp(fd->type,"sensor") == 0){
                 if(type == PWR_ATTR_TEMP)
                         return_value = node_sensor_report(ptr,3,fd);

         }



 	*ts =	getTime();

	if(return_value == 1)
    		return PWR_RET_SUCCESS;
	else
		return PWR_RET_FAILURE;
}

static int hpe_dev_write( pwr_fd_t fd1, PWR_AttrName type, void* ptr, unsigned int len )
{
        char *command = (char *) ptr;
        int limit = atoi(command);
//	printf("\nwrite\n");
	pwr_hpe_fd_t *fd = (pwr_hpe_fd_t*)fd1;
//	printf("Limit: %d\n",limit);
	if(type == PWR_ATTR_POWER_LIMIT_MAX){
		if(power_limit(fd,limit) == 1)
 			return PWR_RET_SUCCESS;
	}
	return PWR_RET_FAILURE;
}

static int hpe_dev_readv( pwr_fd_t fd, unsigned int arraysize, const PWR_AttrName attrs[], void* buf,
                        PWR_Time ts[], int status[] )
{
    return PWR_RET_SUCCESS;
}

static int hpe_dev_writev( pwr_fd_t fd, unsigned int arraysize, const PWR_AttrName attrs[], void* buf, int status[] )
{
    return PWR_RET_SUCCESS;
}

static int hpe_dev_time( pwr_fd_t fd, PWR_Time *timestamp )
{
    return 0;
}

static int hpe_dev_clear( pwr_fd_t fd )
{
    return 0;
}

/*static int hpe_dev_log_start( pwr_fd_t fd, PWR_AttrName name )
{
	return PWR_RET_SUCCESS;
}

static int hpe_dev_log_stop( pwr_fd_t fd, PWR_AttrName name )
{
	return PWR_RET_SUCCESS;
}

static int hpe_dev_get_samples( pwr_fd_t fd, PWR_AttrName name, 
			PWR_Time* ts, double period, unsigned int* nSamples, void* buf )

{
	return PWR_RET_SUCCESS;
}*/

static plugin_devops_t devOps = {
    .open   = hpe_dev_open, 
    .close  = hpe_dev_close,
    .read   = hpe_dev_read,
    .write  = hpe_dev_write,
    .readv  = hpe_dev_readv,
    .writev = hpe_dev_writev,
    .time   = hpe_dev_time,
    .clear  = hpe_dev_clear,
//	.log_start = hpe_dev_log_start,
//	.log_stop = hpe_dev_log_stop,
//	.get_samples = hpe_dev_get_samples,
};


static plugin_devops_t* hpe_dev_init( const char *initstr )
{
	DBGP("initstr='%s'\n",initstr);
//	printf("Init str: %s\n",initstr);
	char initial[50];
	strcpy(initial,initstr); 
	plugin_devops_t* ops = malloc(sizeof(*ops));
	*ops = devOps;
	pwr_hpe_dev_t *p = (pwr_hpe_dev_t *) malloc(sizeof(pwr_hpe_dev_t));
	parse(initial,&p);
	sprintf(p->url,"http://%s:%s/RPC2",p->host,p->port);
//	printf("%s %s ",p->host, p->port);
	ops->private_data = p;

    return ops;
}

static int hpe_dev_final( plugin_devops_t *ops )
{
    xmlrpc_client_cleanup();
    DBGP("\n");
    free( ops->private_data );
    free( ops );
    return 0;
}

static plugin_dev_t dev = {
    .init   = hpe_dev_init, 
    .final  = hpe_dev_final,
};

plugin_dev_t* getDev() {
    return &dev;
}

//
// Start of plugin meta data
//

// Any thing can co here as long as it does not clash with other plugins
static int hpe_getPluginName( size_t len, char* buf )
{
    strncpy(buf,"HPE",len);
	return 0;
}



