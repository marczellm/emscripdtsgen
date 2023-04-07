#include "binding_api.hpp"
#include "lib/lib.hpp"

EMSCRIPTEN_BINDINGS (ExampleBindings) {
    using namespace emscripten;

    function("function0", function0);
    function("function1", function1);
    function("function2", function2);

    constant("konstans", konstans);

    enum_<Enum>("Enum")
    .value("Value1", Enum::Value1)
    .value("Value2", Enum::Value2)
    .value("Value3", Enum::Value3);

    value_object<Struct1> ("Struct1")
    .field("mBool", &Struct1::mBool)
    .field("mEnum", &Struct1::mEnum);

    value_object<Struct2> ("Struct2")
    .field("mStruct", &Struct2::mStruct);

    class_<Class1> ("Class1")
    .smart_ptr<std::shared_ptr<Class1>> ("Class1")
    .class_function("Create", Class1::Create)
    .property("mField", &Class1::mField)
    .property("mChar", &Class1::mChar)
    .function("get_struct1", &Class1::get_struct1)
    .function("get_struct2", &Class1::get_struct2);

    class_<Class2> ("Class2")
    .smart_ptr_constructor("Class2", std::make_shared<Class2, std::string>)
    .function("get_class1", &Class2::get_class1)
    .function("get_class2", &Class2::get_class2);

    class_<Class3> ("Class3");
}