# Chapter 5 Bonus Marks

    › Write down some more examples of strings the Doge language contains.
    › Why are there back slashes \ in front of the quote marks " in the grammar?
    › Why are there back slashes \ at the end of the line in the grammar?
    › Describe textually a grammar for decimal numbers such as 0.01 or 52.221.
    › Describe textually a grammar for web URLs such as http://www.buildyourownlisp.com.
    › Describe textually a grammar for simple English sentences such as the cat sat on the mat.
    › Describe more formally the above grammars. Use |, *, or any symbols of your own invention.
    › If you are familiar with JSON, textually describe a grammar for it.

1. Write down some more examples of strings the Doge language contains.

Valid Doge language strings:

- Zero phrases:
  ""

- One phrase:
  "wow lisp"
  "many language"
  "such c"

- Two phrases:
  "many build wow c"
  "so book such language"

- Three phrases:
  "wow lisp many build such c"

- N phrases:
  Any combination of <adjective> <noun> pairs concatenated together, where adjective ∈ {wow, many, so, such} and noun ∈ {lisp, language, book, build, c}

2. Why are there back slashes \ in front of the quote marks " in the grammar?

- Two purposes: a) to make the string of characters more readable, since it's very long, and b) when found in instances like `\"wow\"` serves the purpose of escaping the character, since the grammar itself uses quotes to denote literals `"wow"` which means "match the text wow" - to put those grammar quotes into a C string, you escape them.

3. Why are there back slashes \ at the end of the line in the grammar?

- The backlash-newline is a line continuation mechanism. Without it, you cannot write a string literal across multiple lines in C source code. They also serve a readability purpose in this case, but they must be there for technical reasons.

4. Describe textually a grammar for decimal numbers such as 0.01 or 52.221.

```bash
digit   : '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9';
dot     : '.';
decimal : <digit>+ <dot> <digit>+;
```

5. Describe textually a grammar for web URLs such as http://www.buildyourownlisp.com.

Starting from a URL of this shape:
http://www.buildyourownlisp.com

```bash
protocol  : "http://" | "https://";
dot: '.';
letter    : 'a' | 'b' | 'c' | ... | 'z';
domain    : "com" | "org" | "net" | "io";
subdomain : "www." | "";
url       : <protocol> <subdomain> <letter>+ <dot> <domain>;
```

6. Describe textually a grammar for simple English sentences such as the cat sat on the mat.

"The cat sat on the mat."

```bash
determiner  : "the" | "a";
noun        : "cat" | "dog" | "mat" | "hat";
verb        : "sat" | "walked" | "jumped";
preposition : "on" | "under" | "near";
sentence: <determiner>* <noun> <verb> <prepositions> <determiner> <noun>;
```
