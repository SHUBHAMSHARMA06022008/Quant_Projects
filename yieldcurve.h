#ifndef YIELDCURVE_H
#define YIELDCURVE_H

#include <map>
#include <cmath>
#include <stdexcept>

class YieldCurve {
private:
    std::map<double, double> data;
public:
    double getrate(double maturity) const {
        if (data.empty()) throw std::out_of_range("Curve is empty");
        if (maturity < data.begin()->first) throw std::out_of_range("Maturity below curve range");
        if (maturity > data.rbegin()->first) throw std::out_of_range("Maturity above curve range");
        auto it = data.find(maturity);
        if (it != data.end()) return it->second;
        auto upper = data.upper_bound(maturity);
        auto lower = std::prev(upper);
        return lower->second + (upper->second - lower->second) * (maturity - lower->first) / (upper->first - lower->first);
    }

    void bootstrap(std::map<double, double> marketPrices, double faceValue) {
        for (auto& val : marketPrices)
            data[val.first] = std::log(faceValue / val.second) / val.first;
    }
};

#endif