// c4.c - C in four functions

// char, int, and pointer types
// if, while, return, and expression statements
// just enough features to allow self-compilation and a bit more

// Written by Robert Swierczek

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <fcntl.h>
#define int long long

char *p, *lp, // current position in source code
     /**
      * Pointer to point to data and Basic Service Set (BSS)
      */
     *data;

int *e, *le,  // current position in emitted code
    /**
     * currently parsed identifier
     */
    *pin,
    /* list of occuring lexies, also known as symbol table, or list of identifiers */
    *lexicon,
    /* current lexie, that is, last string segmented as an atomic lexical chunck of code */
    lexie,
    /* current lexie value, state, or whatever name given to its pat gist */
    nub,
    /* type of the current expression */
    ilk,
    pad,      // local variable offset
    line,     // current line number
    pen,      // print source and assembly flag
    spell;    // print executed instructions

// tokens and classes (operators last and in precedence order)
enum {
  Num = 128, Fun, Sys, Glo, Loc, Id,
  Char, Else, Enum, If, Int, Return, Sizeof, While,
  Assign, Cond, Lor, Lan, Or, Xor, And, Eq, Ne, Lt, Gt, Le, Ge, Shl, Shr, Add, Sub, Mul, Div, Mod, Inc, Dec, Brak
};

// opcodes
enum { LEA ,IMM ,JMP ,JSR ,BZ  ,BNZ ,ENT ,ADJ ,LEV ,LI  ,LC  ,SI  ,SC  ,PSH ,
       OR  ,XOR ,AND ,EQ  ,NE  ,LT  ,GT  ,LE  ,GE  ,SHL ,SHR ,ADD ,SUB ,MUL ,DIV ,MOD ,
       OPEN,READ,CLOS,PRTF,MALC,FREE,MSET,MCMP,EXIT };

// types
enum { CHAR, INT, PTR };

// identifier offsets (since we can't create an ident struct)
enum { Tk, Hash, Name, Class, Type, Val, HClass, HType, HVal, Idsz };

/**
 * Splits the source code into a series of lexies
 */
void next()
{
  char *pp;

  while (lexie = *p) {
    ++p;
    if (lexie == '\n') {
      if (pen) {
        printf("%d: %.*s", line, p - lp, lp);
        lp = p;
        while (le < e) {
          // output the name of an operation, out of its index in an array sting
          // see aid's "Operation emmission" section for more information
          printf("%8.4s", &"LEA ,IMM ,JMP ,JSR ,BZ  ,BNZ ,ENT ,ADJ ,LEV ,LI  ,LC  ,SI  ,SC  ,PSH ,"
                           "OR  ,XOR ,AND ,EQ  ,NE  ,LT  ,GT  ,LE  ,GE  ,SHL ,SHR ,ADD ,SUB ,MUL ,DIV ,MOD ,"
                           "OPEN,READ,CLOS,PRTF,MALC,FREE,MSET,MCMP,EXIT,"[*++le * 5]);
          if (*le <= ADJ) printf(" %d\n", *++le); else printf("\n");
        }
      }
      ++line;
    }
    else if (lexie == '#') {
      while (*p != 0 && *p != '\n') ++p;
    }
    else if ((lexie >= 'a' && lexie <= 'z') || (lexie >= 'A' && lexie <= 'Z') || lexie == '_') {
      pp = p - 1;
      while ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || (*p >= '0' && *p <= '9') || *p == '_')
        lexie = lexie * 147 + *p++;
      lexie = (lexie << 6) + (p - pp);
      pin = lexemes;
      while (pin[Tk]) {
        if (lexie == pin[Hash] && !memcmp((char *)pin[Name], pp, p - pp)) { lexie = pin[Tk]; return; }
        pin = pin + Idsz;
      }
      pin[Name] = (int)pp;
      pin[Hash] = lexie;
      lexie = pin[Tk] = Id;
      return;
    }
    else if (lexie >= '0' && lexie <= '9') {
      if (nub = lexie - '0') { while (*p >= '0' && *p <= '9') nub = nub * 10 + *p++ - '0'; }
      else if (*p == 'x' || *p == 'X') {
        while ((lexie = *++p) && ((lexie >= '0' && lexie <= '9') || (lexie >= 'a' && lexie <= 'f') || (lexie >= 'A' && lexie <= 'F')))
          nub = nub * 16 + (lexie & 15) + (lexie >= 'A' ? 9 : 0);
      }
      else { while (*p >= '0' && *p <= '7') nub = nub * 8 + *p++ - '0'; }
      lexie = Num;
      return;
    }
    else if (lexie == '/') {
      if (*p == '/') {
        ++p;
        while (*p != 0 && *p != '\n') ++p;
      }
      else {
        lexie = Div;
        return;
      }
    }
    else if (lexie == '\'' || lexie == '"') {
      pp = data;
      while (*p != 0 && *p != lexie) {
        if ((nub = *p++) == '\\') {
          if ((nub = *p++) == 'n') nub = '\n';
        }
        if (lexie == '"') *data++ = nub;
      }
      ++p;
      if (lexie == '"') nub = (int)pp; else lexie = Num;
      return;
    }
    else if (lexie == '=') { if (*p == '=') { ++p; lexie = Eq; } else lexie = Assign; return; }
    else if (lexie == '+') { if (*p == '+') { ++p; lexie = Inc; } else lexie = Add; return; }
    else if (lexie == '-') { if (*p == '-') { ++p; lexie = Dec; } else lexie = Sub; return; }
    else if (lexie == '!') { if (*p == '=') { ++p; lexie = Ne; } return; }
    else if (lexie == '<') { if (*p == '=') { ++p; lexie = Le; } else if (*p == '<') { ++p; lexie = Shl; } else lexie = Lt; return; }
    else if (lexie == '>') { if (*p == '=') { ++p; lexie = Ge; } else if (*p == '>') { ++p; lexie = Shr; } else lexie = Gt; return; }
    else if (lexie == '|') { if (*p == '|') { ++p; lexie = Lor; } else lexie = Or; return; }
    else if (lexie == '&') { if (*p == '&') { ++p; lexie = Lan; } else lexie = And; return; }
    else if (lexie == '^') { lexie = Xor; return; }
    else if (lexie == '%') { lexie = Mod; return; }
    else if (lexie == '*') { lexie = Mul; return; }
    else if (lexie == '[') { lexie = Brak; return; }
    else if (lexie == '?') { lexie = Cond; return; }
    else if (lexie == '~' || lexie == ';' || lexie == '{' || lexie == '}' || lexie == '(' || lexie == ')' || lexie == ']' || lexie == ',' || lexie == ':') return;
  }
}

/**
 * Parses expressions
 */
void expr(int lev)
{
  int t, *d;

  if (!lexie) { printf("%d: unexpected eof in expression\n", line); exit(-1); }
  else if (lexie == Num) { *++e = IMM; *++e = nub; next(); ilk = INT; }
  else if (lexie == '"') {
    *++e = IMM; *++e = nub; next();
    while (lexie == '"') next();
    data = (char *)((int)data + sizeof(int) & -sizeof(int)); ilk = PTR;
  }
  else if (lexie == Sizeof) {
    next(); if (lexie == '(') next(); else { printf("%d: open paren expected in sizeof\n", line); exit(-1); }
    ilk = INT; if (lexie == Int) next(); else if (lexie == Char) { next(); ilk = CHAR; }
    while (lexie == Mul) { next(); ilk = ilk + PTR; }
    if (lexie == ')') next(); else { printf("%d: close paren expected in sizeof\n", line); exit(-1); }
    *++e = IMM; *++e = (ilk == CHAR) ? sizeof(char) : sizeof(int);
    ilk = INT;
  }
  else if (lexie == Id) {
    d = pin; next();
    if (lexie == '(') {
      next();
      t = 0;
      while (lexie != ')') { expr(Assign); *++e = PSH; ++t; if (lexie == ',') next(); }
      next();
      if (d[Class] == Sys) *++e = d[Val];
      else if (d[Class] == Fun) { *++e = JSR; *++e = d[Val]; }
      else { printf("%d: bad function call\n", line); exit(-1); }
      if (t) { *++e = ADJ; *++e = t; }
      ilk = d[Type];
    }
    else if (d[Class] == Num) { *++e = IMM; *++e = d[Val]; ilk = INT; }
    else {
      if (d[Class] == Loc) { *++e = LEA; *++e = pad - d[Val]; }
      else if (d[Class] == Glo) { *++e = IMM; *++e = d[Val]; }
      else { printf("%d: undefined variable\n", line); exit(-1); }
      *++e = ((ilk = d[Type]) == CHAR) ? LC : LI;
    }
  }
  else if (lexie == '(') {
    next();
    if (lexie == Int || lexie == Char) {
      t = (lexie == Int) ? INT : CHAR; next();
      while (lexie == Mul) { next(); t = t + PTR; }
      if (lexie == ')') next(); else { printf("%d: bad cast\n", line); exit(-1); }
      expr(Inc);
      ilk = t;
    }
    else {
      expr(Assign);
      if (lexie == ')') next(); else { printf("%d: close paren expected\n", line); exit(-1); }
    }
  }
  else if (lexie == Mul) {
    next(); expr(Inc);
    if (ilk > INT) ilk = ilk - PTR; else { printf("%d: bad dereference\n", line); exit(-1); }
    *++e = (ilk == CHAR) ? LC : LI;
  }
  else if (lexie == And) {
    next(); expr(Inc);
    if (*e == LC || *e == LI) --e; else { printf("%d: bad address-of\n", line); exit(-1); }
    ilk = ilk + PTR;
  }
  else if (lexie == '!') { next(); expr(Inc); *++e = PSH; *++e = IMM; *++e = 0; *++e = EQ; ilk = INT; }
  else if (lexie == '~') { next(); expr(Inc); *++e = PSH; *++e = IMM; *++e = -1; *++e = XOR; ilk = INT; }
  else if (lexie == Add) { next(); expr(Inc); ilk = INT; }
  else if (lexie == Sub) {
    next(); *++e = IMM;
    if (lexie == Num) { *++e = -nub; next(); } else { *++e = -1; *++e = PSH; expr(Inc); *++e = MUL; }
    ilk = INT;
  }
  else if (lexie == Inc || lexie == Dec) {
    t = lexie; next(); expr(Inc);
    if (*e == LC) { *e = PSH; *++e = LC; }
    else if (*e == LI) { *e = PSH; *++e = LI; }
    else { printf("%d: bad lvalue in pre-increment\n", line); exit(-1); }
    *++e = PSH;
    *++e = IMM; *++e = (ilk > PTR) ? sizeof(int) : sizeof(char);
    *++e = (t == Inc) ? ADD : SUB;
    *++e = (ilk == CHAR) ? SC : SI;
  }
  else { printf("%d: bad expression\n", line); exit(-1); }

  while (lexie >= lev) { // "precedence climbing" or "Top Down Operator Precedence" method
    t = ilk;
    if (lexie == Assign) {
      next();
      if (*e == LC || *e == LI) *e = PSH; else { printf("%d: bad lvalue in assignment\n", line); exit(-1); }
      expr(Assign); *++e = ((ilk = t) == CHAR) ? SC : SI;
    }
    else if (lexie == Cond) {
      next();
      *++e = BZ; d = ++e;
      expr(Assign);
      if (lexie == ':') next(); else { printf("%d: conditional missing colon\n", line); exit(-1); }
      *d = (int)(e + 3); *++e = JMP; d = ++e;
      expr(Cond);
      *d = (int)(e + 1);
    }
    else if (lexie == Lor) { next(); *++e = BNZ; d = ++e; expr(Lan); *d = (int)(e + 1); ilk = INT; }
    else if (lexie == Lan) { next(); *++e = BZ;  d = ++e; expr(Or);  *d = (int)(e + 1); ilk = INT; }
    else if (lexie == Or)  { next(); *++e = PSH; expr(Xor); *++e = OR;  ilk = INT; }
    else if (lexie == Xor) { next(); *++e = PSH; expr(And); *++e = XOR; ilk = INT; }
    else if (lexie == And) { next(); *++e = PSH; expr(Eq);  *++e = AND; ilk = INT; }
    else if (lexie == Eq)  { next(); *++e = PSH; expr(Lt);  *++e = EQ;  ilk = INT; }
    else if (lexie == Ne)  { next(); *++e = PSH; expr(Lt);  *++e = NE;  ilk = INT; }
    else if (lexie == Lt)  { next(); *++e = PSH; expr(Shl); *++e = LT;  ilk = INT; }
    else if (lexie == Gt)  { next(); *++e = PSH; expr(Shl); *++e = GT;  ilk = INT; }
    else if (lexie == Le)  { next(); *++e = PSH; expr(Shl); *++e = LE;  ilk = INT; }
    else if (lexie == Ge)  { next(); *++e = PSH; expr(Shl); *++e = GE;  ilk = INT; }
    else if (lexie == Shl) { next(); *++e = PSH; expr(Add); *++e = SHL; ilk = INT; }
    else if (lexie == Shr) { next(); *++e = PSH; expr(Add); *++e = SHR; ilk = INT; }
    else if (lexie == Add) {
      next(); *++e = PSH; expr(Mul);
      if ((ilk = t) > PTR) { *++e = PSH; *++e = IMM; *++e = sizeof(int); *++e = MUL;  }
      *++e = ADD;
    }
    else if (lexie == Sub) {
      next(); *++e = PSH; expr(Mul);
      if (t > PTR && t == ilk) { *++e = SUB; *++e = PSH; *++e = IMM; *++e = sizeof(int); *++e = DIV; ilk = INT; }
      else if ((ilk = t) > PTR) { *++e = PSH; *++e = IMM; *++e = sizeof(int); *++e = MUL; *++e = SUB; }
      else *++e = SUB;
    }
    else if (lexie == Mul) { next(); *++e = PSH; expr(Inc); *++e = MUL; ilk = INT; }
    else if (lexie == Div) { next(); *++e = PSH; expr(Inc); *++e = DIV; ilk = INT; }
    else if (lexie == Mod) { next(); *++e = PSH; expr(Inc); *++e = MOD; ilk = INT; }
    else if (lexie == Inc || lexie == Dec) {
      if (*e == LC) { *e = PSH; *++e = LC; }
      else if (*e == LI) { *e = PSH; *++e = LI; }
      else { printf("%d: bad lvalue in post-increment\n", line); exit(-1); }
      *++e = PSH; *++e = IMM; *++e = (ilk > PTR) ? sizeof(int) : sizeof(char);
      *++e = (lexie == Inc) ? ADD : SUB;
      *++e = (ilk == CHAR) ? SC : SI;
      *++e = PSH; *++e = IMM; *++e = (ilk > PTR) ? sizeof(int) : sizeof(char);
      *++e = (lexie == Inc) ? SUB : ADD;
      next();
    }
    else if (lexie == Brak) {
      next(); *++e = PSH; expr(Assign);
      if (lexie == ']') next(); else { printf("%d: close bracket expected\n", line); exit(-1); }
      if (t > PTR) { *++e = PSH; *++e = IMM; *++e = sizeof(int); *++e = MUL;  }
      else if (t < PTR) { printf("%d: pointer type expected\n", line); exit(-1); }
      *++e = ADD;
      *++e = ((ilk = t - PTR) == CHAR) ? LC : LI;
    }
    else { printf("%d: compiler error lexie=%d\n", line, lexie); exit(-1); }
  }
}

/*
 * Parses statements
 */
void statement()
{
  int *a, *b;

  if (lexie == If) {
    next();
    if (lexie == '(') next(); else { printf("%d: open paren expected\n", line); exit(-1); }
    expr(Assign);
    if (lexie == ')') next(); else { printf("%d: close paren expected\n", line); exit(-1); }
    *++e = BZ; b = ++e;
    statement();
    if (lexie == Else) {
      *b = (int)(e + 3); *++e = JMP; b = ++e;
      next();
      statement();
    }
    *b = (int)(e + 1);
  }
  else if (lexie == While) {
    next();
    a = e + 1;
    if (lexie == '(') next(); else { printf("%d: open paren expected\n", line); exit(-1); }
    expr(Assign);
    if (lexie == ')') next(); else { printf("%d: close paren expected\n", line); exit(-1); }
    *++e = BZ; b = ++e;
    statement();
    *++e = JMP; *++e = (int)a;
    *b = (int)(e + 1);
  }
  else if (lexie == Return) {
    next();
    if (lexie != ';') expr(Assign);
    *++e = LEV;
    if (lexie == ';') next(); else { printf("%d: semicolon expected\n", line); exit(-1); }
  }
  else if (lexie == '{') {
    next();
    while (lexie != '}') statement();
    next();
  }
  else if (lexie == ';') {
    next();
  }
  else {
    expr(Assign);
    if (lexie == ';') next(); else { printf("%d: semicolon expected\n", line); exit(-1); }
  }
}

/**
 * Starts the processing of the source, and stirs interpreter VM's execution
 */
int main(int argc, char **argv)
{
  int fd, bt, ilk, poolsz, *idmain;
  int *pc, *sp, *bp, a, cycle; // vm registers
  int i, *t; // temps

  --argc; ++argv;
  if (argc > 0 && **argv == '-' && (*argv)[1] == 's') { pen = 1; --argc; ++argv; }
  if (argc > 0 && **argv == '-' && (*argv)[1] == 'd') { spell = 1; --argc; ++argv; }
  if (argc < 1) { printf("usage: c4 [-s] [-d] file ...\n"); return -1; }

  if ((fd = open(*argv, 0)) < 0) { printf("could not open(%s)\n", *argv); return -1; }

  poolsz = 256*1024; // arbitrary size
  if (!(lexemes = malloc(poolsz))) { printf("could not malloc(%d) symbol area\n", poolsz); return -1; }
  if (!(le = e = malloc(poolsz))) { printf("could not malloc(%d) text area\n", poolsz); return -1; }
  if (!(data = malloc(poolsz))) { printf("could not malloc(%d) data area\n", poolsz); return -1; }
  if (!(sp = malloc(poolsz))) { printf("could not malloc(%d) stack area\n", poolsz); return -1; }

  memset(lexemes,  0, poolsz);
  memset(e,    0, poolsz);
  memset(data, 0, poolsz);

  p = "char else enum if int return sizeof while "
      "open read close printf malloc free memset memcmp exit void main";
  i = Char; while (i <= While) { next(); pin[Tk] = i++; } // add keywords to symbol table
  i = OPEN; while (i <= EXIT) { next(); pin[Class] = Sys; pin[Type] = INT; pin[Val] = i++; } // add library to symbol table
  next(); pin[Tk] = Char; // handle void type
  next(); idmain = pin; // keep track of main

  if (!(lp = p = malloc(poolsz))) { printf("could not malloc(%d) source area\n", poolsz); return -1; }
  if ((i = read(fd, p, poolsz-1)) <= 0) { printf("read() returned %d\n", i); return -1; }
  p[i] = 0;
  close(fd);

  // parse declarations
  line = 1;
  next();
  while (lexie) {
    bt = INT; // basetype
    if (lexie == Int) next();
    else if (lexie == Char) { next(); bt = CHAR; }
    else if (lexie == Enum) {
      next();
      if (lexie != '{') next();
      if (lexie == '{') {
        next();
        i = 0;
        while (lexie != '}') {
          if (lexie != Id) { printf("%d: bad enum identifier %d\n", line, lexie); return -1; }
          next();
          if (lexie == Assign) {
            next();
            if (lexie != Num) { printf("%d: bad enum initializer\n", line); return -1; }
            i = nub;
            next();
          }
          pin[Class] = Num; pin[Type] = INT; pin[Val] = i++;
          if (lexie == ',') next();
        }
        next();
      }
    }
    while (lexie != ';' && lexie != '}') {
      ilk = bt;
      while (lexie == Mul) { next(); ilk = ilk + PTR; }
      if (lexie != Id) { printf("%d: bad global declaration\n", line); return -1; }
      if (pin[Class]) { printf("%d: duplicate global definition\n", line); return -1; }
      next();
      pin[Type] = ilk;
      if (lexie == '(') { // function
        pin[Class] = Fun;
        pin[Val] = (int)(e + 1);
        next(); i = 0;
        while (lexie != ')') {
          ilk = INT;
          if (lexie == Int) next();
          else if (lexie == Char) { next(); ilk = CHAR; }
          while (lexie == Mul) { next(); ilk = ilk + PTR; }
          if (lexie != Id) { printf("%d: bad parameter declaration\n", line); return -1; }
          if (pin[Class] == Loc) { printf("%d: duplicate parameter definition\n", line); return -1; }
          pin[HClass] = pin[Class]; pin[Class] = Loc;
          pin[HType]  = pin[Type];  pin[Type] = ilk;
          pin[HVal]   = pin[Val];   pin[Val] = i++;
          next();
          if (lexie == ',') next();
        }
        next();
        if (lexie != '{') { printf("%d: bad function definition\n", line); return -1; }
        pad = ++i;
        next();
        while (lexie == Int || lexie == Char) {
          bt = (lexie == Int) ? INT : CHAR;
          next();
          while (lexie != ';') {
            ilk = bt;
            while (lexie == Mul) { next(); ilk = ilk + PTR; }
            if (lexie != Id) { printf("%d: bad local declaration\n", line); return -1; }
            if (pin[Class] == Loc) { printf("%d: duplicate local definition\n", line); return -1; }
            pin[HClass] = pin[Class]; pin[Class] = Loc;
            pin[HType]  = pin[Type];  pin[Type] = ilk;
            pin[HVal]   = pin[Val];   pin[Val] = ++i;
            next();
            if (lexie == ',') next();
          }
          next();
        }
        *++e = ENT; *++e = i - pad;
        while (lexie != '}') statement();
        *++e = LEV;
        pin = lexemes; // unwind symbol table locals
        while (pin[Tk]) {
          if (pin[Class] == Loc) {
            pin[Class] = pin[HClass];
            pin[Type] = pin[HType];
            pin[Val] = pin[HVal];
          }
          pin = pin + Idsz;
        }
      }
      else {
        pin[Class] = Glo;
        pin[Val] = (int)data;
        data = data + sizeof(int);
      }
      if (lexie == ',') next();
    }
    next();
  }

  if (!(pc = (int *)idmain[Val])) { printf("main() not defined\n"); return -1; }
  if (pen) return 0;

  // setup stack
  bp = sp = (int *)((int)sp + poolsz);
  *--sp = EXIT; // call exit if main returns
  *--sp = PSH; t = sp;
  *--sp = argc;
  *--sp = (int)argv;
  *--sp = (int)t;

  // run...
  cycle = 0;
  while (1) {
    i = *pc++; ++cycle;
    if (spell) {
      printf("%d> %.4s", cycle,
        &"LEA ,IMM ,JMP ,JSR ,BZ  ,BNZ ,ENT ,ADJ ,LEV ,LI  ,LC  ,SI  ,SC  ,PSH ,"
         "OR  ,XOR ,AND ,EQ  ,NE  ,LT  ,GT  ,LE  ,GE  ,SHL ,SHR ,ADD ,SUB ,MUL ,DIV ,MOD ,"
         "OPEN,READ,CLOS,PRTF,MALC,FREE,MSET,MCMP,EXIT,"[i * 5]);
      if (i <= ADJ) printf(" %d\n", *pc); else printf("\n");
    }
    if      (i == LEA) a = (int)(bp + *pc++);                             // load local address
    else if (i == IMM) a = *pc++;                                         // load global address or immediate
    else if (i == JMP) pc = (int *)*pc;                                   // jump
    else if (i == JSR) { *--sp = (int)(pc + 1); pc = (int *)*pc; }        // jump to subroutine
    else if (i == BZ)  pc = a ? pc + 1 : (int *)*pc;                      // branch if zero
    else if (i == BNZ) pc = a ? (int *)*pc : pc + 1;                      // branch if not zero
    else if (i == ENT) { *--sp = (int)bp; bp = sp; sp = sp - *pc++; }     // enter subroutine
    else if (i == ADJ) sp = sp + *pc++;                                   // stack adjust
    else if (i == LEV) { sp = bp; bp = (int *)*sp++; pc = (int *)*sp++; } // leave subroutine
    else if (i == LI)  a = *(int *)a;                                     // load int
    else if (i == LC)  a = *(char *)a;                                    // load char
    else if (i == SI)  *(int *)*sp++ = a;                                 // store int
    else if (i == SC)  a = *(char *)*sp++ = a;                            // store char
    else if (i == PSH) *--sp = a;                                         // push

    else if (i == OR)  a = *sp++ |  a;
    else if (i == XOR) a = *sp++ ^  a;
    else if (i == AND) a = *sp++ &  a;
    else if (i == EQ)  a = *sp++ == a;
    else if (i == NE)  a = *sp++ != a;
    else if (i == LT)  a = *sp++ <  a;
    else if (i == GT)  a = *sp++ >  a;
    else if (i == LE)  a = *sp++ <= a;
    else if (i == GE)  a = *sp++ >= a;
    else if (i == SHL) a = *sp++ << a;
    else if (i == SHR) a = *sp++ >> a;
    else if (i == ADD) a = *sp++ +  a;
    else if (i == SUB) a = *sp++ -  a;
    else if (i == MUL) a = *sp++ *  a;
    else if (i == DIV) a = *sp++ /  a;
    else if (i == MOD) a = *sp++ %  a;

    else if (i == OPEN) a = open((char *)sp[1], *sp);
    else if (i == READ) a = read(sp[2], (char *)sp[1], *sp);
    else if (i == CLOS) a = close(*sp);
    else if (i == PRTF) { t = sp + pc[1]; a = printf((char *)t[-1], t[-2], t[-3], t[-4], t[-5], t[-6]); }
    else if (i == MALC) a = (int)malloc(*sp);
    else if (i == FREE) free((void *)*sp);
    else if (i == MSET) a = (int)memset((char *)sp[2], sp[1], *sp);
    else if (i == MCMP) a = memcmp((char *)sp[2], (char *)sp[1], *sp);
    else if (i == EXIT) { printf("exit(%d) cycle = %d\n", *sp, cycle); return *sp; }
    else { printf("unknown instruction = %d! cycle = %d\n", i, cycle); return -1; }
  }
}
