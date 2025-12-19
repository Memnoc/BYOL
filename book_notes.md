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

```

```
