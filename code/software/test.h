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

extern TestFlag test_flag;

void Speed_Management_For_Test3(float distance);


#endif