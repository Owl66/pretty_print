#include <iostream>
#include <stddef.h>
#include <type_traits>
#include <tuple>
#include <set>
#include <vector>
#include <forward_list>
#include <iterator>
#include <utility>


template <typename T>
struct Container_traits {

    template <typename U>
    static std::true_type f(typename U::const_iterator*);
    static std::false_type f(...);

    using type = decltype(f<T>(0));
};

template <typename T>
struct Is_container : public Container_traits<T>::type {};

template <typename T, std::size_t N>
struct Is_container<T[N]> : public std::true_type {};

template <typename T, typename Ch, typename Al>
struct Is_container<std::basic_string<T, Ch, Al>> : public std::false_type {};

//for cross-platform compiler.

#if defined (__MCS__VER) && __MCS__VER == 1600
#define TUPLE_PARAMS \
    typename T0, typename T1, typename T2, typename T3, typename T4, \
    typename T5, typename T6, typename T7, typename T8, typename T9
#define TUPLE_ARGS T0, T1, T2, T3, T4, T5, T6, T7, T8, T9

#elif defined (__GNUC__)
#define TUPLE_PARAMS typename... Args
#define TUPLE_ARGS Args...
#endif

struct Default_formatter {

    template <typename T> void prefix(std::ostream& os, T const&) {
        os << "[ ";
    }
    template <typename T> void separator(std::ostream& os, T const&) const {
        os << ", ";
    }
    template <typename T> void suffix(std::ostream& os, T const&) const {
        os << "] ";
    }

    template <typename A, typename B>
    void prefix(std::ostream& os, std::pair<A,B> const& p) const {
        os << "( ";
    }
    template <typename A, typename B>
    void separator(std::ostream& os, std::pair<A,B> const& p) const {
        os << ", ";
    }
    template <typename A, typename B>
    void suffix(std::ostream& os, std::pair<A,B> const& p) const {
        os << ") ";
    }

    template<TUPLE_PARAMS>
    void prefix(std::ostream& os, std::tuple<TUPLE_ARGS> const& t) const {
        os << "( ";
    }
    template<TUPLE_PARAMS>
    void separator(std::ostream& os, std::tuple<TUPLE_ARGS> const& t) const {
        os << ", ";
    }
    template<TUPLE_PARAMS>
    void suffix(std::ostream& os, std::tuple<TUPLE_ARGS> const& t) const {
        os << ") ";
    }

    template <typename K, typename C, typename A>
    void prefix(std::ostream& os, std::set<K,C,A> const&) const {
        os << "{ ";
    }
    template <typename K, typename C, typename A>
    void separator(std::ostream& os, std::set<K,C,A> const&) const {
        os << ", ";
    }
    template <typename K, typename C, typename A>
    void suffix(std::ostream& os, std::set<K,C,A> const&) const {
        os << "} ";
    }

    template <typename T>
    void print(std::ostream& os, T const& t) const {
        os << t;
    }

    template <typename T, typename Ch, typename A>
    void element(std::ostream& os, std::basic_string<T,Ch,A> const &s) const {
        os << "\"" << s << "\"";
    }

};

//template <typename T>
//void print(std::ostream& os, T const& t);

template <typename T, typename Formatter = Default_formatter>
void print(std::ostream& os, T const& t, Formatter const& fmt = Formatter());

template <typename A, typename B, typename Formatter>
void print(std::ostream& os, std::pair<A,B>& p, Formatter& fmt);

template <TUPLE_PARAMS, typename Formatter>
void print(std::ostream& os, std::tuple<TUPLE_ARGS>& t, Formatter& fmt);


template <typename Tuple, typename Formatter, std::size_t N>
void print_tuple_helper(std::ostream& os, Tuple& t, Formatter& fmt, std::integral_constant<std::size_t, N>);

template <typename Tuple, typename Formatter, std::size_t N>
void print_tuple_helper(std::ostream& os, Tuple& t, Formatter& fmt, std::integral_constant<std::size_t, 1>);

template <typename Tuple, typename Formatter, std::size_t N>
void print_tuple_helper(std::ostream& os, Tuple& t, Formatter& fmt, std::integral_constant<std::size_t, 0>);


template <typename C, typename Formatter>
void print_container_helper(std::ostream& os, C const& c, std::true_type, Formatter const& fmt);

template <typename C, typename Formatter>
void print_container_helper(std::ostream& os, C const& c, std::false_type, Formatter const& fmt);



/*template <typename T>
void print(std::ostream& os, T const& t) {
    print(os, t);
}*/

template <typename C, typename Formatter = Default_formatter>
void print(std::ostream& os, C& c, Formatter const& fmt = Formatter()) {
    print_container_helper(os, c, typename Is_container<C>::type(), fmt);
}

template <typename A, typename B, typename Formatter>
void print(std::ostream& os, std::pair<A,B>& p, Formatter& fmt) {
    fmt.prefix(os, p);
    print(os, p.first, fmt);
    fmt.separator(os, p);
    print(os, p.second, fmt);
    fmt.suffix(os, p);
}

template <TUPLE_PARAMS, typename Formatter>
void print(std::ostream& os, std::tuple<TUPLE_ARGS> const& t, Formatter const& fmt) {
    const std::size_t N = std::tuple_size<std::tuple<TUPLE_ARGS>>::value;
    fmt.prefix(os, t);
    print_tuple_helper(os, fmt, std::integral_constant<std::size_t, N>());
    fmt.suffix(os, t);
}

template <typename Tuple, typename Formatter, std::size_t I>
void print_tuple_helper(std::ostream& os, Tuple& t, Formatter& fmt, std::integral_constant<std::size_t, I>) {

    const std::size_t N = std::tuple_size<Tuple>::value;
    print(os, std::get<N-I>(t), fmt);
    fmt.separator(os, t);
    print_tuple_helper(os, t, fmt, std::integral_constant<std::size_t, I-1>());

}
template <typename Tuple, typename Formatter>
void print_tuple_helper(std::ostream& os, Tuple& t, Formatter& fmt, std::integral_constant<std::size_t, 1>) {

    const std::size_t N = std::tuple_size<Tuple>::value;
    print(os, std::get<N-1>(t), fmt);
}

template <typename Tuple, typename Formatter>
void print_tuple_helper(std::ostream& os, Tuple& t, Formatter& fmt, std::integral_constant<std::size_t, 0>) {
}

template <typename C, typename Formatter>
void print_container_helper(std::ostream& os, C const& c, std::true_type, Formatter const& fmt) {
    fmt.prefix(os, c);
    auto b = std::begin(c);
    auto e = std::end(c);
    if(b != e) {
        for(; ;) {
            print(os, *b, fmt);
            if(++b != e) {
                fmt.separator(os, c);
            } else {
                break; //break the loop.
            }
        }
    }

    fmt.suffix(os, fmt);
}

template <typename C, typename Formatter>
void print_container_helper(std::ostream os, C const& c, std::false_type, Formatter const& fmt) {
    fmt.element(os);
}

template <typename C, typename Formatter = Default_formatter>
void print_line(std::ostream& os, C const& c, Formatter const& fmt = Formatter()) {
    print(os, c, fmt);
    os << std::endl;
}

struct Special_formatter {
    template <typename T>
    void prefix(std::ostream& os, T const& t) const {
        Default_formatter().prefix(os, t);
    }
    template <typename T>
    void separator(std::ostream& os, T const& t) const {
        Default_formatter().separator(os, t);
    }
    template <typename T>
    void suffix(std::ostream& os, T const& t) const {
        Default_formatter().suffix(os, t);
    }
    template <typename T>
    void element(std::ostream& os, T const& t) const {
        Default_formatter().element(os, t);
    }
    template <typename K, typename C, typename A>
    void prefix(std::ostream& os, std::set<K,C,A>& s) const {
        os << "[ " << s.size() << " ]{ ";
    }

    template <typename T, typename A>
    void prefix(std::ostream& os, std::forward_list<T,A>&) const {
        os << "< ";
    }
    template <typename T, typename A>
    void separator(std::ostream& os, std::forward_list<T,A>&) const {
        os << " -> ";
    }
    template <typename T, typename A>
    void suffix(std::ostream& os, std::forward_list<T,A>&) const {
        os << "  >";
    }

    template <typename Ch, typename Tr, typename A>
    void element(std::ostream& os, std::basic_string<Ch,Tr,A>& s) const {
        os << s;
    }
};

int main() {

    std::cout << "Empty Vector ";
    print_line(std::cout, std::vector<int>());

}

