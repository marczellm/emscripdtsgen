#pragma once
#include <string>

#define EMSCRIPTEN_BINDINGS(name) __attribute__((constructor)) static void emscripdtsgen_init_##name()

namespace emscripten {

template<typename ReturnType, typename ... Args>
void function(const std::string& name, ReturnType (*pFunction) (Args ... args)) {

}

template<typename T>
void constant(const std::string& name, const T& value) {

}

template<typename T>
struct enum_ {
    enum_ (const std::string& name) {

    }

    enum_& value(const std::string& name, T value) {
        return *this;
    }
};

template<typename T>
struct value_object {
    value_object (const std::string& name) {

    }
    
    template<typename FieldType>
    value_object& field(const std::string& name, FieldType T::*field) {
        return *this;
    }
};

template<class T>
struct class_ {
    class_ (const std::string& name) {

    }

    template<typename FieldType>
    class_& property(const std::string& name, FieldType T::*field) {
        return *this;
    }

    template<typename ReturnType, typename ... Args>
    class_& function(const std::string& name, ReturnType (T::*pFunction) (Args ... args)) {
        return *this;
    }

    template<typename ReturnType, typename ... Args>
    class_& function(const std::string& name, ReturnType (T::*pFunction) (Args ... args) const) {
        return *this;
    }

    template<typename ReturnType, typename ... Args>
    class_& class_function(const std::string& name, ReturnType (*pFunction) (Args ... args)) {
        return *this;
    }

    template<typename SmartPtrType>
    class_& smart_ptr(const std::string&) {
        return *this;
    }

    template<typename SmartPtrType, typename... Args>
    class_& smart_ptr_constructor(const std::string& smartPtrName, SmartPtrType (*factory)(Args...)) {
        return *this;
    }
};

}