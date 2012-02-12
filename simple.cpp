#include <iostream>
#include <boost/optional.hpp>

#include "cccallcc_fork.hpp"

int f(cont<int> k) {
    std::cout << "f called" << std::endl;
    k(1);
    std::cout << "k called" << std::endl;
    return 0;
}

boost::optional< cont<int> > global_k;

int global_int = 5;

int g(cont<int> k) {
    std::cout << "g called" << std::endl;
    global_k = k;
    global_int++;
    return 0;
}

int main() {
    std::cout << "f returns " << call_cc<int>(f) << std::endl;

    std::cout << "g returns " << call_cc<int>(g) << std::endl;

    std::cout << "global_int = " << global_int << std::endl;

    if (global_k)
        (*global_k)(1);
}
