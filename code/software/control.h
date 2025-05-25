#ifndef _control_h_
#define _control_h_

extern float target_speed;
extern float target_angle;

//************************************* 边界保护 ******************************************//
extern float SAFETY_X_MAX;
extern float SAFETY_X_MIN;
extern float SAFETY_Y_MAX;
extern float SAFETY_Y_MIN;

//************************************* 速度管理 ******************************************//
extern float MAX_SPEED;
extern float MIN_SPEED;
extern float APPROACH_SPEED;
extern float BRAKING_DISTANCE;

//************************************* 点位数据 ******************************************//
extern double GPS_Point[MAX_GPS_POINTS][2];                                     // GPS 数据
extern float INS_Point[MAX_INS_POINTS][2];                                      // INS 点位
extern float GPS_ENU[MAX_GPS_POINTS][2];                                        // ENU点位
extern float S_Point[MAX_INS_POINTS][2];                                        // S 型走位点位
float Adjusted_GPS_ENU[MAX_GPS_POINTS][2];                                      // 调整后的 GPS ENU 坐标

//************************************* GPS 点位处理*****************************************//
extern uint8_t GPS_Point_Index;                                                 // GPS 数据索引
extern uint8_t Start_GPS_Point;
extern uint8_t End_GPS_Point;
extern uint8_t NOW_GPS_Point;

//************************************* INS 点位处理******************************************//
extern uint8_t INS_Point_Index;                                                 // INS 数据索引
extern uint8_t Start_INS_Point;
extern uint8_t End_INS_Point;
extern uint8_t NOW_INS_Point;

//************************************* S型 点位处理******************************************//
extern float S_Distance;
extern uint8_t S_Point_Index;                                                  // S 型走位数据索引
extern uint8_t Start_S_Point;
extern uint8_t End_S_Point;
extern uint8_t NOW_S_Point;

extern uint8_t GPS_TO_INS_POINT;                                                // 切换导航点位

void Safety_Boundary_Check(void);
void Speed_Management(float distance);

void WGS84_to_ENU_Init(void);
void WGS84_to_ENU(double lat, double lon, float* east, float* north);
void WGS84_to_IMU_ENU_Init(void);
void GPS_Navigation(void);
void GPS_ENU_Navigation(void);
void INS_Navigation(void);
void GPS_INS_Navigation(void);
void GPS_ENU_INS_Navigation(void);

void S_Point_Generate_All(void);


#endif
