#pragma once
#include <string>
#include <vector>
#include <functional>
#include <iostream>
#include <typeinfo>
#include <optional>

using namespace std::string_literals;

#define EMSCRIPTEN_BINDINGS(name) __attribute__((constructor)) static void emscripdtsgen_init_##name()

// Declarations can be in any order, therefore we can only count on all type names being registered at the time we start printing them
typedef std::function<std::string ()> LazyString;

class Declarable {
protected:
    Declarable(const std::string& name): mName (name) {}

public:
    const std::string mName;
    virtual ~Declarable() = default;
    virtual std::string declaration() const = 0;
    virtual std::optional<std::string> export_on_module() const {
        return std::nullopt;
    }
};

struct Signature {
    std::string mName;
    LazyString mType;
    std::vector<LazyString> mArguments;
};

template<typename T>
struct TsName {
    static std::string sName;

    static LazyString lazy_name() {
        return [] () {
            if (sName.empty ()) {
                std::cout << "warning: binding for type " << typeid(T).name() << " not found!" << std::endl;
                return "unknown"s;
            }
            return sName;
        };
    }
};

template<typename ... Args>
std::vector<LazyString> collect_arg_types() {
    return { TsName<Args>::lazy_name() ... };
}

struct ModuleContents {
    static ModuleContents& Get() {
        static ModuleContents instance;
        return instance;
    }

    std::vector<Signature> mConstants;
    std::vector<Signature> mFunctions;
    std::vector<std::unique_ptr<Declarable>> mDeclarations;
};

template<typename T> std::string TsName<T>::sName;

template<>
struct TsName<bool> {
    static LazyString lazy_name() {
        return [] () {
            return "boolean";
        };
    }
};

template<>
struct TsName<void> {
    static LazyString lazy_name() {
        return [] () {
            return "void";
        };
    }
};

namespace emscripten {

template<typename ReturnType, typename ... Args>
void function(const std::string& name, ReturnType (*pFunction) (Args ... args)) {
    ModuleContents::Get().mFunctions.push_back(Signature {
        name,
        TsName<ReturnType>::lazy_name(),
        collect_arg_types<Args...>()
    });
}

template<typename T>
void constant(const std::string& name, const T&) {
    ModuleContents::Get().mConstants.push_back({
        name,
        TsName<T>::lazy_name()
    });
}

template<typename T>
class enum_: public Declarable {
    std::vector<std::string> mValues;

public:
    enum_(const std::string& name): Declarable(name) {
        TsName<T>::sName = name;
    }

    ~enum_() {
        ModuleContents::Get().mDeclarations.push_back(std::make_unique<enum_> (*this));
    }

    enum_& value(const std::string& name, T value) {
        mValues.push_back(name);
        return *this;
    }

    std::string declaration() const override {
        std::string ret = "declare enum " + mName + " { ";
        for (const auto& value: mValues) {
            ret += value + ", ";
        }
        return ret + "}";
    }
    
    std::optional<std::string> export_on_module() const override {
        return mName + ": typeof " + mName;
    }
};

template<typename T>
class value_object: public Declarable {
    std::vector<Signature> mFields;
public:
    value_object(const std::string& name): Declarable(name) {
        TsName<T>::sName = name;
    }

    ~value_object() {
        ModuleContents::Get().mDeclarations.push_back(std::make_unique<value_object> (*this));
    }
    
    template<typename FieldType>
    value_object& field(const std::string& name, FieldType T::*field) {
        mFields.push_back({
            name, TsName<FieldType>::lazy_name(), {}
        });
        return *this;
    }

    std::string declaration() const override {
        std::string ret = "interface " + mName + " {";
        for (const auto& field: mFields) {
            ret += "\n\t" + field.mName + ": " + field.mType () + "\n";
        }
        return ret + "}";
    }
};

template<class T>
class class_: public Declarable {
    bool mConstructible = false;
    std::vector<Signature> mProperties;
    std::vector<Signature> mMethods;
    std::vector<Signature> mStaticMethods;
public:
    class_(const std::string& name): Declarable(name) {
        TsName<T>::sName = name;
    }

    ~class_() {
        ModuleContents::Get().mDeclarations.push_back(std::make_unique<class_> (*this));
    }

    std::optional<std::string> export_on_module() const override {
        return mName + ": { prototype: " + mName + " }"; 
    }

    template<typename FieldType>
    class_& property(const std::string& name, FieldType T::*field) {
        mProperties.push_back({
            name, TsName<FieldType>::lazy_name(), {}
        });
        return *this;
    }

    template<typename ReturnType, typename ... Args>
    class_& function(const std::string& name, ReturnType (T::*pFunction) (Args ... args)) {
        mMethods.push_back(Signature {
            name,
            TsName<T>::lazy_name(),
            collect_arg_types<Args...> ()
        });
        return *this;
    }

    template<typename ReturnType, typename ... Args>
    class_& function(const std::string& name, ReturnType (T::*pFunction) (Args ... args) const) {
        mMethods.push_back(Signature {
            name,
            TsName<T>::lazy_name(),
            collect_arg_types<Args...> ()
        });        
        return *this;
    }

    template<typename ReturnType, typename ... Args>
    class_& class_function(const std::string& name, ReturnType (*pFunction) (Args ... args)) {
        mStaticMethods.push_back(Signature {
            name,
            TsName<T>::lazy_name(),
            collect_arg_types<Args...> ()
        });
        
        return *this;
    }

    template<typename SmartPtrType>
    class_& smart_ptr(const std::string&) {
        return *this;
    }

    template<typename SmartPtrType, typename... Args>
    class_& smart_ptr_constructor(const std::string& smartPtrName, SmartPtrType (*factory)(Args...)) {
        mConstructible = true;
        return *this;
    }

    std::string declaration() const override {
        std::string ret = "interface " + mName + " {";
       
        return ret + "\n\tdelete(): void\n}";
    }
};

}