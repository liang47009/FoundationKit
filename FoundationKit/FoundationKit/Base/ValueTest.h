#pragma once
#include "assert.h"
#include "Value.h"

USING_NS_FK;

#define EQUAL_PCHAR(a, b) assert(strcmp(a, b)==0)
#define EQUAL_STR(a,b)    assert(a.compare(b) == 0)
#define EQUAL_VAL(a, b)   assert(a == b)

static void valueClassTest()
{
    unsigned char      v0 = 'A';
    unsigned short     v1 = 10;
    unsigned int       v2 = 100;
    unsigned long      v3 = 1000;
    unsigned long long v4 = 10000;
    bool v5 = true;
    char v6 = 'B';
    short v7 = 11;
    int   v8 = 101;
    long  v9 = 1001;
    long long v10 = 10001;
    float     v11 = 10.f;
    double    v12 = 100.f;
    char*     v13 = "sfsdfsdfdsfds";
    std::string v14 = "ffffff00000ffff";

    Value vv0(v0); 
    Value vv1(v1); 
    Value vv2(v2); 
    Value vv3(v3); 
    Value vv4(v4); 
    Value vv5(v5); 
    Value vv6(v6); 
    Value vv7(v7); 
    Value vv8(v8); 
    Value vv9(v9); 
    Value vv10(v10);
    Value vv11(v11);
    Value vv12(v12);
    Value vv13(v13);
    Value vv14(v14); 

    long val = vv9.as<long>();

    EQUAL_VAL(v0, vv0.as<unsigned char>());
    EQUAL_VAL(v1, vv1.as<unsigned short>());
    EQUAL_VAL(v2, vv2.as<unsigned int>());
    EQUAL_VAL(v3, vv3.as<unsigned long>());
    EQUAL_VAL(v4, vv4.as<unsigned long long>());
    EQUAL_VAL(v5, vv5.as<bool>());
    EQUAL_VAL(v6, vv6.as<char>());
    EQUAL_VAL(v7, vv7.as<short>());
    EQUAL_VAL(v8, vv8.as<int>());
    EQUAL_VAL(v9, vv9.as<long>());
    EQUAL_VAL(v10, vv10.as<long long>());
    EQUAL_VAL(v11, vv11.as<float>());
    EQUAL_VAL(v12, vv12.as<double>());
    EQUAL_PCHAR(v13, vv13.as<char*>());
    EQUAL_STR(v14, vv14.as<std::string>());


    Value vv15(vv13);
    EQUAL_PCHAR(v13, vv15.as<char*>());


    Value vv17 = vv14;
    EQUAL_STR(v14, vv17.as<std::string>());

    bool bis_same = vv17 == vv14;
    bis_same = vv17 != vv14;

    bis_same = vv17 == vv15;


    Value vv16(std::move(vv13));
    EQUAL_PCHAR(v13, vv16.as<char*>());
    Value vv18 = std::move(vv14);
    EQUAL_STR(v14, vv18.as<std::string>());

    Value vv19;
    vv19 = v8;



   
}