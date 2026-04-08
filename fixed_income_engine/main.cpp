#include "YieldCurve.h"
#include "ZeroCouponBond.h"
#include "CouponBond.h"
#include "Liability.h"
#include "Portfolio.h"
#include "NelsonSiegel.h"
#include <iostream>
using namespace std;

int main() {
    try {
        // ── Yield Curve ──────────────────────────────
        map<double, double> marketPrices;
        double faceValue = 1000;
        for (double t = 0.5; t <= 5.0; t += 0.5) {
            double rate = 0.03 + 0.004 * t - 0.0003 * t * t;
            marketPrices[t] = faceValue * exp(-rate * t);
        }
        YieldCurve curve;
        curve.bootstrap(marketPrices, faceValue);
        cout << "=== YIELD CURVE ===\n";
        for (auto& p : marketPrices)
            cout << "t=" << p.first << " | r=" << curve.getrate(p.first) << "\n";

        // ── Interpolation ────────────────────────────
        cout << "\n=== INTERPOLATION ===\n";
        cout << "Rate at t=1.75: " << curve.getrate(1.75) << "\n";

        // ── Zero Coupon Bond ─────────────────────────
        cout << "\n=== ZERO COUPON BOND ===\n";
        ZeroCouponBond zcb(1000, 2.0);
        cout << "Price:              " << zcb.price(curve) << "\n";
        cout << "IRR (mkt=920):      " << zcb.IRR(920) << "\n";
        cout << "Modified Duration:  " << zcb.modified_duration(curve) << "\n";
        cout << "Effective Duration: " << zcb.effective_duration(curve) << "\n";
        cout << "Convexity:          " << zcb.convexity(curve) << "\n";
        cout << "DV01:               " << zcb.dv01(curve) << "\n";

        // ── Coupon Bond ──────────────────────────────
        cout << "\n=== COUPON BOND ===\n";
        CouponBond cb(2, 3.0, 0.05, 1000); // semi-annual, 3yr, 5% coupon
        double mktPrice = 980;
        cout << "Price:              " << cb.price(curve) << "\n";
        cout << "IRR (mkt=980):      " << cb.IRR(mktPrice) << "\n";
        cout << "Macaulay Duration:  " << cb.macaulay(curve) << "\n";
        cout << "Effective Duration: " << cb.effective_duration(mktPrice) << "\n";
        cout << "Convexity:          " << cb.convexity(mktPrice) << "\n";
        cout << "DV01:               " << cb.dv01(mktPrice) << "\n";

        // ── Nelson-Siegel ────────────────────────────
        cout << "\n=== NELSON-SIEGEL ===\n";
        map<double, double> observedRates;
        for (auto& p : marketPrices)
            observedRates[p.first] = curve.getrate(p.first);
        NelsonSiegel ns;
        ns.fit(observedRates);
        ns.printParams();
        cout << "NS rate at t=1.75: " << ns.getRate(1.75) << "\n";
        cout << "Linear interp t=1.75: " << curve.getrate(1.75) << "\n";

        // ── Redington ────────────────────────────────
        cout << "\n=== REDINGTON ===\n";
        vector<ZeroCouponBond> assets = {
            ZeroCouponBond(1000, 1.0),
            ZeroCouponBond(1000, 3.0),
            ZeroCouponBond(1000, 5.0)
        };
        vector<Liability> liabilities = {
            Liability(950, 1.0),
            Liability(950, 3.0),
            Liability(950, 5.0)
        };
        Portfolio portfolio(assets, liabilities);
        cout << (portfolio.checkRedington(curve) ? "SATISFIED" : "NOT SATISFIED") << "\n";

    } catch (const exception& e) {
        cout << "Error: " << e.what() << "\n";
    }
    return 0;
}