# Quant Projects

## Projects

- **[india_term_spread](india_term_spread/)** — Does India's 10Y-vs-3M term spread lead
  manufacturing-growth slowdowns and RBI repo cuts? Logit + Markov regime-switching,
  with expanding-window pseudo-out-of-sample evaluation. See its README for setup and
  full limitations.
- **[fixed_income_engine](fixed_income_engine/)** — C++ library for bond pricing and
  risk: yield curve bootstrapping, Nelson-Siegel fitting, duration/convexity, Redington
  immunization.
- **[order_book](order_book/)** — Limit order book engine (C++, compiled to WASM) with
  an HFT-style visualization frontend.
- **portfolio_optimizer.ipynb** — Portfolio optimization notebook.

## india_term_spread — limitations

- **Not a real-time vintage backtest.** FRED provides revised historical observations rather than the exact information set that was available to market participants on each release date.

- **One-month predictor lag is a proxy, not exact release timing.** Exact publication-date alignment is not uniformly available for all of the series used, so a one-month lag is used as a conservative information-set safeguard.

- **RBI event history is hand-curated.** Policy-change dates are compiled from MPC resolutions and have not been independently cross-validated against a second source within the notebook. An incorrect date could therefore affect multiple overlapping six-month target windows.

- **Manufacturing production YoY is a growth proxy.** It is not India's headline IIP or GDP measure, so the growth result should be interpreted specifically as a manufacturing-production result.

- **CPI series is archived through March 2025.** The OECD-sourced monthly FRED CPI series is discontinued, so the common analytical sample ends there rather than being artificially extended.

- **Slowdown threshold is calibrated once.** The bottom-tercile threshold is estimated using data through December 2018 and then held fixed. This avoids full-sample leakage but assumes the distribution of weak-growth episodes is sufficiently stable across subsequent periods, including COVID and the 2022 tightening cycle.

- **Overlapping forward windows reduce effective sample size.** HAC standard errors address part of the resulting serial dependence, but the effective number of independent economic episodes is smaller than the number of monthly observations. Pseudo-\(R^2\) and AUC statistics should therefore be interpreted cautiously.

- **In-sample regressions are for structural interpretation, not forecasting claims.** The expanding-window pseudo-out-of-sample evaluation is the relevant forecasting test, but it remains a historical exercise based on revised data rather than a true real-time vintage backtest.

- **Regime-switching results are exploratory.** The full-sample regime estimates are descriptive. The expanding-window low-regime probability is evaluated against future outcomes as a consistency check, not as a standalone forecasting model.

- **Indian G-Sec market structure is not explicitly modeled.** SLR-driven captive demand, RBI open-market operations, and capital-account restrictions can affect the yield curve independently of macroeconomic expectations and are not separately identified in this study.
