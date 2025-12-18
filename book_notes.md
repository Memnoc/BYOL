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
