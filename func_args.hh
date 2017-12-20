#ifndef _FUNC_ARGS_HH_
#define _FUNC_ARGS_HH_

#include <functional>
#include <type_traits>
#include <utility>

namespace type_utility {

struct no_such_type {};

template<class... T>
struct type_list {};

template<class typelist, int I>
struct get_type_at_index;

template<int I>
struct get_type_at_index<type_list<>, I> {
    using type = no_such_type;
};

template<class T0, class... Ts, int I>
struct get_type_at_index<type_list<T0, Ts...>, I> {
    using type = typename get_type_at_index<type_list<Ts...>, I - 1>::type;
};

template<class T0, class... Ts>
struct get_type_at_index<type_list<T0, Ts...>, 0> {
    using type = T0;
};

template<class Func>
struct get_ret_type;

template<class Func, int I>
struct get_arg_type_at_index;

template<class Ret, class... Args>
struct get_ret_type<Ret(Args...)> {
    using type = Ret;
};

template<class Func>
using get_ret_type_t = typename get_ret_type<Func>::type;

template<class Ret, class... Args, int I>
struct get_arg_type_at_index<Ret(Args...), I> {
    using type = typename get_type_at_index<
        type_list<Args...>, I
        >::type;
};

template<class Func, int I>
using get_arg_type_at_index_t = typename get_arg_type_at_index<Func, I>::type;

template<bool IsVoid>
struct invoke_and_convert_void_impl;

template<>
struct invoke_and_convert_void_impl<true>
{
    template<class Func, class... Args>
    static no_such_type
    do_(Func&& func, Args&&... args)
    {
        std::invoke(std::forward<Func>(func), std::forward<Args>(args)...);
        return no_such_type{};
    }
};

template<>
struct invoke_and_convert_void_impl<false>
{
    template<class Func, class... Args>
    static decltype(auto)
    do_(Func&& func, Args&&... args)
    {
        return std::invoke(std::forward<Func>(func), std::forward<Args>(args)...);
    }
};

/* invoke a function, if it returns void,
 * then return a no_such_type instance 
 */
template<class Func, class... Args>
decltype(auto)
invoke_and_convert_void(Func&& func, Args&&... args)
{
    return invoke_and_convert_void_impl<
        std::is_same_v<
        std::result_of_t<Func(Args...)>, void 
        >>::
        do_(std::forward<Func>(func),
            std::forward<Args>(args)...
           );
}

} /* namespace type_utility */

#endif /* _FUNC_ARGS_HH_ */

