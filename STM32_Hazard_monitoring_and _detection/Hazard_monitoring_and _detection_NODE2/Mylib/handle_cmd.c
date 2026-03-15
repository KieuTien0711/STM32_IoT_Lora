#include "handle_cmd.h"

// ---------- Handle Cmd Function -------------------
// Ham xu ly lenh CMD va luu gia tri vao cac bien
#include "handle_cmd.h"

// ---------- Handle Cmd Function -------------------
// Ham xu ly chuoi lenh CMD va luu gia tri vao bien tuong ung
void handle_Cmd(char *buff_cmd, HostwebTypedef *hostweb_node2)
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
            char *cmd = cmd_start;           // Tro den ten lenh (vi du: tempthreshold)
            char *value_str = colon_pos + 1; // Tro den gia tri (vi du: 45.50)

            // Xu ly va gan gia tri tuong ung
            if (strcmp(cmd, "tempthreshold") == 0) {
                hostweb_node2->temperature_threshold = atof(value_str);
            } else if (strcmp(cmd, "humidthreshold") == 0) {
                hostweb_node2->humid_threshold = atof(value_str);
            } else if (strcmp(cmd, "buzzerstatenode2") == 0) {
                hostweb_node2->buzzer_state = atoi(value_str);  // atoi dung cho so nguyen
            }
        }

        cmd_start = cmd_end + 1;  // Chuyen sang lenh tiep theo
    }
}

// Ham kiem tra va xu ly chuoi CMD tu node2
void cmd_process_data(char *buff_cmd, HostwebTypedef *hostweb_node2)
{
	if (strstr(buff_cmd, "id:node2")) {
			// Xu ly lenh cua node2
			handle_Cmd(buff_cmd, hostweb_node2);
//				HAL_Delay(200);

	}
}
