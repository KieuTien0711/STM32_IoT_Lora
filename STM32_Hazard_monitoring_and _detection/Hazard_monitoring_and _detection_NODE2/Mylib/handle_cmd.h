#ifndef HANDLE_CMD_H
#define HANDLE_CMD_H
#include "dht22.h"
#include "hostweb.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>
void cmd_process_data(char *buff_cmd, HostwebTypedef *hostweb_node2);

#endif