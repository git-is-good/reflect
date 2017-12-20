#include "func_args.hh"

using namespace type_utility;

#include <typeinfo>
#include <type_traits>
#include <iostream>
#include <vector>

void funny() {}

template<class T>
void great(T&&) {
    std::cout << "great called\n";
}

struct Hello {};
struct World {};

void test() {
    using f = void(Hello, Hello &, const Hello &, std::vector<World>, double);

    std::cout << typeid(get_ret_type_t<f>).name() << "\n";
    std::cout << typeid(get_arg_type_at_index_t<f, 0>).name() << "\n";
    std::cout << typeid(get_arg_type_at_index_t<f, 1>).name() << "\n";
    std::cout << typeid(get_arg_type_at_index_t<f, 2>).name() << "\n";
    std::cout << typeid(get_arg_type_at_index_t<f, 3>).name() << "\n";

    std::cout << std::boolalpha;
    std::cout << (typeid(get_arg_type_at_index_t<f, 0>) == typeid(get_arg_type_at_index_t<f, 1>))
        << "\n";

    std::cout << (typeid(get_arg_type_at_index_t<f, 0>) == typeid(get_arg_type_at_index_t<f, 2>))
        << "\n";

//    get_arg_type_at_index_t<f, 5> h; // should fail to compile

    great(invoke_and_convert_void(funny));
}

int main() {
    test();
}
