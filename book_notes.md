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
