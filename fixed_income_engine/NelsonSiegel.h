#ifndef NELSONSIEGEL_H
#define NELSONSIEGEL_H

#include <map>
#include <cmath>
#include <iostream>

class NelsonSiegel {
private:
    double beta0, beta1, beta2, tau;

    double rate(double t) const {
        double x = t / tau;
        double factor = (1 - std::exp(-x)) / x;
        return beta0 + beta1 * factor + beta2 * (factor - std::exp(-x));
    }

    double loss_with(const std::map<double, double>& obs, double b0, double b1, double b2, double t) const {
        double total = 0;
        for (auto& p : obs) {
            double x = p.first / t;
            double factor = (1 - std::exp(-x)) / x;
            double r = b0 + b1 * factor + b2 * (factor - std::exp(-x));
            double err = r - p.second;
            total += err * err;
        }
        return total;
    }

    double loss(const std::map<double, double>& obs) const { return loss_with(obs, beta0, beta1, beta2, tau); }

public:
    NelsonSiegel() : beta0(0.05), beta1(-0.02), beta2(0.01), tau(1.5) {}

    void fit(const std::map<double, double>& observed, double lr = 0.001, int iterations = 10000) {
        double h = 1e-5;
        for (int i = 0; i < iterations; i++) {
            double l0 = loss(observed);
            double gb0 = (loss_with(observed, beta0+h, beta1, beta2, tau) - l0) / h;
            double gb1 = (loss_with(observed, beta0, beta1+h, beta2, tau) - l0) / h;
            double gb2 = (loss_with(observed, beta0, beta1, beta2+h, tau) - l0) / h;
            double gt  = (loss_with(observed, beta0, beta1, beta2, tau+h) - l0) / h;
            beta0 -= lr * gb0;
            beta1 -= lr * gb1;
            beta2 -= lr * gb2;
            tau   -= lr * gt;
        }
    }

    double getRate(double t) const { return rate(t); }

    void printParams() const {
        std::cout << "b0: " << beta0 << " b1: " << beta1 << " b2: " << beta2 << " tau: " << tau << std::endl;
    }
};

#endif