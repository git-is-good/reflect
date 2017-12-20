#include "reflect.hh"

#include <functional>
#include <iostream>

class Pluser {
public:
    int addone(int a) {
        std::cout << "Secret: " << secret << ", " << a << " addone called.\n";
        return a + 1;
    }

    double addtwo(double a, double b) {
        std::cout << "Secret: " << secret << ", " << a << " + " << b << " addtwo called.\n";
        return a + b;
    }

    void incone(std::reference_wrapper<int> a) {
        a += 1;
    }

    std::string secret;
};

REGISTER_CLS(Pluser);

REGISTER_CLS_CREATE_INSTANCE(Pluser);

REGISTER_CLS_FUNC1(Pluser, addone, int);
REGISTER_CLS_FUNC2(Pluser, addtwo, double, double);
REGISTER_CLS_FUNC1(Pluser, incone, std::reference_wrapper<int>);

void test() {
    ClassObjBase *cls = g_ClassObjMgr.getClass("Pluser");
    std::cout << cls->get_class_name() << "\n";

    FuncObjBase *func = cls->getFunc("addone");

    assert(func);

    func->call(cls->createInstance(), 23);

    FuncObjBase *func2 = cls->getFunc("addtwo");

    assert(func2);

    auto res = func2->call(cls->createInstance(), 23.0, 45.0);
    std::cout << "Result: " << std::any_cast<double>(res) << "\n";


    Pluser ins = std::any_cast<Pluser>(cls->createInstance());

    ins.secret = "Holy";
    ins.addtwo(123, 34);

    int a = 0;
    std::cout << "Before: a = " << a << "\n";
    FuncObjBase *func3 = cls->getFunc("incone");
    func3->call(cls->createInstance(), std::ref(a));
    std::cout << "After:  a = " << a << "\n";

    ins.incone(a);
    std::cout << "Encore after:  a = " << a << "\n";
}

int main() {
    test();
}
