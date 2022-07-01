# RISC-V
RISC-V simulator by UnsignedLemon.
* 5-stage pipeline.
* supporting forwarding and random stage order.
* with a hybrid PC predictor.
## Implementation
### Register part:
* `memoryData` is the memory of the simulator.
* `R[0] ~ R[31]` is the valid registers.
