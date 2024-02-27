A basic kjell written in C.

# Implemented

+ Cool prompt
+ Fork/wait
+ Cd
+ Exit code negation (!)
+ Sequential lists
+ Or lists
+ And lists
+ Exec

## Next

+ Subshell
+ and much more

# Run
+ `nix run "github:sebastianselander/kjell"`
+ `make run` with all dependencies installed

# Development

`nix develop` to enter a development shell with all dependencies, then running
the kjell should only require `make run`

The following are the dependencies if access to nix flakes is not possible.
+ C compiler
+ Make
+ BNFC
+ Bison
+ Flex
