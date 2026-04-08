#ifndef ZEROCOUPONBOND_H
#define ZEROCOUPONBOND_H

#include <cmath>
#include "YieldCurve.h"
#include "Bond.h"

class ZeroCouponBond : public Bond{
private:
    double facevalue;
    double maturity;
public:
    ZeroCouponBond(double facevalue, double maturity) : facevalue(facevalue), maturity(maturity) {}

    double price_from_rate(double r) { return facevalue * std::exp(-r * maturity); }

    double price(const YieldCurve& curve) { return price_from_rate(curve.getrate(maturity)); }

    double IRR(double marketprice) { return std::log(facevalue / marketprice) / maturity; }

    double macaulay(const YieldCurve& curve) { return maturity; }

    double modified_duration(const YieldCurve& curve) { return maturity; }

    double effective_duration(const YieldCurve& curve, double dy = 0.0001) {
        double r = curve.getrate(maturity);
        return (price_from_rate(r - dy) - price_from_rate(r + dy)) / (2 * price(curve) * dy);
    }

    double convexity(const YieldCurve& curve, double dy = 0.0001) {
        double r = curve.getrate(maturity);
        double p0 = price(curve);
        return (price_from_rate(r + dy) + price_from_rate(r - dy) - 2 * p0) / (p0 * dy * dy);
    }

    double dv01(const YieldCurve& curve) {
        double r = curve.getrate(maturity);
        return std::abs(price_from_rate(r + 0.0001) - price_from_rate(r));
    }
};

#endif