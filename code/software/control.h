#ifndef _control_h_
#define _control_h_

extern float target_speed;
extern float target_angle;

//************************************** 速度管理 ******************************************//
extern float MAX_SPEED;
extern float MIN_SPEED;
extern float APPROACH_SPEED;
extern float BRAKING_DISTANCE;
extern float GPS_SWITCH_DISTANCE;                                                // GPS 切换距离
extern float INS_SWITCH_DISTANCE;                                                // INS 切换距离

//************************************** 点位数据 *******************************************//
extern double GPS_Point[MAX_GPS_POINTS][2];                                     // GPS 数据
extern float INS_Point[MAX_INS_POINTS][2];                                      // INS 点位
extern float GPS_ENU[MAX_GPS_POINTS][2];                                        // ENU点位
extern float S_Point[MAX_INS_POINTS][2];                                        // S 型走位点位
extern float S_Point_Navigation_Frame[MAX_INS_POINTS][2];                       // S 型走位点导航坐标系
extern float INS_Point_Navigation_Frame[MAX_INS_POINTS][2];                     // INS 点位 导航坐标系

//************************************* GPS 点位处理*****************************************//
extern uint8_t GPS_Point_Index;                                                 // GPS 数据索引
extern uint8_t Start_GPS_Point;
extern uint8_t End_GPS_Point;
extern uint8_t NOW_GPS_Point;

extern double Direction_Point[2][2];                                          // 两个发车方向GPS点
extern float Start_Direction;                                                  // 发车方向角度
extern uint8_t Direction_Point_Index;                                          // 发车方向点索引

//************************************* INS 点位处理******************************************//
extern uint8_t INS_Point_Index;                                                 // INS 数据索引
extern uint8_t Start_INS_Point;
extern uint8_t Back_INS_Point;
extern uint8_t End_INS_Point;
extern uint8_t NOW_INS_Point;

//************************************* S型 点位处理******************************************//
extern float S_Distance;
extern uint8_t S_Point_Index;                                                  // S 型走位数据索引
extern uint8_t Start_S_Point;
extern uint8_t End_S_Point;
extern uint8_t NOW_S_Point;

extern float next_target_angle;                                                // 下一个目标角度

extern uint8_t GPS_TO_INS_Point;                                                // 切换导航点位

extern uint8_t reach_flag;                                                      // 到达标志位
extern uint8_t angle_flag;                                                      // 角度到达标志位

void Safety_Boundary_Check(void);
void Speed_Management(float distance);
void Brake(void);

void WGS84_to_ENU_Init(void);
void WGS84_to_ENU(double lat, double lon, float* east, float* north);
void Vehicle_To_Navigation_INS(void);
void Vehicle_To_Navigation_S(void);

void Mirror_INS_Point_Generate(void);
void Get_Start_Direction(void);

void GPS_Navigation(void);
void GPS_ENU_Navigation(void);
void INS_Navigation(void);
void GPS_INS_Navigation(void);
void GPS_ENU_INS_Navigation(void);

void S_Point_Generate_All(void);


#endif
