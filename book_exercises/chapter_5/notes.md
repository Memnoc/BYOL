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
