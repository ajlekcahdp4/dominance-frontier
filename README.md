# Dominance Frontier

## Install ALL the dependencies

Install nix if you still don't have it. See https://nixos.org/download/

```sh
nix develop
# Or (if you use direnv)
direnv allow
```

## How To Run:

```sh
cmake -S . -B build
cmake --build build
build/domfront --help
```

Or build package using Nix:

```sh
nix build .
result/bin/domfront --help
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
