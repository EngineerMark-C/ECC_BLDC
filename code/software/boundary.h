#ifndef _boundary_h_
#define _boundary_h_

//************************************* 边界保护 ******************************************//
extern float SAFETY_MARGIN_X;
extern float SAFETY_MARGIN_Y;
extern float SAFETY_X_MAX;
extern float SAFETY_X_MIN;
extern float SAFETY_Y_MAX;
extern float SAFETY_Y_MIN;

void Safety_Boundary_Check(void);
void Calculate_Safety_Boundary(NavigationFlag flag);

#endif
