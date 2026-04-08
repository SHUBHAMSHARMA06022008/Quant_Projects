#include <iostream>
#include <map>
#include <vector>
#include "YieldCurve.h"
#include "Bond.h"
#include "ZeroCouponBond.h"
#include "CouponBond.h"
#include "Liability.h"
#include "Portfolio.h"
#include "NelsonSiegel.h"

using namespace std;

int main() {
    try {
        cout << "==============================\n";
        cout << "   YIELD CURVE BOOTSTRAP\n";
        cout << "==============================\n";

        map<double, double> marketPrices;
        double faceValue = 1000;
        for (double t = 0.25; t <= 10.0; t += 0.25) {
            double rate = 0.03 + 0.002 * t - 0.00003 * t * t;
            marketPrices[t] = faceValue * exp(-rate * t);
        }

        YieldCurve curve;
        curve.bootstrap(marketPrices, faceValue);

        int count = 0;
        for (auto& p : marketPrices) {
            cout << "t=" << p.first << " | rate=" << curve.getrate(p.first) << "\n";
            if (++count == 8) break;
        }

        cout << "\n==============================\n";
        cout << "   ZERO COUPON BOND TEST\n";
        cout << "==============================\n";

        ZeroCouponBond zcb1(1000, 1);
        ZeroCouponBond zcb2(1000, 3);
        ZeroCouponBond zcb3(1000, 5);

        for (auto* b : {&zcb1, &zcb2, &zcb3}) {
            cout << "\nMaturity:          " << b->macaulay(curve) << "yr\n";
            cout << "Price:             " << b->price(curve) << "\n";
            cout << "IRR (mkt=900):     " << b->IRR(900) << "\n";
            cout << "Modified Duration: " << b->modified_duration(curve) << "\n";
            cout << "Effective Duration:" << b->effective_duration(curve) << "\n";
            cout << "Convexity:         " << b->convexity(curve) << "\n";
            cout << "DV01:              " << b->dv01(curve) << "\n";
        }

        cout << "\n==============================\n";
        cout << "   COUPON BOND TEST\n";
        cout << "==============================\n";

        CouponBond cb1(1, 3, 0.05, 1000);
        CouponBond cb2(2, 5, 0.06, 1000);
        CouponBond cb3(1, 7, 0.04, 1000);

        for (auto* b : {&cb1, &cb2, &cb3}) {
            double mktPrice = 950;
            cout << "\nPrice (curve):     " << b->price(curve) << "\n";
            cout << "IRR (mkt=950):     " << b->IRR(mktPrice) << "\n";
            cout << "Macaulay Duration: " << b->macaulay(curve) << "\n";
            cout << "Modified Duration: " << b->modified_duration(curve) << "\n";
            cout << "Effective Duration:" << b->effective_duration(mktPrice) << "\n";
            cout << "Convexity:         " << b->convexity(curve) << "\n";
            cout << "DV01:              " << b->dv01(mktPrice) << "\n";
        }

        cout << "\n==============================\n";
        cout << "   LIABILITY TEST\n";
        cout << "==============================\n";

        Liability l1(500, 1);
        Liability l2(700, 3);
        Liability l3(800, 5);

        for (auto* l : {&l1, &l2, &l3}) {
            cout << "\nPV:                " << l->pv(curve) << "\n";
            cout << "Modified Duration: " << l->modified_duration(curve) << "\n";
            cout << "Convexity:         " << l->convexity(curve) << "\n";
        }

        cout << "\n==============================\n";
        cout << "   MIXED PORTFOLIO + REDINGTON\n";
        cout << "==============================\n";

        vector<Bond*> assets = {&zcb1, &zcb2, &zcb3, &cb1, &cb2, &cb3};
        vector<Liability> liabilities = {l1, l2, l3};

        Portfolio portfolio(assets, liabilities);
        bool result = portfolio.checkRedington(curve);
        cout << "Redington Immunization: " << (result ? "SATISFIED" : "NOT SATISFIED") << "\n";

        cout << "\n==============================\n";
        cout << "   NELSON-SIEGEL FIT\n";
        cout << "==============================\n";

        map<double, double> observedRates;
        for (auto& p : marketPrices)
            observedRates[p.first] = curve.getrate(p.first);

        NelsonSiegel ns;
        ns.fit(observedRates, 0.001, 5000);
        ns.printParams();

        cout << "\nt=1.5 | Linear: " << curve.getrate(1.5) << " | NS: " << ns.getRate(1.5) << "\n";
        cout << "t=3.7 | Linear: " << curve.getrate(3.7) << " | NS: " << ns.getRate(3.7) << "\n";

    } catch (const exception& e) {
        cout << "Error: " << e.what() << "\n";
    }

    return 0;
}