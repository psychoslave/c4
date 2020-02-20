# c4 - C in four functions

An adapation of [Robert Swierczek's exercise in minimalism](https://github.com/rswier/c4) aiming at:
- using a more informative terminology;
- documenting the less obvious parts (open an issue if something is not).

On most Nix boxes, this should do the trick to compile the project and then
C-subset code with the c4 executable thereafter, including c4.c itself:

    gcc -o c4 c4.c
    ./c4 hello.c
    ./c4 -s hello.c

    ./c4 c4.c hello.c
    ./c4 c4.c c4.c hello.c

# Known issues

For mac users, to avoid the reject of a `main` function which is not type `int`:

  gcc -Wno-all -arch i386 -o c4 c4.c

Alternatively, one should be able to compile it using clang

  clang -m32 c4.c

If it still doesn't work, there might be more chances to make the project run
with [kbrock's fork](https://github.com/kbrock/c4), checking out the 'longs' branch.

# More help

Look at this [repository aid](./help.md). If you don't find what you are looking
for, open an issue.
