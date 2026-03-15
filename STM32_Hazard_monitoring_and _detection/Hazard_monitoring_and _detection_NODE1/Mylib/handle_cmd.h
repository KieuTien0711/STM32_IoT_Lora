#ifndef HANDLE_CMD_H
#define HANDLE_CMD_H
#include "hostweb.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>
void handle_Cmd(char *buff_cmd, HostwebTypedef *hostweb_node1);
void cmd_process_data(char *buff_cmd, HostwebTypedef *hostweb_node1);
#endif