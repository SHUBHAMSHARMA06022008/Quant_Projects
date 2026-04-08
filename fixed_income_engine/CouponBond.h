#ifndef COUPONBOND_H
#define COUPONBOND_H

#include <cmath>
#include "Bond.h"
#include "YieldCurve.h"

class CouponBond : public Bond {
private:
    double facevalue, couponrate, maturity, frequency;

    double price_from_rate(double r) {
        double coupon = facevalue * couponrate / frequency;
        double pv = 0;
        for (int i = 1; i <= maturity * frequency; i++) {
            double t = (double)i / frequency;
            pv += coupon * std::exp(-r * t);
        }
        return pv + facevalue * std::exp(-r * maturity);
    }

public:
    CouponBond(double frequency, double maturity, double couponrate, double facevalue)
        : frequency(frequency), maturity(maturity), couponrate(couponrate), facevalue(facevalue) {}

    double price(const YieldCurve& curve) override {
        double coupon = facevalue * couponrate / frequency;
        double pv = 0;
        for (int i = 1; i <= maturity * frequency; i++) {
            double t = (double)i / frequency;
            pv += coupon * std::exp(-curve.getrate(t) * t);
        }
        return pv + facevalue * std::exp(-curve.getrate(maturity) * maturity);
    }

    double IRR(double marketPrice) {
        double low = 0.0, high = 1.0, mid;
        for (int i = 0; i < 100; i++) {
            mid = (low + high) / 2.0;
            price_from_rate(mid) > marketPrice ? low = mid : high = mid;
        }
        return mid;
    }

    double macaulay(const YieldCurve& curve) {
        double coupon = facevalue * couponrate / frequency;
        double weighted_sum = 0;
        for (int i = 1; i <= maturity * frequency; i++) {
            double t = (double)i / frequency;
            weighted_sum += t * coupon * std::exp(-curve.getrate(t) * t);
        }
        weighted_sum += maturity * facevalue * std::exp(-curve.getrate(maturity) * maturity);
        return weighted_sum / price(curve);
    }

    double modified_duration(const YieldCurve& curve) override {
        return macaulay(curve);
    }

    double convexity(const YieldCurve& curve, double dy = 0.0001) override {
        double r = IRR(price(curve));
        double p0 = price_from_rate(r);
        return (price_from_rate(r + dy) + price_from_rate(r - dy) - 2 * p0) / (p0 * dy * dy);
    }

    double effective_duration(double marketPrice, double dy = 0.0001) {
        double r = IRR(marketPrice);
        double p0 = price_from_rate(r);
        return (price_from_rate(r - dy) - price_from_rate(r + dy)) / (2 * p0 * dy);
    }

    double dv01(double marketPrice) {
        double r = IRR(marketPrice);
        return std::abs(price_from_rate(r + 0.0001) - price_from_rate(r));
    }
};

#endif