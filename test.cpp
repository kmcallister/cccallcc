#include <iostream>
#include <boost/optional.hpp>

#include "cccallcc.hpp"

int f(cont<int> k) {
    k(1);
    return 0;
}

boost::optional< cont<int> > global_k;

int g(cont<int> k) {
    global_k = k;
    return 0;
}

int main() {
    using namespace std;

    cout << "f => " << call_cc<int>(f) << endl;

    cout << "g => " << call_cc<int>(g) << endl;
    if (global_k)
        (*global_k)(1);
}
