#ifndef BOND_H
#define BOND_H

#include "YieldCurve.h"

class Bond {
public:
    virtual double price(const YieldCurve& curve) = 0;
    virtual double modified_duration(const YieldCurve& curve) = 0;
    virtual double convexity(const YieldCurve& curve, double dy = 0.0001) = 0;
    virtual ~Bond() {}
};

#endif