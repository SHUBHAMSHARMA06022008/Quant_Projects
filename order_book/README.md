# Order Book Simulator

A compact limit order book project with:

- a C++20 matching engine in `orderbook.cpp`
- an Emscripten-generated WebAssembly build in `orderbook.js` + `orderbook.wasm`
- a browser-based HFT-style visualization in `orderbook_hft.html`

The project can be used in two ways:

1. Run the native C++ program to exercise the engine from `main()`
2. Open the HTML dashboard to simulate a live order book, trade tape, and agent activity in the browser

## What It Does

The engine maintains separate bid and ask books, keyed by price and preserving FIFO order within each price level. It supports:

- `GTC` orders
- `FnK` orders
- buy and sell sides
- order cancellation
- order modification
- trade generation when the best bid crosses the best ask
- aggregated level snapshots for bids and asks

At a high level:

- bids are stored in descending price order
- asks are stored in ascending price order
- matching happens while best bid `>=` best ask
- fills are partial when necessary
- filled orders are removed from the book
- unmatched `FnK` orders are discarded instead of resting

## Project Files

- `orderbook.cpp`: core order book engine, order types, matching logic, and Emscripten exports
- `orderbook_hft.html`: browser UI and market simulation loop
- `orderbook.js`: generated JavaScript glue code for the WASM build
- `orderbook.wasm`: compiled WebAssembly module
- `.vscode/c_cpp_properties.json`: local VS Code C++ configuration

## Native C++ Run

Build with any C++20-capable compiler.

Example with `g++`:

```bash
g++ -std=c++20 -O2 orderbook.cpp -o orderbook
./orderbook
```

On Windows with MinGW:

```powershell
g++ -std=c++20 -O2 orderbook.cpp -o orderbook.exe
.\orderbook.exe
```

The current `main()` inserts a few sample orders and prints:

- number of trades created
- number of orders still resting in the book

## WebAssembly Build

The HTML file expects `orderbook.js` and `orderbook.wasm` in the same folder.

To rebuild them with Emscripten:

```bash
em++ -O3 -std=c++20 orderbook.cpp -s WASM=1 -s MODULARIZE=1 -s EXPORT_NAME="OrderbookModule" -s EXPORTED_FUNCTIONS=[_ob_create,_ob_destroy,_ob_add_order,_ob_cancel,_ob_size,_ob_get_levels,_malloc,_free] -s EXPORTED_RUNTIME_METHODS=[ccall,cwrap,UTF8ToString] -s ALLOW_MEMORY_GROWTH=1 -o orderbook.js
```

The exported C API includes:

- `ob_create`
- `ob_destroy`
- `ob_add_order`
- `ob_cancel`
- `ob_size`
- `ob_get_levels`

## Running the Browser Demo

Serve the folder over HTTP so the browser can load the WASM assets:

```bash
python -m http.server 8080
```

Then open:

```text
http://localhost:8080/orderbook_hft.html
```

The UI includes:

- live bid/ask ladder
- spread and last price metrics
- trade tape
- simulated trading agents
- event stream
- speed controls
- `GTC` / `FnK` mode switching
- pause and reset controls

If the WASM files do not load, the page can fall back to a built-in JavaScript order book implementation.

## Matching Model

The C++ engine uses:

- `std::map` for price levels
- `std::list` for time-priority queues at each level
- `std::unordered_map` for direct lookup by order id
- `std::shared_ptr` to store active orders

This gives:

- ordered best-price access
- stable iterators for queued orders
- efficient cancellation by id
- straightforward aggregation into level summaries

## Notes

- Prices are stored as integer ticks, so the UI formats values like `10000` as `$100.00`
- `FnK` appears to be used here as an immediate-match-only order type
- `orderbook.js` and `orderbook.wasm` are generated artifacts and can be rebuilt from `orderbook.cpp`

## Future Improvements

- add unit tests for matching, cancel, and modify paths
- record full trade details across the WASM bridge instead of returning only a trade count
- add explicit command-line examples for submitting orders
- clean up duplicate includes and minor naming inconsistencies in the C++ source
