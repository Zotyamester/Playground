#ifndef LILUTILS_HPP
#define LILUTILS_HPP

#include <cfloat>

inline double reciprocal(double x)
try {
    return 1.0 / x;
} catch(...) {
    return DBL_MAX;
}

#endif // LILUTILS_HPP
