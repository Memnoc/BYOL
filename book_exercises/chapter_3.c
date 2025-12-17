#include <stdio.h>

// INFO: declare a function that outputs "Hello, C" `n` number of times. Call
// this from main.

int hello_c(int c) {
  int counter = 0;
  while (counter < 5) {
    puts("() Hello, C");
    counter++;
  }
  return 0;
}

int main(void) {
  // INFO: use a for loop to print out "Hello, C" five times.
  for (int i = 0; i < 5; i++) {
    puts("For Hello, C");
  }

  // INFO: use a while loop to print out "Hello, C" five times.
  int i = 0;
  while (i < 5) {
    puts("While Hello, C");
    i++;
  }

  // INFO: tCall function
  hello_c(5);

  // INFO: What is the do loop, and how does that work in C?
  // https://www.w3schools.com/c/c_do_while_loop.php
  int y = 0;
  do {
    puts("Do/While loop");
    y++;
  } while (y < 5);

  // INFO: what is the switch statement and how does that work?
  int day = 4;
  switch (day) {
  case 1:
    puts("Monday");
    break;
  case 2:
    puts("Tuesday");
    break;
  case 3:
    puts("Wednesday");
    break;
  case 4:
    puts("Thursday");
    break;
  case 5:
    puts("Friday");
    break;
  }
  // outputs "Thursday" day 4
  return 0;
}

// INFO: What built in types are there other than the ones listed?
// https://www.geeksforgeeks.org/c/data-types-in-c/
// INFO: what other conditional operators are there other than greater than `>`
// and less than `<`? https://www.geeksforgeeks.org/c/operators-in-c/
// INFO: what does `typedef` do exactly?
// It lets you create a new name alias for an existing type. It's basically an
// aid to readability.
