# Fixed Income Analytics Engine

A C++ library for pricing and risk analysis of fixed income instruments, built from first principles. Covers zero coupon bonds, coupon bonds, yield curve bootstrapping, Nelson-Siegel curve fitting, and Redington immunization.

---

## What it does

- **Yield Curve** — bootstraps spot rates from market prices of zero coupon bonds using continuous compounding. Supports linear interpolation for arbitrary maturities.

- **Zero Coupon Bond** — prices bonds using the full term structure, computes IRR, Macaulay duration, modified duration, effective duration, convexity, and DV01.

- **Coupon Bond** — prices using per-cashflow spot rates from the curve. Solves for IRR via bisection. Computes Macaulay duration as a PV-weighted average of cashflow times, plus effective duration and convexity.

- **Nelson-Siegel Model** — fits a smooth parametric curve to observed spot rates using gradient descent. Captures level, slope, and curvature of the yield curve.

- **Redington Immunization** — checks whether a portfolio of bonds is immunized against interest rate risk relative to a liability stream: PV match, duration match, and convexity condition.

---

## Project structure

```
fixed_income_engine/
├── YieldCurve.h        # Bootstrapping + interpolation
├── ZeroCouponBond.h    # ZCB pricing and risk metrics
├── CouponBond.h        # Coupon bond pricing and risk metrics
├── Liability.h         # Liability PV, duration, convexity
├── Portfolio.h         # Redington immunization checker
├── NelsonSiegel.h      # Parametric curve fitting
└── main.cpp            # Example usage
```

---

## Compile and run

```bash
g++ main.cpp -o engine.exe
./engine.exe
```

Requires a C++17 compiler.

---

## Concepts implemented

- Continuous compounding throughout
- Term structure pricing (each cashflow discounted at its own spot rate)
- Bisection method for YTM/IRR on coupon bonds
- Numerical differentiation for effective duration and convexity
- Gradient descent with numerical gradients for Nelson-Siegel calibration
- PV-weighted portfolio aggregation for Redington conditions

---

## Background

Built as a learning project while studying actuarial science (CM1) and Hull's Options, Futures and Other Derivatives. The goal was to implement fixed income analytics from scratch in C++ rather than relying on libraries — understanding every formula at the code level.