#ifndef LIABILITY_H
#define LIABILITY_H

#include <cmath>
#include "YieldCurve.h"

class Liability {
private:
    double amount, time_due;
public:
    Liability(double amount, double time_due) : amount(amount), time_due(time_due) {}

    double pv(const YieldCurve& curve) { return amount * std::exp(-curve.getrate(time_due) * time_due); }

    double modified_duration(const YieldCurve& curve) { return time_due; }

    double convexity(const YieldCurve& curve, double dy = 0.0001) {
        double r = curve.getrate(time_due);
        double p0 = pv(curve);
        double p_plus = amount * std::exp(-(r + dy) * time_due);
        double p_minus = amount * std::exp(-(r - dy) * time_due);
        return (p_minus + p_plus - 2 * p0) / (p0 * dy * dy);
    }
};

#endif