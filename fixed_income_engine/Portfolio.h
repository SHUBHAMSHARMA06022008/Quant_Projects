#ifndef PORTFOLIO_H
#define PORTFOLIO_H

#include <vector>
#include <cmath>
#include "Bond.h"
#include "Liability.h"
#include "YieldCurve.h"

class Portfolio {
private:
    std::vector<Bond*> assets;
    std::vector<Liability> liabilities;
public:
    Portfolio(std::vector<Bond*> assets, std::vector<Liability> liabilities)
        : assets(assets), liabilities(liabilities) {}

    bool checkRedington(const YieldCurve& curve) {
        double total_asset_pv = 0, total_liability_pv = 0;
        double asset_duration = 0, liability_duration = 0;
        double asset_convexity = 0, liability_convexity = 0;

        for (auto& a : assets) total_asset_pv += a->price(curve);
        for (auto& a : assets) {
            double w = a->price(curve) / total_asset_pv;
            asset_duration += w * a->modified_duration(curve);
            asset_convexity += w * a->convexity(curve);
        }
        for (auto& l : liabilities) total_liability_pv += l.pv(curve);
        for (auto& l : liabilities) {
            double w = l.pv(curve) / total_liability_pv;
            liability_duration += w * l.modified_duration(curve);
            liability_convexity += w * l.convexity(curve);
        }

        return std::abs(total_asset_pv - total_liability_pv) < 1e-6 &&
               std::abs(asset_duration - liability_duration) < 1e-6 &&
               asset_convexity > liability_convexity;
    }
};

#endif