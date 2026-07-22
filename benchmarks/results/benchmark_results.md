# CacheProBenchmark Results

## Element Access

| Test | Iteration | CachePro | stdLRU | Δ |
|---|---|---|---|---|
| Get() Hit | 10K | 48.92 us | 119.15 us | +143.6% |
| Get() Hit | 100K | 541.00 us | 1.24 ms | +129.0% |
| Get() Hit | 1M | 4.89 ms | 11.98 ms | +145.2% |
| Peek() Hit | 10K | 48.92 us | 43.62 us | -10.8% |
| Peek() Hit | 100K | 378.54 us | 486.77 us | +28.6% |
| Peek() Hit | 1M | 4.92 ms | 4.42 ms | -10.3% |

## Iteration

| Test | Iteration | CachePro | stdLRU | Δ |
|---|---|---|---|---|
| Keys() Traversal | 10K | 3.84 ms | 3.80 ms | -1.0% |
| Keys() Traversal | 100K | 38.59 ms | 38.15 ms | -1.1% |
| Keys() Traversal | 1M | 383.14 ms | 432.45 ms | +12.9% |

## Search

| Test | Iteration | CachePro | stdLRU | Δ |
|---|---|---|---|---|
| Contains() Miss | 10K | 32.85 us | 132.92 us | +304.7% |
| Contains() Miss | 100K | 324.54 us | 1.32 ms | +308.2% |
| Contains() Miss | 1M | 3.81 ms | 13.28 ms | +248.1% |
| Get() Miss | 10K | 32.85 us | 187.38 us | +470.5% |
| Get() Miss | 100K | 324.62 us | 1.51 ms | +366.3% |
| Get() Miss | 1M | 3.27 ms | 15.19 ms | +363.9% |

## Emplace

| Test | Iteration | CachePro | stdLRU | Δ |
|---|---|---|---|---|
| Emplace() Insert | 10K | 2.75 ms | 3.36 ms | +22.1% |
| Emplace() Insert | 100K | 30.47 ms | 50.91 ms | +67.1% |
| Emplace() Insert | 1M | 273.25 ms | 588.93 ms | +115.5% |

## Erase

| Test | Iteration | CachePro | stdLRU | Δ |
|---|---|---|---|---|
| Erase() Existing | 10K | 8.87 ms | 7.43 ms | -16.2% |
| Erase() Existing | 100K | 71.26 ms | 100.17 ms | +40.6% |
| Erase() Existing | 1M | 353.47 ms | 423.01 ms | +19.7% |

## Insert

| Test | Iteration | CachePro | stdLRU | Δ |
|---|---|---|---|---|
| Put() Insert | 10K | 3.22 ms | 5.64 ms | +75.1% |
| Put() Insert | 100K | 41.75 ms | 61.07 ms | +46.3% |
| Put() Insert | 1M | 337.22 ms | 580.12 ms | +72.0% |

## Pop Clear

| Test | Iteration | CachePro | stdLRU | Δ |
|---|---|---|---|---|
| Clear() + Refill | 10K | 31.40 ms | 362.81 ms | +1055.4% |
| Clear() + Refill | 100K | 313.88 ms | 3.27 s | +941.8% |
| Clear() + Refill | 1M | 5.27 s | 56.67 s | +975.5% |

## Push Back

| Test | Iteration | CachePro | stdLRU | Δ |
|---|---|---|---|---|
| Put() Insert (evicting) | 10K | 1.25 ms | 7.90 ms | +531.6% |
| Put() Insert (evicting) | 100K | 12.44 ms | 75.71 ms | +508.7% |
| Put() Insert (evicting) | 1M | 123.00 ms | 772.22 ms | +527.8% |

## Construction

| Test | Iteration | CachePro | stdLRU | Δ |
|---|---|---|---|---|
| Construct Empty | 10K | 156.04 ms | 111.08 us | -99.9% |
| Construct Empty | 100K | 1.32 s | 1.04 ms | -99.9% |
| Construct Empty | 1M | 6.01 s | 11.21 ms | -99.8% |

## Move

| Test | Iteration | CachePro | stdLRU | Δ |
|---|---|---|---|---|
| Move-assign | 10K | 637.85 us | 497.00 us | -22.1% |
| Move-assign | 100K | 6.43 ms | 4.97 ms | -22.7% |
| Move-assign | 1M | 65.34 ms | 50.05 ms | -23.4% |
| Move-construct | 10K | 75.49 ms | 744.30 ms | +885.9% |
| Move-construct | 100K | 757.21 ms | 5.04 s | +565.2% |
| Move-construct | 1M | 4.10 s | 69.51 s | +1596.7% |

## Reallocation

| Test | Iteration | CachePro | stdLRU | Δ |
|---|---|---|---|---|
| Resize() Grow | 10K | 6.22 ms | 12.83 ms | +106.4% |
| Resize() Grow | 100K | 47.07 ms | 1.14 s | +2316.8% |
| Resize() Grow | 1M | 345.12 ms | 426.00 s | +123333.8% |

## Reserve

| Test | Iteration | CachePro | stdLRU | Δ |
|---|---|---|---|---|
| Reserve() | 10K | 61.23 us | 997.54 us | +1529.1% |
| Reserve() | 100K | 622.38 us | 9.07 ms | +1357.7% |
| Reserve() | 1M | 6.24 ms | 88.59 ms | +1319.4% |

## Shrink To Fit

| Test | Iteration | CachePro | stdLRU | Δ |
|---|---|---|---|---|
| Shrink To Fit() | 10K | 62.46 ms | 18.69 ms | -70.1% |
| Shrink To Fit() | 100K | 602.51 ms | 139.80 ms | -76.8% |
| Shrink To Fit() | 1M | 9.37 s | 3.00 s | -68.0% |

## Observer

| Test | Iteration | CachePro | stdLRU | Δ |
|---|---|---|---|---|
| HitCount() | 10K | 20.23 us |
| HitCount() | 100K | 133.54 us |
| HitCount() | 1M | 2.00 ms |
| MissCount() | 10K | 13.46 us |
| MissCount() | 100K | 200.31 us |
| MissCount() | 1M | 2.05 ms |
| HitRate() | 10K | 153.54 us |
| HitRate() | 100K | 1.53 ms |
| HitRate() | 1M | 15.50 ms |
| MostRecentKey() | 10K | 33.54 us |
| MostRecentKey() | 100K | 390.69 us |
| MostRecentKey() | 1M | 3.33 ms |
| LeastRecentKey() | 10K | 33.54 us |
| LeastRecentKey() | 100K | 333.54 us |
| LeastRecentKey() | 1M | 3.38 ms |

## State

| Test | Iteration | CachePro | stdLRU | Δ |
|---|---|---|---|---|
| Size() | 10K | 20.23 us | 20.31 us | +0.4% |
| Size() | 100K | 133.54 us | 200.23 us | +49.9% |
| Size() | 1M | 1.33 ms | 2.06 ms | +54.5% |
| Empty() | 10K | 27.08 us | 27.00 us | -0.3% |
| Empty() | 100K | 267.00 us | 266.77 us | -0.1% |
| Empty() | 1M | 2.77 ms | 2.71 ms | -2.1% |
| Capacity() | 10K | 20.08 us | 20.00 us | -0.4% |
| Capacity() | 100K | 200.23 us | 200.23 us | +0.0% |
| Capacity() | 1M | 2.08 ms | 2.00 ms | -3.8% |
