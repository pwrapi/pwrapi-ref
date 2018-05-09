/* 
 * Copyright 2014-2016 Sandia Corporation. Under the terms of Contract
 * DE-AC04-94AL85000, there is a non-exclusive license for use of this work 
 * by or on behalf of the U.S. Government. Export of this program may require
 * a license from the United States Government.
 *
 * This file is part of the Power API Prototype software package. For license
 * information, see the LICENSE file in the top level directory of the
 * distribution.
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

void parseOpenStr(char * str, pwr_sgi_fd_t **data){
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


void parse(char *str, pwr_sgi_dev_t **data ){
  
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


static pwr_fd_t sgi_dev_open( plugin_devops_t* ops, const char *openstr )
{
	pwr_sgi_fd_t *fd = (pwr_sgi_fd_t *)malloc(sizeof(pwr_sgi_fd_t));   
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

static int sgi_dev_close( pwr_fd_t fd1 )

{
	pwr_sgi_fd_t *fd = (pwr_sgi_fd_t*)fd1;
	xmlrpc_env_clean(&(fd->env));



    DBGP("\n");
    free( fd1 );

    return 0;
}

static int sgi_dev_read( pwr_fd_t fd1, PWR_AttrName type, void* ptr, unsigned int len, PWR_Time* ts )
{

	int return_value = 0;
	pwr_sgi_fd_t *fd = (pwr_sgi_fd_t*)fd1;
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

static int sgi_dev_write( pwr_fd_t fd1, PWR_AttrName type, void* ptr, unsigned int len )
{
        char *command = (char *) ptr;
        int limit = atoi(command);
//	printf("\nwrite\n");
	pwr_sgi_fd_t *fd = (pwr_sgi_fd_t*)fd1;
//	printf("Limit: %d\n",limit);
	if(type == PWR_ATTR_POWER_LIMIT_MAX){
		if(power_limit(fd,limit) == 1)
 			return PWR_RET_SUCCESS;
	}
	return PWR_RET_FAILURE;
}

static int sgi_dev_readv( pwr_fd_t fd, unsigned int arraysize, const PWR_AttrName attrs[], void* buf,
                        PWR_Time ts[], int status[] )
{
    return PWR_RET_SUCCESS;
}

static int sgi_dev_writev( pwr_fd_t fd, unsigned int arraysize, const PWR_AttrName attrs[], void* buf, int status[] )
{
    return PWR_RET_SUCCESS;
}

static int sgi_dev_time( pwr_fd_t fd, PWR_Time *timestamp )
{
    return 0;
}

static int sgi_dev_clear( pwr_fd_t fd )
{
    return 0;
}

/*static int sgi_dev_log_start( pwr_fd_t fd, PWR_AttrName name )
{
	return PWR_RET_SUCCESS;
}

static int sgi_dev_log_stop( pwr_fd_t fd, PWR_AttrName name )
{
	return PWR_RET_SUCCESS;
}

static int sgi_dev_get_samples( pwr_fd_t fd, PWR_AttrName name, 
			PWR_Time* ts, double period, unsigned int* nSamples, void* buf )

{
	return PWR_RET_SUCCESS;
}*/

static plugin_devops_t devOps = {
    .open   = sgi_dev_open, 
    .close  = sgi_dev_close,
    .read   = sgi_dev_read,
    .write  = sgi_dev_write,
    .readv  = sgi_dev_readv,
    .writev = sgi_dev_writev,
    .time   = sgi_dev_time,
    .clear  = sgi_dev_clear,
//	.log_start = sgi_dev_log_start,
//	.log_stop = sgi_dev_log_stop,
//	.get_samples = sgi_dev_get_samples,
};


static plugin_devops_t* sgi_dev_init( const char *initstr )
{
	DBGP("initstr='%s'\n",initstr);
//	printf("Init str: %s\n",initstr);
	char initial[50];
	strcpy(initial,initstr); 
	plugin_devops_t* ops = malloc(sizeof(*ops));
	*ops = devOps;
	pwr_sgi_dev_t *p = (pwr_sgi_dev_t *) malloc(sizeof(pwr_sgi_dev_t));
	parse(initial,&p);
	sprintf(p->url,"http://%s:%s/RPC2",p->host,p->port);
//	printf("%s %s ",p->host, p->port);
	ops->private_data = p;

    return ops;
}

static int sgi_dev_final( plugin_devops_t *ops )
{
    xmlrpc_client_cleanup();
    DBGP("\n");
    free( ops->private_data );
    free( ops );
    return 0;
}

static plugin_dev_t dev = {
    .init   = sgi_dev_init, 
    .final  = sgi_dev_final,
};

plugin_dev_t* getDev() {
    return &dev;
}

//
// Start of plugin meta data
//

// Any thing can co here as long as it does not clash with other plugins
static int sgi_getPluginName( size_t len, char* buf )
{
    strncpy(buf,"SGI",len);
	return 0;
}



