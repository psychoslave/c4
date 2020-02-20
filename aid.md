# Exlpanation

# Operation emmission

[Clariffication provided by maffydub](https://news.ycombinator.com/item?id=8561499)

>It's taking an integer (representing an operation) and printing out the name of that operation.
>
>First thing to say is that "* ++le" is the integer representing the operation to perform. This basically walks through the array of instructions returning each integer in turn.
>
>Starting at the beginning of the line, we have "printf" with a format string of "%8.4s". This means print out the first 4 characters of the string that I pass next (padded to 8 characters). There then follows a string containing all of the operation names, in numerical order, padded to 4 characters and separated by commas (so the start of each is 5 apart). Finally, we do a lookup into this string (treating it as an array) at offset "* ++le * 5", i.e. the integer representing the operation multipled by 5 (5 being the number of characters between the start of each operation name). Doing this lookup gives us a char, but actually we wanted the pointer to this char (as we want printf to print out this char and the following 3 chars), so we take the address of this char (the & at the beginning of the whole expression).


# Glossary

- recursive descent parser (RDP)
- Look-Ahead Left-to-right Rightmost (LALR): parsing in an anticipative streamwise climactic(/lag) manner, in a [teleo](https://en.wiktionary.org/wiki/teleo-)[cataphoric](https://en.wikipedia.org/wiki/Cataphora) way
- virtual machine (VM)

# Thanks

- Robert Swierczek, for the C4 project
- [userbinator](https://news.ycombinator.com/item?id=8560974), for its comment suggestions
- ackalker, coolreader18 cremno, flohofwoe, jacobvosmaer, keanzu, kschults, methyl, nichochar for [https://news.ycombinator.com/item?id=22354283](discussions on the MacOS issues)

# References

- [C4: C in Four Functions (2014) | Hacker News](https://news.ycombinator.com/item?id=22353532)
- [C4 – C in 4 functions | Hacker News](https://news.ycombinator.com/item?id=8558822)
- [“Compiler Construction” by Niklaus Wirth (2014) [pdf] | Hacker News](https://news.ycombinator.com/item?id=10764672)
- [The Architecture of Open Source Applications](http://aosabook.org/en/index.html)
- [Block Started by Symbol - Wikipedia](https://en.wikipedia.org/wiki/.bss) (BSS)
- [Lexer hack - Wikipedia](https://en.wikipedia.org/wiki/Lexer_hack)
- [Precedence climbing method](https://en.wikipedia.org/wiki/Operator-precedence_parser#Precedence_climbing_method)
- [C99 Lex/Flex & YACC/Bison Grammars](https://gist.github.com/codebrainz/2933703)
- [vsedach/Vacietis: C to Common Lisp compiler](https://github.com/vsedach/Vacietis)
- other tiny C compilers:
  - [swieros](https://github.com/rswier/swieros), a tiny and fast Unix-ish kernel, compiler, and userland by Robert Swierczek
  - [OTCC : Obfuscated Tiny C Compiler](https://bellard.org/otcc/)
  - [A C subset parser, code generator, and virtual machine in about 300 lines of C](http://www.iro.umontreal.ca/~felipe/IFT2030-Automne2002/Complements/tinyc.c) by Marc Feeley
  - [TCC](https://bellard.org/tcc/)
- [low line count, high quality](https://www.reddit.com/r/tinycode/)
