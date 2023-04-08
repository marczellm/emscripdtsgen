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
typedef std::function<std::string()> LazyString;

class Declarable {
protected:
    Declarable(const std::string& name): mName(name) {}

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

    std::string to_function_signature() const {
        std::string ret = "function " + mName + "(";
        
        if (!mArguments.empty()) {
            size_t i = 0;
            for (; i < mArguments.size() - 1; i++) {
                ret += "arg" + std::to_string(i) + ": " + mArguments[i]() + ", ";
            }
            ret += "arg" + std::to_string(i) + ": " + mArguments[i]();        
        }
        return ret + "): " + mType();
    }

    std::string to_method_signature() const {
        std::string ret = mName + "(";
        
        if (!mArguments.empty()) {
            size_t i = 0;
            for (; i < mArguments.size() - 1; i++) {
                ret += "arg" + std::to_string(i) + ": " + mArguments[i]() + ", ";
            }
            ret += "arg" + std::to_string(i) + ": " + mArguments[i]();        
        }
        return ret + "): " + mType();
    }
};

template<typename T>
struct TsName {
    static std::string sName;

    static LazyString lazy_name() {
        return [] () {
            if constexpr (std::is_arithmetic<T>()) {
                return "number"s;
            }
            if (sName.empty()) {
                std::cout << "warning: binding for type " << typeid(T).name() << " not found!" << std::endl;
                return "unknown"s;
            }
            return sName;
        };
    }
};

template<typename T> std::string TsName<T>::sName;

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

template<>
struct TsName<std::string> {
    static LazyString lazy_name() {
        return [] () {
            return "string";
        };
    }
};

template<typename T>
struct TsName<const T> : public TsName <T> {};

template<typename T>
struct TsName<T&> : public TsName <T> {};

template<typename T>
struct TsName<T&&> : public TsName <T> {};

template<typename T>
struct TsName<std::shared_ptr<T>> : public TsName <T> {};

namespace emscripten {

template<typename ReturnType, typename ... Args>
void function(const std::string& name, ReturnType (*pFunction)(Args ... args)) {
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
        ModuleContents::Get().mDeclarations.push_back(std::make_unique<enum_>(*this));
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
        ModuleContents::Get().mDeclarations.push_back(std::make_unique<value_object>(*this));
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
            ret += "\n\t" + field.mName + ": " + field.mType();
        }
        return ret + "\n}";
    }
};

template<class T>
class class_: public Declarable {
    std::vector<Signature> mConstructors;
    std::vector<Signature> mProperties;
    std::vector<Signature> mMethods;
    std::vector<Signature> mStaticMethods;
public:
    class_(const std::string& name): Declarable(name) {
        TsName<T>::sName = name;
    }

    ~class_() {
        ModuleContents::Get().mDeclarations.push_back(std::make_unique<class_>(*this));
    }

    template<typename FieldType>
    class_& property(const std::string& name, FieldType T::*field) {
        mProperties.push_back({
            name, TsName<FieldType>::lazy_name(), {}
        });
        return *this;
    }

    template<typename ReturnType, typename ... Args>
    class_& function(const std::string& name, ReturnType (T::*pFunction)(Args ... args)) {
        mMethods.push_back(Signature {
            name,
            TsName<ReturnType>::lazy_name(),
            collect_arg_types<Args...>()
        });
        return *this;
    }

    template<typename ReturnType, typename ... Args>
    class_& function(const std::string& name, ReturnType (T::*pFunction)(Args ... args) const) {
        mMethods.push_back(Signature {
            name,
            TsName<ReturnType>::lazy_name(),
            collect_arg_types<Args...>()
        });        
        return *this;
    }

    template<typename ReturnType, typename ... Args>
    class_& class_function(const std::string& name, ReturnType (*pFunction)(Args ... args)) {
        mStaticMethods.push_back(Signature {
            name,
            TsName<ReturnType>::lazy_name(),
            collect_arg_types<Args...>()
        });
        
        return *this;
    }

    template<typename SmartPtrType>
    class_& smart_ptr(const std::string&) {
        return *this;
    }

    template<typename SmartPtrType, typename... Args>
    class_& smart_ptr_constructor(const std::string& smartPtrName, SmartPtrType (*factory)(Args...)) {
        mConstructors.push_back(Signature {
            "new",
            TsName<T>::lazy_name(),
            collect_arg_types<Args...>()
        });
        return *this;
    }

    std::string declaration() const override {
        std::string ret = "interface " + mName + " extends CppObjectHandle {";

        for (const auto& prop: mProperties) {
            ret += "\n\t" + prop.mName + ": " + prop.mType();
        }
        
        for (const auto& method: mMethods) {
            ret += "\n\t" + method.to_method_signature();
        }
       
        return ret + "\n}";
    }

    std::optional<std::string> export_on_module() const override {
        if (mConstructors.empty() && mStaticMethods.empty()) {
            return std::nullopt;
        }

        std::string ret = mName + ": {\n\t\tprototype: " + mName;

        for (const auto& constructor: mConstructors) {
            ret += "\n\t\t" + constructor.to_method_signature();
        }

        for (const auto& method: mStaticMethods) {
            ret += "\n\t\t" + method.to_method_signature();
        }

        return ret + "\n\t}";
    }
};

}