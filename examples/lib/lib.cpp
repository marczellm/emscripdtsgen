#include "lib.hpp"

int function0() {
    return 0;
}

float function1(int arg) {
    return arg;
}

double function2(short arg1, long arg2) {
    return arg1 + arg2;
}

std::shared_ptr<Class1> Class1::Create() {
    return std::make_shared<Class1>();
}

Struct1 Class1::get_struct1() {
    return {};
}

Struct2 Class1::get_struct2() const {
    return {};
}

std::shared_ptr<Class1> Class2::get_class1(Struct1) {
    return nullptr;
}

std::shared_ptr<Class2> Class2::get_class2(std::shared_ptr<Class1>) {
    return nullptr;
}

Class2::Class2 (std::string) {

}