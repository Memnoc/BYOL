# Notes from the book

## Chapter I

A program in C consists of only **function definitions** and **structure definitions**. Therefore a source file is simply a list of functions and types.

**Structs** are used to create new types. They are basically several variables bundled together into a single package.
We use structures to represent more complex data types.

For example, here is a complex type called point that represent a point in 2D space.

```C
typedef struct {
  float x;
  float y;
} point;

```

To use this type, we can place it above any functions that wishes to use it. Individual fields can be accessed via the dot notation.

```c
point p;
p.x = 0.1;
p.y = 10.0;

float length = sqrt(p.x * p.x + p.y * p.y);

```

## Strings

In C, strings are represented by the pointer type `char*`. Under the hood, they are arrays of characters, where the final character is a special character called the **null terminator**. String literals in C can be represented by simply putting text between quotation marks: "Hello, C".

## Loops: for and while

**While** repeatedly execute a block until some condition becomes false.
**For** is the classic that requires **initializer**, the **condition** and an **incrementer**. They are more compact and thus more widely used than the while loops.

## Chomsky's observations on natural languages

"Natural languages are built up of recursive and repeated substructures" - and this is very true for programming languages as well.
The consequence of this observation by Chomsky is important. It means that although there are an infinite number of different things that can be said, or written down in a particular language, it is still possible to process and understand all of them with a finite number of re-write rules. The name given to a set of re-write rules is a grammar.
[Wiki on Chomsky Hierachy](https://en.wikipedia.org/wiki/Chomsky_hierarchy)

## MPC - Parser Combinator

The book's author gears us towards using a library of his invention called `mpc`. It's a parser combinator that let's you easily define a language grammar, something we need to have in order to describe our own Lisp.
The library is still supported and has a huge number of stars and forks, this is reassuring :)
[orangeduck/mcp](https://github.com/orangeduck/mpc)

## The lanugage of Shiba Inu

The book explains how to use the `mpc` library with an example: an attempt to decode the language of [Shiba Inu](https://www.buildyourownlisp.com/chapter5_languages#parser_combinators)
The criteria are as follows:

› An Adjective is either "wow", "many", "so" or "such".
› A Noun is either "lisp", "language", "c", "book" or "build".
› A Phrase is an Adjective followed by a Noun.
› A Doge is zero or more Phrases.

```C
/* Build a parser 'Adjective' to recognize descriptions'*/
mpc_parser_t* Adjective = mpc_or(4,
  mpc_sym("wow"), mpc_sym("many"),
  mpc_sym("so"), mpc_sym("such")
);


/* Build a parser 'Noun' to recognize things */
mpc_parser_t* Noun = mpc_or(5,
  mpc_sym("lisp"), mpc_sym("language"),
  mpc_sym("book"), mpc_sym("build"),
  mpc_sym("c")
);

```

To create a phrase, the library has a dedicated function called `mpc_and` which literally means a functions that takes two arguments (it actually takes more but these two are the ones we care about for now) - Noun and Adjective:

```C
mpc_parser_t* Phrase = mpc_and(2, mpcf_strfold, Adjective, Noun, free);
```

Notice that `mpcf_strfold` serves the purposed of defining how results are joined together.

```C
mpc_parser_t* Doge = mpc_many(mpcf_strfold, Phrase);
```

In the example above, we define the Doge language using the `mpc_many` functions to specify which parser are required (zero or more) and we pass again `mpcf_strfold` to specify how the results are joined together.

Effectively, by declaring `Doge` the way we did, we are declaring a **parser that looks for zero or more occurrences of another parser**. Because our `Doge` parser accepts inputs of any length, this creates an _ipso facto_ **infinite** language.
For a more granular approach, the [mpc respository](http://github.com/orangeduck/mpc) is something I want to visit in the future.

Here is a more detailed example of a grammar in `mcp` with a little more natural form.

```C
mpc_parser_t* Adjective = mpc_new("adjective");
mpc_parser_t* Noun      = mpc_new("noun");
mpc_parser_t* Phrase    = mpc_new("phrase");
mpc_parser_t* Doge      = mpc_new("doge");

mpca_lang(MPCA_LANG_DEFAULT,
  "                                           \
    adjective : \"wow\" | \"many\"            \
              |  \"so\" | \"such\";           \
    noun      : \"lisp\" | \"language\"       \
              | \"book\" | \"build\" | \"c\"; \
    phrase    : <adjective> <noun>;           \
    doge      : <phrase>*;                    \
  ",
  Adjective, Noun, Phrase, Doge);

/* Do some parsing here... */

mpc_cleanup(4, Adjective, Noun, Phrase, Doge);
```

## Polish Notation

To use `mpc`, the book guides me into implementing a simple grammar that resembles a mathematical subset of Lisp. A simple notation for arithmetic where the operator comes before the operands.

```bash
1 + 2 + 3 is + 1 2 6
6 + (2 * 9) is + 6 (* 2 9)
(10 * 2) / (4 + 2) is / (* 10 2) (+ 4 2)
```

We need to work out a grammar that describes exactly this notation.
Personal observation: this is exactly like Common Lisp.
Secondly, we can say that _a program is an operator followed by one or more expressions_ where _an expression_ is either a number, or, in parentheses, an operator followed by one or more expressions.

More formally:

```
Program -> the start of input, and Operator, one or more Expression, and the end of input.
Expression -> Either a number or (, and Operator, one or more Expression, and an ).
Operator ->  '+', '-', '*', or '/'.
Number -> an optional '-' and or more characters between 0 and 9.
```

### Regular Expressions

A way to write grammar for small sections of text such as words or numbers. We kind of need this at this point because we have no way to express things like: the end of an input, optional characters, or range of characters.
The regular expressions rules we need are:

```
. -> Any character is required
a -> The character 'a' is required
[abcdef] -> Any character set 'abcdef' is required
[a-f] -> Any character in the range 'a' to 'f' is required
a? -> The character 'a' is optional
a* -> Zero or more of the character 'a' are required
a+ -> One or more of the character 'a' are  required
^ -> The start of input  is required
$ -> The end of input is required
```

## Practical implementation of the Polish Notation

What follows is a practical implementation of the Polish Notation grammar:

```C
  mpc_parser_t *Number = mpc_new("number");
  mpc_parser_t *Operator = mpc_new("operator");
  mpc_parser_t *Expr = mpc_new("expr");
  mpc_parser_t *Starspy = mpc_new("starspy");

  /* Define them with the following Language */
  mpca_lang(MPCA_LANG_DEFAULT,
            "                                                     \
    number   : /-?[0-9]+/ ;                             \
    operator : '+' | '-' | '*' | '/' ;                  \
    expr     : <number> | '(' <operator> <expr>+ ')' ;  \
    lispy    : /^/ <operator> <expr>+ /$/ ;             \
  ",
            Number, Operator, Expr, Starspy);
```

Basically, the implementation creates some parser using `mpc` and make sure it cleans it right before main ends:

```C
mpc_cleanup(4, Number, Operator, Expr, Starspy);
```

To then be able to use the parser while gathering user input, we pass them in the while loop:

```C
mpc_result_t r;
if(mpc_parse("<stdin>", input, Starspy, &r)) {
  /* On Success Print the AST */
  mpc_ast_print(r.output);
  mp_ast_delete(r.output);
} else {
  /* Otherwise Print the Error */
  mpc_err_print(r.error);
  mpc_err_delete(r.error);
}
```

The result is something able to parse expressions like:

```bash
starspy> + 5 (* 2 2)
regex
  operator|char:1:1 '+'
  expr|number|regex:1:3 '5'
  expr|>
    char:1:5 '('
    operator|char:1:6 '*'
    expr|number|regex:1:8 '2'
    expr|number|regex:1:10 '2'
    char:1:11 ')'
  regex
```

1. We call the `mpc_parse` and we pass our parser Starspy, and the input from the user `input`.
2. The result of parsing is copied into the variable `r` and it returns `1` on success and `0` on failure. Important to notice, we pass the address of r `&r` so just the pointer and not a copy of `r`. This becomes relevant because `mpc_parse()` receives a pointer and thus can modify the original variable:

```C
int mpc_parse(const char *filename, const char *string, mpc_parser_t *p, mpc_result_t *r) {}
```

3. On success the internal structures are copied into `r` in the field `output` and it's printed out using `mpc_ast_print` and subsequently deleted using `mpc_ast_delete`. Conversely, in case of an error, we copy that into `r` and in the field `error` and we print is using `mpc_err_print` and delete it using `mpc_err_delete` `
