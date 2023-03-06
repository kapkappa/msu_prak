# OpenMP scaling problem: numerous creating/destroying threads

The reason: extra resources needed for creating and destroying threads

Example: multi-level for loop, with paralleling the lowest cycle

Optimization: creating threads in the high domains
