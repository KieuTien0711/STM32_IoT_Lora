#include "handle_cmd.h"
// ---------- Handle Cmd Function -------------------
// Ham xu ly lenh CMD va luu gia tri vao cac bien

// ---------- Handle Cmd Function -------------------
// Ham xu ly chuoi lenh CMD va luu gia tri vao bien tuong ung

char *cmd;
void handle_Cmd(char *buff_cmd, HostwebTypedef *hostweb_node1)
{
    char *cmd_start = buff_cmd;
    char *cmd_end;

    // Vong lap de tach tung lenh theo dau ';'
    while ((cmd_end = strchr(cmd_start, ';')) != NULL) {
        *cmd_end = '\0';  // Thay dau ';' bang ky tu ket thuc chuoi

        // Tim dau ':' de tach ten lenh va gia tri
        char *colon_pos = strchr(cmd_start, ':');
        if (colon_pos != NULL) {
            *colon_pos = '\0';  // Thay dau ':' bang ky tu ket thuc chuoi
            cmd = cmd_start;           // Tro den ten lenh (vi du: ppmthreshold)
            char *value_str = colon_pos + 1; // Tro den gia tri (vi du: 45.50)

            // Xu ly va gan gia tri tuong ung
            if (strcmp(cmd, "ppmthreshold") == 0) {
                hostweb_node1->ppm_threshold = atof(value_str);
            } else if (strcmp(cmd, "pgathreshold") == 0) {
                hostweb_node1->pga_threshold = atof(value_str);
            } else if (strcmp(cmd, "buzzerstatenode1") == 0) {
                hostweb_node1->buzzer_state = atoi(value_str);  // atoi dung cho so nguyen
            }
        }

        cmd_start = cmd_end + 1;  // Chuyen sang lenh tiep theo
    }
}

// Ham kiem tra va xu ly chuoi CMD tu node1
void cmd_process_data(char *buff_cmd, HostwebTypedef *hostweb_node1)
{
	if (strstr(buff_cmd, "id:node1")) {
			// Xu ly lenh cua node1
			handle_Cmd(buff_cmd, hostweb_node1);
//				HAL_Delay(200);

	}
}
