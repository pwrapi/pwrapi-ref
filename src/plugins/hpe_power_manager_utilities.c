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

int error_return_value(int error_value, char* name){
	switch (error_value){
            	case -1:
                        DBGP("Polling service is not running. Error code: -1\n");
			return 0;
                case -2:
                        DBGP("Node parameter invalid. Error code: -2\n");
                        return 0;

                case -3:
                        DBGP("Invalid input parameter. Error code: -3\n");
                        return 0;

                case -4:
                        DBGP("Node:'%s' not in PCM database. Error code: -4\n");
                        return 0;

                case -5:
                        DBGP("Node: '%s' failed to perform the operation. Error code: -5\n");
                        return 0;

                case -6:
                        DBGP("Nodeset: '%s' not in PCM database. Error code: -6\n");
                        return 0;

                case -7:
                        DBGP("Nodeset parameter invalid. Error code: -7\n");
                        return 0;
		case -8:
                         DBGP("Sensor not found. Error code: -8\n");
                         return 0;

                case -9:
                        DBGP("Node Suspended. Error code: -9\n");
                        return 0;
                case -10:
                        DBGP("Node offline. Error code: -10\n");
                        return 0;

                case -11:
                        DBGP("Chassis Suspended. Error code: -11\n");
                        return 0;

                case -12:
                        DBGP("Chassis offline. Error code: -12\n");
                        return 0;

                case -13:
                        DBGP("Rack Suspended. Error code: -13\n");
                        return 0;

                case -14:
                        DBGP("Rack offline. Error code: -14\n");
                        return 0;

                }
	DBGP("Unknown error\n");
	return 0;
}

/* Function to fetch energy/power values from pdu/rack. If system has pdu, power and energy metrics will be fetched from pdu. Otherwise, result will be the addition of values from all the individual nodes.  */
int rack_or_pdu_report(void *ptr,pwr_sgi_fd_t *fd, PWR_AttrName type){
	xmlrpc_env env = fd->env;
        xmlrpc_value * resultP;
      	if(type == PWR_ATTR_ENERGY){
		if(strcmp(fd->type,"rack")== 0){
			strcpy(fd->methodName, RACK_ENERGY_REPORT);
		}
		else if (strcmp(fd->type,"pdu") == 0){
			strcpy(fd->methodName, PDU_ENERGY_REPORT);
		}
	}
	else if(type == PWR_ATTR_POWER){
		if(strcmp(fd->type,"rack")== 0){
                        strcpy(fd->methodName, RACK_POWER_REPORT);
                }
                else if (strcmp(fd->type,"pdu") == 0){
                        strcpy(fd->methodName, PDU_POWER_REPORT);
                }

	}
    /* Initialize our error-handling environment. */
        xmlrpc_env_init(&env);

    /* Make the remote procedure call */
        DBGP("Rack Name: %s, Method Name: %s\n",fd->rack_name,fd->methodName);
	DBGP("URL: %s, Method Name: %s, name: %s\n",fd->dev->url,fd->methodName,fd->rack_name);
        resultP = xmlrpc_client_call(&env, fd->dev->url,fd->methodName,"(s)",fd->rack_name);
        if (env.fault_occurred){
               	DBGP("xmlrpc fault occured. Fault code %d, Fault string: %s",env.fault_code, env.fault_string);
		return 0;		// xml_rpc error handling
        }
    /*xml_rpc query returns an array. If the query is executed successfully, size of array returned is 2 (value, timestamp). If unsuccessful, size of array returned is 1 (negative integer).*/	
        int size = xmlrpc_array_size(&env,resultP);
        if(size<=1 || resultP == NULL){
                if(size==1){
			xmlrpc_value * value;
                        int error_value = 0;
			int return_value = 0;
                        xmlrpc_array_read_item(&env,resultP,0,&value);
                        xmlrpc_read_int(&env, value, &error_value);  	// error value will be stored in error_value variable.
//        		DBGP("TYPE: %s\n", xmlrpc_type_name(xmlrpc_value_type(value)) );
                        return_value = error_return_value(error_value,fd->rack_name);
			return return_value;
                }
                return 0;
        }
        xmlrpc_value * value;
        double double_value;
        xmlrpc_array_read_item(&env,resultP,0,&value);
        xmlrpc_read_double(&env, value, &double_value);
        *(double*)ptr = double_value;
	if(resultP)
                xmlrpc_DECREF(resultP);
        return 1;
}

int chassis_report(void *ptr,pwr_sgi_fd_t *fd, PWR_AttrName type){
	xmlrpc_env env = fd->env;
        xmlrpc_value * resultP;
      	if(type == PWR_ATTR_ENERGY){
		strcpy(fd->methodName, CHASSIS_ENERGY_REPORT);
	}
	else if(type == PWR_ATTR_POWER){
                strcpy(fd->methodName, CHASSIS_POWER_REPORT);

	}
    /* Initialize our error-handling environment. */
        xmlrpc_env_init(&env);

    /* Make the remote procedure call */
        DBGP("Chassis Name: %s, Method Name: %s\n",fd->chassis_name,fd->methodName);
	DBGP("URL: %s, Method Name: %s, name: %s\n",fd->dev->url,fd->methodName,fd->chassis_name);
        resultP = xmlrpc_client_call(&env, fd->dev->url,fd->methodName,"(s)",fd->chassis_name);
        if (env.fault_occurred){
               	DBGP("xmlrpc fault occured. Fault code %d, Fault string: %s",env.fault_code, env.fault_string);
		return 0;		// xml_rpc error handling
        }
    /*xml_rpc query returns an array. If the query is executed successfully, size of array returned is 2 (value, timestamp). If unsuccessful, size of array returned is 1 (negative integer).*/	
        int size = xmlrpc_array_size(&env,resultP);
        if(size<=1 || resultP == NULL){
                if(size==1){
			xmlrpc_value * value;
                        int error_value = 0;
			int return_value = 0;
                        xmlrpc_array_read_item(&env,resultP,0,&value);
                        xmlrpc_read_int(&env, value, &error_value);  	// error value will be stored in error_value variable.
//        		DBGP("TYPE: %s\n", xmlrpc_type_name(xmlrpc_value_type(value)) );
                        return_value = error_return_value(error_value,fd->rack_name);
			return return_value;
                }
                return 0;
        }
        xmlrpc_value * value;
        double double_value;
        xmlrpc_array_read_item(&env,resultP,0,&value);
        xmlrpc_read_double(&env, value, &double_value);
        *(double*)ptr = double_value;
	if(resultP)
                xmlrpc_DECREF(resultP);
        return 1;
}

/* Function to set the power limit of nodes or nodeset. Successful query does not return anything. */
int power_limit(pwr_sgi_fd_t *fd, int limit){
        xmlrpc_env env = fd->env;
        xmlrpc_value * resultP;
	char type_name[50];
	if(strcmp(fd->type,"node") == 0){
        	strcpy(fd->methodName, NODE_POWER_LIMIT);
		strcpy(type_name,fd->node);
	}
	else if(strcmp(fd->type,"nodeset") == 0){
        	strcpy(fd->methodName, NODESET_POWER_LIMIT);
		strcpy(type_name,fd->nodeset);
	}
        xmlrpc_env_init(&env);
	DBGP("URL: %s, Method Name: %s, name: %s\n",fd->dev->url,fd->methodName,type_name);
        resultP = xmlrpc_client_call(&env, fd->dev->url,fd->methodName,"(si)",type_name,limit);
	if (env.fault_occurred){
               	DBGP("xmlrpc fault occured. Fault code %d, Fault string: %s",env.fault_code, env.fault_string);
                return 0;	//xmlrpc error handling
        }
        int size = xmlrpc_array_size(&env,resultP);     
	if(size==1){
           	xmlrpc_value * value;
                int error_value = 0;
                int return_value = 0;
                xmlrpc_array_read_item(&env,resultP,0,&value);
                xmlrpc_read_int(&env, value, &error_value);     // error value will be stored in error_value variable.
//              DBGP("TYPE: %s\n", xmlrpc_type_name(xmlrpc_value_type(value)) );
		return_value = error_return_value(error_value,fd->rack_name);
                return return_value;
         }
	
 	if(resultP)
		xmlrpc_DECREF(resultP);
	return 1;

}

/* Function to fetch thermal sensor values from nodes/nodesets. */
int thermal_report(void *ptr,pwr_sgi_fd_t *fd){
        xmlrpc_env env = fd->env;
        xmlrpc_value * resultP;
        char type_name[50];
       	int return_value = 0;
	if(strcmp(fd->type,"node") == 0){
		strcpy(fd->methodName, NODE_THERMAL_REPORT);
		strcpy(type_name,fd->node);
	}
	else if(strcmp(fd->type,"nodeset") == 0){
                strcpy(fd->methodName, NODESET_THERMAL_REPORT);
                strcpy(type_name,fd->nodeset);
        }

        xmlrpc_env_init(&env);
	DBGP("URL: %s, Method Name: %s, name: %s\n",fd->dev->url,fd->methodName,type_name);
        resultP = xmlrpc_client_call(&env, fd->dev->url,fd->methodName,"(s)",type_name);
        if (env.fault_occurred){
               	DBGP("xmlrpc fault occured. Fault code %d, Fault string: %s",env.fault_code, env.fault_string);
                return 0;	//xmlrpc error handling
        }
/*For node, if query execution is successful, returned value is string. And if unsuccessful, returned value is and array of size one containing negative integer. */
        if(strcmp(fd->type,"node") == 0){
		if(strcmp(xmlrpc_type_name(xmlrpc_value_type(resultP)),"ARRAY") == 0 ){
			xmlrpc_value * value;
                	int error_value = 1;
			xmlrpc_array_read_item(&env,resultP,0,&value);
			xmlrpc_read_int(&env, value, &error_value);
                        return_value = error_return_value(error_value,type_name);

		}
		else if(strcmp(xmlrpc_type_name(xmlrpc_value_type(resultP)),"STRING") == 0 ){
			const char * str_val;
        		xmlrpc_read_string(&env, resultP, &str_val);
	        	double double_val = strtod(str_val,NULL);       //converting string to double
        		free((void *)str_val);
        		*(double*)ptr = double_val;               
			return_value = 1;
		}
		else{
			DBGP("Empty or unexpected response. Node name: %s, Method Name: %s \n",type_name,fd->methodName);
			return_value = 0;
		}
	}  
/*For nodeset, if query execution is successful, returned value is an array of size one containing value as double. And if unsuccessful, returned value is and array of size one containing negative integer. */
 
	else{
		double double_value;
	        int size = xmlrpc_array_size(&env,resultP);
//	        DBGP("TYPE: %s\n", xmlrpc_type_name(xmlrpc_value_type(resultP)) );
	        if(size <1){
	            	DBGP("Unknown Error.\n");
	                return 0;
	        }
	        xmlrpc_value * value;
        	int error_value = 1;
        	xmlrpc_array_read_item(&env,resultP,0,&value);    
		if(strcmp(xmlrpc_type_name(xmlrpc_value_type(value)),"INT") == 0 ){   // if returned value is int, some error has occured
		        xmlrpc_read_int(&env, value, &error_value);
		        return_value = error_return_value(error_value,type_name);
		}
		else if (strcmp(xmlrpc_type_name(xmlrpc_value_type(value)),"DOUBLE") == 0 ){
	        	xmlrpc_read_double(&env, value, &double_value);
        		*(double*)ptr = double_value;
			return_value = 1;
		}
	}
       	if(resultP)
                xmlrpc_DECREF(resultP);
        return return_value;
}

/*Function to fetch thermal the value of the individual sensor like CPU, DIMM, etc. If query execution is successful, array of size 7 is returned. Temperature value is at index 3 and data type is string. If query execution is unsuccessful, size of array returned is 1 and it contains a negative intger.*/
int node_sensor_report(void *ptr, int attribute, pwr_sgi_fd_t *fd){
    	xmlrpc_env env = fd->env;
    	xmlrpc_value * resultP;
    	strcpy(fd->methodName, NODE_SENSOR_REPORT);
    	xmlrpc_env_init(&env);
	DBGP("URL: %s, Method Name: %s, sensor_name: %s\n",fd->dev->url,fd->methodName,fd->sensor_name);
    	resultP = xmlrpc_client_call(&env, fd->dev->url,fd->methodName,"(ss)",fd->node,fd->sensor_name);
	if (env.fault_occurred){
               	DBGP("xmlrpc fault occured. Fault code %d, Fault string: %s",env.fault_code, env.fault_string);
		return 0;    	//xmlrpc error handling
	}
	xmlrpc_value * value;
        double double_value;
        int size = xmlrpc_array_size(&env,resultP);
	if(size <= 1 || resultP == NULL){
		if(size <1){
			DBGP("Unknown Error.\n");
			return 0;
		}
		int return_value = 0;
		xmlrpc_value * e_value;
        	int error_value;
		xmlrpc_array_read_item(&env,resultP,0,&e_value);
                xmlrpc_read_int(&env, e_value, &error_value);
		return_value = error_return_value(error_value,fd->node);	
	        if(resultP)
        	        xmlrpc_DECREF(resultP);

		return return_value;
	}
        const char * name;
        xmlrpc_array_read_item(&env,resultP,3,&value); //Temperature value is at index 3.
        xmlrpc_read_string(&env, value, &name); 
        double temp = strtod(name,NULL); 
        free((void *)name);
	*(double*)ptr = temp;

 	if(resultP)
		xmlrpc_DECREF(resultP);
    	return 1;
}

/* Function to fetch energy/power values of the nodeset. If query execution is successful, an array of size 2 is returned. Index 0 has power and index 1 has energy value. Integer variable attribute tells which index to read. Unsuccessful query execution will return an array of size 1, containing negative integer. */
int nodeset_power_report(void *ptr, int attribute, pwr_sgi_fd_t *fd){
    	xmlrpc_env env = fd->env;
    	xmlrpc_value * resultP;
    	strcpy(fd->methodName, NODESET_POWER_REPORT);
    	xmlrpc_env_init(&env);

	DBGP("URL: %s, Method Name: %s, Nodeset Name: %s\n",fd->dev->url,fd->methodName,fd->nodeset);
    	resultP = xmlrpc_client_call(&env, fd->dev->url,fd->methodName,"(s)",fd->nodeset);
	if (env.fault_occurred){
               	DBGP("xmlrpc fault occured. Fault code %d, Fault string: %s",env.fault_code, env.fault_string);
		return 1;
	}
        int size = xmlrpc_array_size(&env,resultP);
	
	if(size <= 1 || resultP == NULL){
		if(size <1){
			DBGP("Unknown Error.\n");
			return 0;
		}
		int return_value = 0;
		xmlrpc_value * e_value;
        	int error_value;
		xmlrpc_array_read_item(&env,resultP,0,&e_value);
                xmlrpc_read_int(&env, e_value, &error_value);
		return_value = error_return_value(error_value,fd->node);	
	        if(resultP)
        	        xmlrpc_DECREF(resultP);
		return return_value;
	}
        xmlrpc_value * value;
        double double_value;

	xmlrpc_array_read_item(&env,resultP,attribute,&value);
        xmlrpc_read_double(&env, value, &double_value);
	*(double*)ptr = double_value;

    	if(resultP)
		xmlrpc_DECREF(resultP);

    	return 1;

} 

/* Function to fetch energy/power values of the node. If query execution is successful, an array of size 6 is returned. Index 2 has power and index 0 has energy value. Integer variable attribute tells which index to read. Unsuccessful query execution will return an array of size 1, containing negative integer. */

int node_power_report(void *ptr, int attribute, pwr_sgi_fd_t *fd){
    	xmlrpc_env env = fd->env;
    	xmlrpc_value * resultP;
    	strcpy(fd->methodName, NODE_POWER_REPORT);
    	xmlrpc_env_init(&env);
	DBGP("URL: %s, Method Name: %s, Node Name: %s\n",fd->dev->url,fd->methodName,fd->node);
    	resultP = xmlrpc_client_call(&env, fd->dev->url,fd->methodName,"(s)",fd->node);

	if (env.fault_occurred){
               	DBGP("xmlrpc fault occured. Fault code %d, Fault string: %s",env.fault_code, env.fault_string);
		return 0;
	}
        int size = xmlrpc_array_size(&env,resultP);
	
	if(size <= 1 || resultP == NULL){
		if(size <1){
			DBGP("Unknown Error.\n");
			return 0;
		}
		int return_value = 0;
		xmlrpc_value * e_value;
        	int error_value;
		xmlrpc_array_read_item(&env,resultP,0,&e_value);
                xmlrpc_read_int(&env, e_value, &error_value);
		return_value = error_return_value(error_value,fd->node);	
	        if(resultP)
        	        xmlrpc_DECREF(resultP);

		return return_value;
	}
        xmlrpc_value * value;
        double double_value;
        int integer_value;
	xmlrpc_array_read_item(&env,resultP,attribute,&value);
	if(attribute == 2){	//power attribute has data type int and energy attribute has data type double.
		xmlrpc_read_int(&env, value, &integer_value);
		*(double*)ptr = (double)integer_value;
	}
	else{
		xmlrpc_read_double(&env, value, &double_value);
		*(double*)ptr = double_value;
	}

    	if(resultP)
		xmlrpc_DECREF(resultP);

    	return 1;

} 

int node_power_get_limit(void *ptr,  pwr_sgi_fd_t *fd){
    	xmlrpc_env env = fd->env;
    	xmlrpc_value * resultP;
    	strcpy(fd->methodName, NODE_POWER_GET_LIMIT);
    	xmlrpc_env_init(&env);
	DBGP("URL: %s, Method Name: %s, Node: %s\n",fd->dev->url,fd->methodName,fd->node);
    	resultP = xmlrpc_client_call(&env, fd->dev->url,fd->methodName,"(s)",fd->node);
	if (env.fault_occurred){
               	DBGP("xmlrpc fault occured. Fault code %d, Fault string: %s",env.fault_code, env.fault_string);
		return 0;    	//xmlrpc error handling
	}
	xmlrpc_value * value;
        int int_value, return_value = 0;
        int size = xmlrpc_array_size(&env,resultP);
	if(size <= 0 || resultP == NULL){
		DBGP("Unknown Error.\n");
		return_value = 0;
	        if(resultP)
        	        xmlrpc_DECREF(resultP);

		return return_value;
	}
        xmlrpc_array_read_item(&env,resultP,0,&value); 
        xmlrpc_read_int(&env, value, &int_value); 
	if(int_value < 0){
		return_value = error_return_value(int_value,fd->node);
	}
	else{
		*(double*)ptr = (double)int_value;
		return_value = 1;
	}
 	if(resultP)
		xmlrpc_DECREF(resultP);
    	return return_value;
}

