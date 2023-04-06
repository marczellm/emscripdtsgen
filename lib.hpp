#pragma once
#include <string>
#include <memory>

int function0();

float function1(int arg);

double function2(short arg1, long arg2);

const std::string konstans = "constant";

enum class Enum {
    Value1, Value2, Value3
};

struct Struct1 {
    bool mBool;
    Enum mEnum;
};

struct Struct2 {
    Struct1 mStruct;
};

class Class1 {
public:
    static std::shared_ptr<Class1> Create();

    int mField;
    const char mChar = 'c';
    Struct1 get_struct1();
    Struct2 get_struct2() const;
};

class Class2 {
    public:
    Class2 (std::string);

    std::shared_ptr<Class1> get_class1(Struct2);
    std::shared_ptr<Class2> get_class2(std::shared_ptr<Class1>);
};