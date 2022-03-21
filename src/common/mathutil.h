#pragma once

#import <math.h>

namespace trackman {

static int highestPowerOf2(int n) {
    int p = (int)log2(n);
    return (int)pow(2, p);
}

} // namespace trackman
