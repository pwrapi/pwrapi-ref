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

#define MONITOR_NODE_REPORT "monitor_node_report"
#define NODE_POWER_REPORT "node_power_report"
#define NODESET_POWER_REPORT "nodeset_power_report"
#define RACK_POWER_REPORT "rack_power_report"
#define RACK_ENERGY_REPORT "rack_energy_report"
#define PDU_POWER_REPORT "pdu_power_report"
#define PDU_ENERGY_REPORT "pdu_energy_report"
#define CHASSIS_POWER_REPORT "chassis_power_report"
#define CHASSIS_ENERGY_REPORT "chassis_energy_report"
#define NODE_POWER_LIMIT "node_power_limit"
#define NODESET_POWER_LIMIT "nodeset_power_limit"
#define NODE_SENSOR_REPORT "node_sensor_report"
#define NODESET_THERMAL_REPORT "nodeset_thermal_report"
#define NODE_THERMAL_REPORT "node_thermal_report"
#define NODE_POWER_GET_LIMIT "node_power_get_limit"
typedef struct {
    char url[100];
        char host[20];
        char port[10];
} pwr_sgi_dev_t;

typedef struct {
        pwr_sgi_dev_t *dev;
        char sensor_name[50];
        char methodName[50];
        char node[20];
        char rack_name[20];
        char chassis_name[20];
        char type[20];
        char nodeset[50];
        xmlrpc_env env;

} pwr_sgi_fd_t;

int error_return_value(int error_value, char* name);
int rack_or_pdu_report(void *ptr,pwr_sgi_fd_t *fd, PWR_AttrName type);
int chassis_report(void *ptr,pwr_sgi_fd_t *fd, PWR_AttrName type);
int power_limit(pwr_sgi_fd_t *fd, int limit);
int thermal_report(void *ptr,pwr_sgi_fd_t *fd);
int node_sensor_report(void *ptr, int attribute, pwr_sgi_fd_t *fd);
int nodeset_power_report(void *ptr, int attribute, pwr_sgi_fd_t *fd);
int node_power_report(void *ptr, int attribute, pwr_sgi_fd_t *fd);
int node_power_get_limit(void *ptr,  pwr_sgi_fd_t *fd);

