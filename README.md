# RISC-V
RISC-V simulator by UnsignedLemon.
* 5-stage pipeline.
* supporting forwarding and random stage order.
* with a hybrid PC predictor.
## Implementation
### Register part:
* `memoryData` is the memory of the simulator.
* `R[0] ~ R[31]` are the valid registers.
* `B1 ~ B4` are the between-process buffers that store temporary results of function units.
* `PC` is the program counter. There's a complex predictor to determine jump & branch addresses.

**NOTICE: all the register parts are updated on the rise edge of clock. In WB process, it only changes their input value, while their output value is changed on the next cycle.**

### Function units part:
* `IF`: fetch data (data from PC) and do basic operations, such as acquiring `opcode` for prediction usage.
* `ID`: decode instruction (data from B1) , get `cmdType`,`rd/rs1/rs2` registers, try to load register data for B2.
* `EX`: do all the calculation (data from B2), such as compute add results or memory address. Also check PC prediction and determin whether instructions behind `shouldDiscard`
* `ME`: memory access (data from B3). Using 3 cycles to simulate time cost.
* `WB`: write back to registers (data from B4).

**Function units can be regarded as combinational circuits although they all have REG/MEM access. Their values can be updated right after REG/MEM update and actually have NO STORAGE UNITS in them (in my code, the storage buffer in each units represents the output ports to the next REAL buffers.)**

### Structure ###
![Markdown](sample.png)

## Code structure
* `resources.h`,`resources.cpp`,`enum.cpp`: global definations.
* `main.cpp`: major part of simulator, containing process of clock rise,`registers`update,`units`update.
* `reg.h`,`reg.cpp`: registers containing `REGISTER`,`PC`,`BUFFER`.
* `data.h`,`data.cpp`: memory.
* `units.h`, `units.cpp`: function units containing `IF/ID/EX/ME/WB`.
* `predictor.h`, `predictor.cpp`: hybrid predictor.

## Predictor Implementation
A **2-bit counter** *(I call it TBC)* is required, which stores a 2-bit counter for every branch PC.
There's an advanced version, **Local History Predictor** *(I call it LHP)*, which stores many different 2-bit counter for one branch PC in terms of their 'branch history', a 0/1 sequence of certain length representing the last few branches of this PC `taken/not_taken`.
Also there's another version, **Global History Predictor** *(GHP)*, where all the branch PC share a global sequence of `taken/not_taken`.

Here are the variables I used in the predictors above.
### All predictor ###
* `targetAddress[PCTableSize]`: if `taken`, jump to where.
### TBC ###
* `twoBitCounter[]`: 2-bit counter for every PC.
### LHP ###
* `branchHistoryTable[]`: the history sequence of every PC representing `taken/not_taken`.
* `PCTable[PCTableSize][historySize]:` the 2-bit counter for every PC of every history sequence.
### GHP ###
* `globalPattern`: the global history sequence of all branches.
* `PCTable[][]`: same as above. 

I take PCTableSize as (1<<8) and historySize as (1<<9). Thus 9 previous decisions are recorded. Also, in all testcases, the PC address are relatively small so there are almost no aliasing situations.

`GHP` and `LHP` need some cycles to train accuracy. So a `trainCycle[PCTableSize]` array is used to track the branch cycles of every PC. When the cycle is low, we take the `TBC` to do prediction, while there are enough cycles, we use `GHP` or `LHP` determined by another 2-bit counter `predictorChoice[]`.

### Results ###
| Testcase | Accuracy | Hit | Total |
| :----: | :----: | :------------: | :----------: |
|**array_test1**|0.545455|12|22|
|**array_test2**|0.576923|15|26|
|**basicopt1**|0.99456|154295|155139|
|**bulgarian**|0.95822|68506|71493|
|**expr**|0.828829|92|111|
|**gcd**|0.65|78|120|
|**hanoi**|0.980524|17117|17457|
|**lvalue2**|0.666667|4|6|
|**magic**|0.879474|59689|67869|
|**manyarguments**|0.6|6|10|
|**multiarray**|0.888889|144|162|
|**naive**|/|0|0|
|**pi**|0.851553|34024993|39956380|
|**qsort**|0.97707|195458|200045|
|**queens**|0.838594|64669|77116|
|**statement_test**|0.59901|121|202|
|**superloop**|0.985484|428712|435027|
|**tak**|0.791669|48006|60639 |

We can see that the total accuracy of testcases is above 0.8, and the hybrid predictor has a slight advantage over single `LHP` or `TBC`. By the way, the performance of `GHP` is really bad in testcases, for there are no situations of large scale of `if` branches. However, the hybrid predictor still did a good job.
