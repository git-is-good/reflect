#ifndef _REFLECT_HH_
#define _REFLECT_HH_

#include "func_args.hh"

#include <typeinfo>
#include <type_traits>
#include <string>
#include <vector>
#include <any>
#include <map>
#include <memory>
#include <exception>
#include <cassert>

// Reflection functionalities:
// 1, Find a class by its name
// 2, Find a method by class and method name
// 3, call a method found by functionality 2

class WrongCallException
    : public std::exception
{
    const char *what() const noexcept override {
        return "Invalid number of arguments";
    }
};

class FuncObjBase 
{
public:
    virtual ~FuncObjBase() = default;

    virtual std::any call(std::any that) {
        throw WrongCallException();
    }

    virtual std::any call(std::any that, std::any arg1) {
        throw WrongCallException();
    }

    virtual std::any call(std::any that, std::any arg1, std::any arg2) {
        throw WrongCallException();
    }

    virtual std::any call(std::any that, std::any arg1, std::any arg2, std::any arg3) {
        throw WrongCallException();
    }

    virtual const char *get_func_name() = 0;
};

class ClassObjBase
{
public:
    virtual ~ClassObjBase() = default;
    virtual FuncObjBase *getFunc(std::string const &) = 0;
    virtual std::any createInstance() = 0;
    virtual const char *get_class_name() = 0;
};

class ClassObjMgr 
{
public:
    void regist(ClassObjBase *obj) {
        classObjs[obj->get_class_name()] = obj;
    }

    static ClassObjMgr &Instance() {
        static ClassObjMgr obj;
        return obj;
    }

    ClassObjBase *getClass(std::string const &s) {
        return classObjs[s];
    }
private:
    std::map<std::string, ClassObjBase*> classObjs;
};


#define g_ClassObjMgr ClassObjMgr::Instance()

#define _REGISTER_CLS_FUNC_COMMON_BEGIN(className, funcName) \
    class _FuncObj_ ## className ## _ ## funcName           \
        : public FuncObjBase                                \
    {                                                       \
        friend class _ClassObj_ ## className;               \
    public:                                                 \


#define _REGISTER_CLS_FUNC_COMMON_END(className, funcName)  \
        const char *get_func_name() override {              \
            return func_name;                               \
        }                                                   \
    private:                                                \
        _FuncObj_ ## className ## _ ## funcName() {         \
            _ClassObj_ ## className::Instance().regist(this);   \
        }                                                   \
                                                            \
        static _FuncObj_ ## className ## _ ## funcName&     \
        Instance() { return obj_; }                         \
                                                            \
        const char *func_name = #funcName;                  \
        static _FuncObj_ ## className ## _ ## funcName obj_;\
    };                                                      \
                                                            \
    _FuncObj_ ## className ## _ ## funcName                 \
    _FuncObj_ ## className ## _ ## funcName :: obj_;

#define REGISTER_CLS_FUNC0(className, funcName)             \
    _REGISTER_CLS_FUNC_COMMON_BEGIN(className, funcName)    \
        std::any call(std::any that) override               \
        {                                                   \
            className &obj = std::any_cast<className&>(that);   \
            return type_utility::invoke_and_convert_void(&className::funcName, obj);    \
        }                                                   \
    _REGISTER_CLS_FUNC_COMMON_END(className, funcName)


#define REGISTER_CLS_FUNC1(className, funcName, type1)      \
    _REGISTER_CLS_FUNC_COMMON_BEGIN(className, funcName)    \
        std::any call(std::any that, std::any arg1) override    \
        {                                                   \
            className &obj = std::any_cast<className&>(that); \
            type1 s1 = std::any_cast<type1>(arg1);          \
            return type_utility::invoke_and_convert_void(&className::funcName, obj, s1);    \
        }                                                   \
    _REGISTER_CLS_FUNC_COMMON_END(className, funcName)


#define REGISTER_CLS_FUNC2(className, funcName, type1, type2)   \
    _REGISTER_CLS_FUNC_COMMON_BEGIN(className, funcName)    \
        std::any call(std::any that, std::any arg1, std::any arg2) override    \
        {                                                   \
            className &obj = std::any_cast<className&>(that); \
            type1 s1 = std::any_cast<type1>(arg1);          \
            type2 s2 = std::any_cast<type2>(arg2);          \
            return type_utility::invoke_and_convert_void(&className::funcName, obj, s1, s2);    \
        }                                                   \
    _REGISTER_CLS_FUNC_COMMON_END(className, funcName)


#define REGISTER_CLS_FUNC3(className, funcName, type1, type2, type3)    \
    _REGISTER_CLS_FUNC_COMMON_BEGIN(className, funcName)    \
        std::any call(std::any that, std::any arg1) override    \
        {                                                   \
            className &obj = std::any_cast<className&>(that);   \
            type1 s1 = std::any_cast<type1>(arg1);          \
            type2 s2 = std::any_cast<type2>(arg2);          \
            type3 s3 = std::any_cast<type3>(arg3);          \
            return type_utility::invoke_and_convert_void(&className::funcName, obj, s1, s2, s3);    \
        }                                                   \
    _REGISTER_CLS_FUNC_COMMON_END(className, funcName)


// put this in a .hh file
// just after the class definition
#define REGISTER_CLS(className)                             \
    class _ClassObj_ ## className                           \
        : public ClassObjBase                               \
    {                                                       \
    public:                                                 \
        static _ClassObj_ ## className&                     \
        Instance() {                                        \
            return obj_;                                    \
        }                                                   \
                                                            \
        void regist(FuncObjBase *func_obj) {                \
            funcs[func_obj->get_func_name()] = func_obj;    \
        }                                                   \
                                                            \
        FuncObjBase *getFunc(std::string const &s) override \
        {                                                   \
            return funcs[s];                                \
        }                                                   \
                                                            \
        std::any createInstance() override {                \
            return className{};                             \
        }                                                   \
                                                            \
        const char *get_class_name() override {             \
            return class_name;                              \
        }                                                   \
                                                            \
    private:                                                \
        _ClassObj_ ## className() {                         \
            g_ClassObjMgr.regist(this);                     \
        }                                                   \
                                                            \
        std::map<std::string, FuncObjBase*> funcs;          \
                                                            \
        const char *class_name = #className;                \
                                                            \
        static _ClassObj_ ## className obj_;                \
    };


#define REGISTER_CLS_CREATE_INSTANCE(className)             \
    _ClassObj_ ## className _ClassObj_ ## className :: obj_;


#endif /* _REFLECT_HH_ */

