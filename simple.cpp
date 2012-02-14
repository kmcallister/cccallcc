#include <iostream>
#include <boost/optional.hpp>

#include "cccallcc_fork.hpp"

////////////////////////////////////////////////////
// Simple examples of using fork-based continuations


// Early exit.

int f(cont<int> k) {
    std::cout << "f called" << std::endl;
    k(1);
    std::cout << "k called" << std::endl;
    return 0;
}

void example_f() {
    std::cout << "f returns " << call_cc<int>(f) << std::endl;
}


// Re-entering a saved continuation.

boost::optional< cont<int> > global_k;

int global_int = 5;

int g(cont<int> k) {
    std::cout << "g called" << std::endl;
    global_k = k;
    global_int++;
    return 0;
}

void example_g() {
    std::cout << "g returns " << call_cc<int>(g) << std::endl;

    std::cout << "global_int = " << global_int << std::endl;

    if (global_k)
        (*global_k)(1);
}


// Run both examples.

int main() {
    example_f();
    example_g();
    return 0;
}
