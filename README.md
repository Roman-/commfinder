# commfinder
## description
This tool finds all possible center-safe commutator algorithms for 5x5 cube in form of [A, B] where A is 1 move long and B is up to N moves long (N is specified in command line args). Commutators that don't lead to any interesting cube states are eliminated.

"Interesting" cases include: 3cycles, 2-2-swaps and 5cycles for all 5 elements of 5x5 cube, and also corner twists and edge flips.

## results
Results has been already generated by me and saved to https://github.com/Roman-/comms. Note that in comms repo there is one particular redundancy type eliminated that is still persist in this project. To illustrate this particular redundancy I give you the example of the alg it included. Consider this: `[L, U2 M U R U]` for FUL-UBL-RFD corner 3cycle — the M move doesn't play any role here.

## build
```bash
mkdir build
cd build
cmake ..
make
```

## usage
```
./commfinder output_path max_partb_moves
```
Example:
```
./commfinder /tmp/comms.txt 5
```
will find all comms [A, B] where part B is 1-5 moves long and save everything in /tmp/comms.txt.