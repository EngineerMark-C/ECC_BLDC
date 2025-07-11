#ifndef TEST_H
#define TEST_H

typedef enum
{
    NO_TEST,
    TEST__1,
    TEST__2,
    TEST__3,
    TEST__4
} TestFlag;

typedef struct
{
    uint8_t Point_Index;        // 点位索引
    float Through_Speed;        // 通过速度
} Test3Data;


extern TestFlag test_flag;
extern Test3Data Test3Element[4];

void Speed_Management_For_Test3(float distance, uint8_t i);


#endif