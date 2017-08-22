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


#ifndef REDFISH_DEV_H
#define REDFISH_DEV_H

#include "pwrdev.h"

#ifdef __cplusplus
extern "C" {
#endif



static plugin_devops_t* redfish_dev_init( const char *initstr );
static pwr_fd_t redfish_dev_open( plugin_devops_t* ops, const char *openstr );
static int redfish_dev_read( pwr_fd_t fd, PWR_AttrName type, void* ptr, unsigned int len, PWR_Time* ts );
static int redfish_dev_write( pwr_fd_t fd, PWR_AttrName type, void* ptr, unsigned int len );
static int redfish_dev_readv( pwr_fd_t fd, unsigned int arraysize, const PWR_AttrName attrs[], 
                                void* buf, PWR_Time ts[], int status[] );
static int redfish_dev_writev( pwr_fd_t fd, unsigned int arraysize, const PWR_AttrName attrs[], 
                                void* ptr, int status[] );
static int redfish_dev_parse( const char *initstr, unsigned int *saddr, unsigned int *sport );
static int redfish_dev_close( pwr_fd_t fd );
static int redfish_dev_final( plugin_devops_t *ops );
static int redfish_dev_time(pwr_fd_t fd, PWR_Time *timestamp );
static int redfish_dev_clear(pwr_fd_t fd);
static int redfish_dev_log_start(pwr_fd_t fd, PWR_AttrName name);
static int redfish_dev_log_stop(pwr_fd_t fd, PWR_AttrName name);
static int redfish_dev_get_samples( pwr_fd_t fd, PWR_AttrName name, PWR_Time* ts, double period, 
                                    unsigned int* nSamples, void* buf );

static double getTime();
static double getTimeSec();

#ifdef __cplusplus
}
#endif

#endif
