#ifndef _menu_h_
#define _menu_h_

void Button_init(void);
void Menu(void);

void Display_Menu(void);
void Display_Main_Menu(void);
void Display_Steer_Menu(void);
void Display_Speed_Imu_Info(void);
void Display_GPS_Info(void);
void Display_GPS_Point(void);
void Display_GPS_INS_Path(void);
void Display_Speed_Manage_Menu(void);
void Display_Calibrate_Gyro(void);
void Display_INS_Point(void);
void Display_ENU_Point(void);
void Display_Nav_Mode_Menu(void);
void Display_S_Point(void);
void Display_Camera(void);
void Display_Boundary(void);

void Main_Menu_Key_Process(void);
void Steer_Menu_Key_Process(void);
void GPS_Point_Menu_Key_Process(void);
void GPS_INS_Path_Menu_Key_Process(void);
void Speed_Manage_Menu_Key_Process(void);
void Calibrate_Gyro_Menu_Key_Process(void);
void INS_Point_Menu_Key_Process(void);
void ENU_Point_Menu_Key_Process(void);
void Nav_Mode_Key_Process(void);
void S_Point_Menu_Key_Process(void);
void Camera_Menu_Key_Process(void);
void Boundary_Menu_Key_Process(void);

#endif
