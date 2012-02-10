#include <iostream>
#include <boost/optional.hpp>

#include "cccallcc.hpp"

int f(cont<int> k) {
    std::cout << "f called" << std::endl;
    k(1);
    return 0;
}

boost::optional< cont<int> > global_k;

int g(cont<int> k) {
    std::cout << "g called" << std::endl;
    global_k = k;
    return 0;
}

int main() {
    std::cout << "f returns " << call_cc<int>(f) << std::endl;

    std::cout << "g returns " << call_cc<int>(g) << std::endl;
    if (global_k)
        (*global_k)(1);
}
