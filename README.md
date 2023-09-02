# Dominance Frontier

# Dependencies
1. LLVM

## How To Run:
```shell
git clone git@github.com:ajlekcahdp4/dominance-frontier.git
cd dominance-frontier
cmake -S . -B build
cd build
cmake --build .
./domfront --help
```

## Options
```shell
domfront options:

  --dump-cfg=<filename>      - Dump CFG to dot file
  --dump-df=<filename>       - Dump DF graph to dot file
  --dump-dj=<filename>       - Dump DJ graph to dot file
  --dump-dom-tree=<filename> - Dump Dom Tree to dot file
  --dump-idf=<filename>      - Dump IDF graph to dot file
  --gen-iterations=<ulong>   - Iteration number for CFG generation
  --generate-cfg             - Forces to generate CFG, not take it as an input
  --print-idom               - Print Immediate Dominators
  --seed=<ulong>             - Random seed
```

## Examples
To explore examples of input CFG and output see [examples](./examples/)