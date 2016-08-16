#ifndef __ESP8266_WIFI_LINK_CONFIG_H
#define	__ESP8266_WIFI_LINK_CONFIG_H


#include "stm32f10x.h"
#include <stdbool.h>


extern bool IS_WIFI_LOG_DEBUG;
extern char WIFI_SSID_NAME[];
extern char WIFI_PASSWORD[];
extern char TCP_SERVER_IPADDRESS[];
extern char TCP_SERVER_PORT[];
extern char TCP_CILENT_IPADDRESS[];
extern char TCP_CILENT_PORT[];

#endif    /* __WIFI_CONFIG_H */
