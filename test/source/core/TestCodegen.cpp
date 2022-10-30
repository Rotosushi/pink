
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <random>

#include <algorithm>
#include <fstream>

#include "Test.h"
#include "core/TestCodegen.h"

#include "aux/Error.h"

// this function is nearly identical to system(),
// except it runs the given process directly,
// and does not invoke the shell to do it.
// this removes a call to exec, and thus speeds
// up this code.
// #PORTABILITY: This function is not portable.
//    The functions which allow you to spawn new
//    processes are not the same accross OS's, and
//    it is for this reason that this section of
//    tests might be better written in a more cross
//    platform language like python.
//    alternatively, if a version of this function
//    with the same signature can be defined for
//    more than one operating system, then we could
//    have both versions here in the source file
//    simply choose one or the other
//    #if defined(__OS__)
//    ...
//    #endif
//    with preprocessor guards.
auto Run(const char *file, char *const *argv) -> int {
  int result = 0;

  pid_t pid = fork();

  if (pid < 0) {
    pink::FatalError(std::string("A call to fork failed: ") + strerror(errno),
                     __FILE__, __LINE__);
  }

  if (pid == 0) // we are executing the child process
  {
    execv(file, argv);
    exit(1); // execv never returns.
  } else {   // we are executing the parent process
    siginfo_t siginfo;
    int wait_result = waitid(P_PID, pid, &siginfo, WEXITED);

    if (wait_result < 0) {
      pink::FatalError(std::string("A call to waitid failed: ") +
                           strerror(errno),
                       __FILE__, __LINE__);
    }

    if (siginfo.si_code == CLD_EXITED) {
      result = siginfo.si_value.sival_int;
    } else {
      result = -1;
    }
  }

  return result;
}

/*
 * This function wraps the core test actions for a file within a function,
 * Since we are testing the execution of the compiler itself, we are going
 * to have to run the compiler multiple times against multiple files to
 * test multiple things. the general process is as follows:
 *  1: emit the test_contents into a file
 *  2: compile that file into an executable
 *  3: run that executable and check the exit code matches expected_value.
 *      return true when the code matches, false otherwise.
 *
 * note: i realize that this is not how the exit code for programs is actually
 * intended to be used, however we have no other way of observing the results
 * of executing the output program. We can modify this procedure to pipe the
 * output of the program into somewhere we can inspect. in this way we should
 * be able to write more complex tests and more complex programs.
 * but that relies on the compiler defining read/write routines, strings in some
 * capacity, and defining the stdin, stdout, and stderr io streams for the
 * program to utilize this functionality.
 *
 * note: The exit_code itself can only contain 8 bits of information, so we can
 * only reliably test the behavior of the program if the programs output is also
 * restricted to this range of values. (I suppose we could also force whatever
 * value we compute into a byte sized scalar, which would cut off the number in
 * the same way that getting the exit_code does. but that is very opaque.)
 */
// NOLINTBEGIN
// I would use modern cpp practices, but this is linux interop code,
// so this code is in an old style.
auto TestFile(std::string test_contents, int expected_value) -> bool {
  bool result = true;

  char filename[] = "test_file.p";

  std::fstream outfile;

  outfile.open(filename,
               std::ios_base::in | std::ios_base::out | std::ios_base::trunc);

  if (!outfile.is_open()) {
    pink::FatalError(std::string("Failed to open file: ") + filename, __FILE__,
                     __LINE__);
  }

  outfile << test_contents;

  outfile.close();

  char pink[] = "./pink";

  char *const pink_args[] = {pink, filename, (char *)nullptr};

  Run(pink, pink_args);

  char exe_name[] = "test_file";

  char *exe_args[] = {exe_name, (char *)nullptr};

  int prog_result = Run(exe_name, exe_args);

  char obj_name[] = "test_file.o";

  std::remove(obj_name);
  std::remove(filename);
  std::remove(exe_name);

  if (prog_result != expected_value) {
    result = false;
  }

  return result;
}

auto TestSimpleCodegen(std::ostream &out, int numIter, std::mt19937_64 &gen)
    -> bool {
  std::uniform_int_distribution<> zero_to_one_hundred(0, 100);
  bool result = true;
  for (int i = 0; i < numIter; i++) {
    int num = zero_to_one_hundred(gen);
    std::string numstr = std::to_string(num);
    result &=
        Test(out, "Simple main " + numstr + "",
             TestFile(std::string("fn main () { ") + numstr + "; }\n", num));
  }
  return result;
}

auto TestBindCodegen(std::ostream &out, int numIter, std::mt19937_64 &gen)
    -> bool {
  std::uniform_int_distribution<> zero_to_one_hundred(0, 100);
  bool result = true;
  for (int i = 0; i < numIter; i++) {
    int num1 = zero_to_one_hundred(gen);
    std::string num1str = std::to_string(num1);

    result &= Test(
        out, "Bind: x := " + num1str,
        TestFile(std::string("fn main() { var x := ") + num1str + "; x;}\n",
                 num1));
  }
  return result;
}

auto TestAssignmentCodegen(std::ostream &out, int numIter, std::mt19937_64 &gen)
    -> bool {
  std::uniform_int_distribution<> zero_to_one_hundred(0, 100);
  bool result = true;
  // tests that assignment works
  for (int i = 0; i < numIter; i++) {
    int num1 = zero_to_one_hundred(gen);
    std::string num1str = std::to_string(num1);

    result &= Test(out, "Assignment: x = " + num1str,
                   TestFile(std::string("fn main() { var x := 0; x =") +
                                num1str + "; x;}\n",
                            num1));
  }

  // tests that compound assignment works
  for (int i = 0; i < numIter; i++) {
    int num1 = zero_to_one_hundred(gen);
    std::string num1str = std::to_string(num1);

    result &= Test(
        out, "Compound Assignment: x = y = " + num1str,
        TestFile(std::string("fn main() { var x := 0; var y := 0; x = y = " +
                             num1str + "; x == y;}\n"),
                 1));
  }

  // tests that the lhs and rhs of an assignment expression treat
  // variables distinctly
  for (int i = 0; i < numIter; i++) {
    int num1 = zero_to_one_hundred(gen);
    int res = num1 + num1;
    std::string num1str = std::to_string(num1);
    std::string resstr = std::to_string(res);

    result &= Test(out,
                   "Assignment to modified self: x = " + num1str +
                       ", x + x == " + resstr,
                   TestFile(std::string("fn main () { var x := ") + num1str +
                                "; x = x + x; x;}\n",
                            res));
  }
  return result;
}

auto TestBinopIntegerArithmetic(std::ostream &out, int numIter,
                                std::mt19937_64 &gen) -> bool {
  bool result = true;
  std::uniform_int_distribution<> zero_to_one_hundred(0, 100);
  std::uniform_int_distribution<> one_to_one_hundred(1, 100);
  std::uniform_int_distribution<> zero_to_four(0, 4);
  std::uniform_int_distribution<> zero_to_ten(0, 10);
  for (int i = 0; i < numIter; i++) {
    int num1 = 0;
    int num2 = 0; // x, y | x + y <= 100
    int res = 0;
    std::string num1str;
    std::string num2str;
    std::string resstr;
    // select a random binary operator defined for integers
    int numop = zero_to_four(gen);
    const char *op = nullptr;
    // initialize variables according to the behavior of the operator selected
    switch (numop) {
    case 0:
      op = "+";
      num1 = zero_to_one_hundred(gen);
      num2 = zero_to_one_hundred(gen);
      res = num1 + num2;
      num1str = std::to_string(num1);
      num2str = std::to_string(num2);
      resstr = std::to_string(num1 + num2);
      break;

    case 1: {
      op = "-";
      int n1 = one_to_one_hundred(gen);
      int n2 = one_to_one_hundred(gen);
      // ensure subtraction does not produce a negative value
      if (n1 > n2) {
        num1 = n1;
        num2 = n2;
      } else {
        num1 = n2;
        num2 = n1;
      }

      res = num1 - num2;
      num1str = std::to_string(num1);
      num2str = std::to_string(num2);
      resstr = std::to_string(num1 - num2);
      break;
    }

    case 2:
      op = "*";
      // ensure that multiplication doesn't produce values greater than one
      // hundred, we can construct values up to 255 without worry, but one
      // hundred is an easy bounds to think about.
      num1 = zero_to_ten(gen);
      num2 = zero_to_ten(gen);
      res = num1 * num2;
      num1str = std::to_string(num1);
      num2str = std::to_string(num2);
      resstr = std::to_string(num1 * num2);
      break;

    case 3:
      op = "/";
      // ensure that division doesn't have zero on the bottom
      num1 = zero_to_one_hundred(gen);
      num2 = one_to_one_hundred(gen);
      res = num1 / num2;
      num1str = std::to_string(num1);
      num2str = std::to_string(num2);
      resstr = std::to_string(num1 / num2);
      break;

    case 4:
      op = "%";
      num1 = zero_to_one_hundred(gen);
      num2 = one_to_one_hundred(gen);
      res = num1 % num2;
      num1str = std::to_string(num1);
      num2str = std::to_string(num2);
      resstr = std::to_string(num1 % num2);
      break;
    }

    result &=
        Test(out,
             "Int Arithmetic Binop: x = " + num1str + "; y = " + num2str +
                 "; x " + std::string(op) + " y == " + resstr,
             TestFile(std::string("fn main () { var x := 0; var y := 0; x = ") +
                          num1str + "; y = " + num2str + "; var c := x " +
                          std::string(op) + " y; c;}\n",
                      res));
  }

  return result;
}

auto TestBinopIntegerComparison(std::ostream &out, int numIter,
                                std::mt19937_64 &gen) -> bool {
  bool result = true;
  std::uniform_int_distribution<> zero_to_one_hundred(0, 100);
  std::uniform_int_distribution<> zero_to_five(0, 5);
  for (int i = 0; i < numIter; i++) {
    int num1 = zero_to_one_hundred(gen);
    int num2 = zero_to_one_hundred(gen);
    bool res = false;
    std::string num1str = std::to_string(num1);
    std::string num2str = std::to_string(num2);
    std::string resstr;
    // select a random binary operator defined for integers
    int numop = zero_to_five(gen);
    const char *op = nullptr;

    // initialize variables according to the behavior of the operator selected
    switch (numop) {
    case 0:
      op = "==";
      res = (num1 == num2);
      resstr = res ? "true" : "false";
      break;

    case 1:
      op = "!=";
      res = (num1 != num2);
      resstr = res ? "true" : "false";
      break;

    case 2:
      op = "<";
      res = (num1 < num2);
      resstr = res ? "true" : "false";
      break;

    case 3:
      op = "<=";
      res = (num1 <= num2);
      resstr = res ? "true" : "false";
      break;

    case 4:
      op = ">";
      res = (num1 > num2);
      resstr = res ? "true" : "false";
      break;

    case 5:
      op = ">=";
      res = (num1 >= num2);
      resstr = res ? "true" : "false";
      break;
    }

    result &=
        Test(out,
             "Int Comparison Binop: x = " + num1str + "; y = " + num2str +
                 "; x " + std::string(op) + " y == " + resstr,
             TestFile(std::string("fn main () { var x := 0; var y := 0; x = ") +
                          num1str + "; y = " + num2str + "; var c := x " +
                          std::string(op) + " y; c;}\n",
                      res));
  }
  return result;
}

auto TestBinopBooleanArithmetic(std::ostream &out, int numIter,
                                std::mt19937_64 &gen) -> bool {
  bool result = true;
  std::uniform_int_distribution<> zero_to_one(0, 1);
  std::bernoulli_distribution true_or_false(0.5);
  for (int i = 0; i < numIter; i++) {
    // test boolean arithmetic operators
    bool b1 = true_or_false(gen);
    bool b2 = true_or_false(gen);
    bool res = false;
    std::string b1str = b1 ? "true" : "false";
    std::string b2str = b2 ? "true" : "false";
    std::string resstr;
    std::string op;

    int numop = zero_to_one(gen);
    switch (numop) {
    case 0:
      op = "&";
      res = b1 && b2;
      resstr = res ? "true" : "false";
      break;

    case 1:
      op = "|";
      res = b1 || b2;
      resstr = res ? "true" : "false";
      break;
    }

    result &=
        Test(out,
             "Boolean Arithmetic Binop: x = " + b1str + "; y = " + b2str +
                 "; x " + op + " y == " + resstr,
             TestFile("fn main () { var x := " + b1str + "; var y := " + b2str +
                          "; var z := x " + op + " y; z; }",
                      res));
  }
  return result;
}

auto TestArrayCodegen(std::ostream &out, int numIter, std::mt19937_64 &gen)
    -> bool {
  bool result = true;
  for (int i = 0; i < numIter; i++) {
    int array[5] = {rand() % 50, rand() % 50, rand() % 50, rand() % 50,
                    rand() % 50};
    int idx1 = rand() % 5, idx2 = rand() % 5;
    std::string idx1str = std::to_string(idx1), idx2str = std::to_string(idx2);
    std::string num1str = std::to_string(array[idx1]),
                num2str = std::to_string(array[idx2]),
                resstr = std::to_string(array[idx1] + array[idx2]);
    std::string arr0str = std::to_string(array[0]),
                arr1str = std::to_string(array[1]),
                arr2str = std::to_string(array[2]),
                arr3str = std::to_string(array[3]),
                arr4str = std::to_string(array[4]);

    std::string teststr = "fn main() { a := [";
    teststr += arr0str + ",";
    teststr += arr1str + ",";
    teststr += arr2str + ",";
    teststr += arr3str + ",";
    teststr += arr4str + "];";
    teststr += "b := a[" + idx1str + "];";
    teststr += "c := a[" + idx2str + "];";
    teststr += "b + c;};";
    
    std::string descstr = "Allocating an array, [";
    descstr += arr0str + ",";
    descstr += arr1str + ",";
    descstr += arr2str + ",";
    descstr += arr3str + ",";
    descstr += arr4str;
    descstr += "] and performing arithmetic on two ofit's elements: ";
    descstr += num1str + " + ";
    descstr += num2str + " = ";
    descstr += resstr;
    result &= Test(out, descstr, TestFile(teststr, array[idx1] + array[idx2]));
  }
  return result;
}

// NOLINTEND

/*
 *  TestBasic tests that the compiler can emit a correct program,
 *    given the input file. this revolves around generating
 *    random numbers for the contents of any given input file,
 *    so we can be sure to generate test code which we are not
 *    specifically conforming to.
 *
 *  #TODO: 9/15/2022
 *    This function is reaching 1000 lines. it may be too big.
 *    We could refactor this routine into more than one testing
 *    routine, for granularity of errors, and for readability of
 *    this file in particular. (all these tests are good so far.)
 *
 */
auto TestCodegen(std::ostream &out) -> bool {
  // So I can see two choices for testing the whole compiler:
  // one, we copy the code from main and call Compile and Link
  // on an input file.
  // two, we call the built program 'pink' passing in the correct
  // arguments to compile the file.
  // (there may be more choices)
  //
  // i think option two is better because we are testing the compiler,
  // and not it's components. This has the effect that we are testing
  // the exact same thing that the user interacts with.
  // additionally, if we ever make a change in main, we don't have to
  // make the corresponding change here to reflect the difference,
  // making development and upkeep easier.

  bool result = true;
  out << "\n----------------------------------\n";
  out << "Testing pink::Ast::Codegen: \n";

  // the tests are defined such that we can run them multiple
  // times with different random numbers, so we are not cherry
  // picking behavior to test. This is good, because it means
  // we cannot write the code to fit exactly to the tests. It
  // is also bad, because it means we are not specifically testing
  // every edge case.
  int numIter = 1;

  std::random_device rnd;
  std::mt19937_64 gen(rnd());

  result &= TestSimpleCodegen(out, numIter, gen);
  result &= TestBindCodegen(out, numIter, gen);
  result &= TestAssignmentCodegen(out, numIter, gen);
  result &= TestBinopIntegerArithmetic(out, numIter, gen);
  result &= TestBinopIntegerComparison(out, numIter, gen);
  result &= TestBinopBooleanArithmetic(out, numIter, gen);
  /*
    for (int i = 0; i < numIter; i++)
    {
      int num1 = rand() % 50, num2 = rand() % 50; // x, y | x + y <= 100
      std::string num1str = std::to_string(num1);
      std::string num2str = std::to_string(num2);

      result &= Test(
        out,
        "Create a Pointer, Assign through a Pointer: x = (" + num1str + "); y =
    &x; y = " + num2str , TestFile( std::string("fn main () { x := 0; y := &x; x
    = ") + num1str + "; *y = " + num2str + "; x;};\n", num2
          )
      );
    }

    for (int i = 0; i < numIter; i++)
    {
      int num1 = rand() % 50, num2 = rand() % 50; // x, y | x + y <= 100
      std::string num1str = std::to_string(num1);
      std::string num2str = std::to_string(num2);
      std::string resstr  = std::to_string(num1 + num2);

      result &= Test(
        out,
        "Arithmetic through a Pointer: x = (" + num1str + "); y = (" + num2str +
    "); x + y = " + resstr , TestFile( std::string("fn main () { a := b := c :=
    0; x := &a; y := &b; z := &c; *x = ")
            + num1str
            + "; *y = "
            + num2str
            + "; *z = *x + *y; c;};\n",
          num1 + num2
          )
      );
    }

    for (int i = 0; i < numIter; i++)
    {
      int num1 = rand() % 2, num2 = rand() % 2; // x, y | x + y <= 100
      std::string num1str = num1 == 0 ? "false" : "true";
      std::string num2str = num2 == 0 ? "false" : "true";
      std::string resstr  = num1 == num2 ? "true" : "false";

      result &= Test(
        out,
        "Boolean arithmetic through a pointer: x = (" + num1str + "); y = (" +
    num2str + "); x == y = " + resstr , TestFile( std::string("fn main () { a :=
    b := c := false; x := &a; y := &b; z := &c; *x = ")
            + num1str
            + "; *y = "
            + num2str
            + "; *z = *x == *y; c;};\n",
          num1 == num2
          )
      );
    }



    for (int i = 0; i < numIter; i++)
    {
      int num1 = rand() % 50, num2 = rand() % 50;
      int idx1 = rand() % 5, idx2 = rand() % 5;

      // if we reassign the contents to the same location, that causes
      // us to overwrite the contents, and so the addition will not
      // match what we expect, since this is not a problem with the
      // code, and simply a problem with the test text, we avoid that case
      // here.
      while (idx1 == idx2)
      {
        idx2 = rand() % 5;
      }

      std::string idx1str = std::to_string(idx1),
                  idx2str = std::to_string(idx2);
      std::string num1str = std::to_string(num1),
                  num2str = std::to_string(num2),
                  resstr  = std::to_string(num1 + num2);

      std::string teststr = std::string("fn main() { a := [0,0,0,0,0];")
                                      + "b := a + " + idx1str + "; *b = " +
    num1str + ";"
                                      + "c := a + " + idx2str + "; *c = " +
    num2str + ";"
                                      + "*b + *c;};";
      result &= Test(out,
              "Allocating an array [0,0,0,0,0], Assigning values to two
    elements, " + idx1str + " and " + idx2str + ". Then performing arithmetic on
    those two elements: " + num1str + " + " + num2str + " = " + resstr,
              TestFile(teststr, num1 + num2)
              );

    }

    for (int i = 0; i < numIter; i++)
    {
      int array[] = {rand() % 100, rand() % 100, rand() % 100, rand() % 100,
    rand() % 100}; int idx = rand() % 5, res = array[idx]; std::string a0str =
    std::to_string(array[0]), a1str = std::to_string(array[1]), a2str =
    std::to_string(array[2]), a3str = std::to_string(array[3]), a4str =
    std::to_string(array[4]), idxstr = std::to_string(idx), resstr =
    std::to_string(res);

      std::string teststr = std::string("fn main() { a := (")
                          + a0str + ","
                          + a1str + ","
                          + a2str + ","
                          + a3str + ","
                          + a4str + "); b := a."
                          + idxstr + "; b;};";

      result &= Test(out,
                    "Allocating a Tuple (" + a0str + "," + a1str + "," + a2str +
    "," + a3str + "," + a4str + ") Accessing element " + idxstr + " = " +
    resstr, TestFile(teststr, res));
    }

    for (int i = 0; i < numIter; i++)
    {
      int array[] = {rand() % 100, rand() % 100, rand() % 100, rand() % 100,
    rand() % 100}; int idx = rand() % 5, res = rand() % 100; std::string a0str =
    std::to_string(array[0]), a1str = std::to_string(array[1]), a2str =
    std::to_string(array[2]), a3str = std::to_string(array[3]), a4str =
    std::to_string(array[4]), idxstr = std::to_string(idx), resstr =
    std::to_string(res);

      std::string teststr = std::string("fn main() { a := (")
                          + a0str + ","
                          + a1str + ","
                          + a2str + ","
                          + a3str + ","
                          + a4str + "); a."
                          + idxstr + " = "
                          + resstr + "; a." + idxstr + ";};";

      result &= Test(out,
                    "Allocating a Tuple (" + a0str + "," + a1str + "," + a2str +
    "," + a3str + "," + a4str + ") Assigning element " + idxstr + " = " +
    resstr, TestFile(teststr, res));
    }

    for (int i = 0; i < numIter; i++)
    {
      int array[3][2] = {{rand() % 100, rand() % 100}, {rand() % 100, rand() %
    100}, {rand() % 100, rand() % 100}}; int idx1 = rand() % 3, idx2 = rand() %
    2, res = array[idx1][idx2]; std::string a0str = std::to_string(array[0][0]),
                  a1str = std::to_string(array[0][1]),
                  a2str = std::to_string(array[1][0]),
                  a3str = std::to_string(array[1][1]),
                  a4str = std::to_string(array[2][0]),
                  a5str = std::to_string(array[2][1]),
                  idx1str = std::to_string(idx1),
                  idx2str = std::to_string(idx2),
                  resstr = std::to_string(res);

      std::string teststr = std::string("fn main() { a := ((")
                          + a0str + ","
                          + a1str + "),("
                          + a2str + ","
                          + a3str + "),("
                          + a4str + ","
                          + a5str + "));"
                          + "a." + idx1str + "." + idx2str + ";};";

      result &= Test(out,
                    "Allocating a recursive Tuple ((" + a0str + "," + a1str +
    "),(" + a2str + "," + a3str + "),(" + a4str + "," + a5str + ")) Accessing
    element t." + idx1str + "." + idx2str + " = " + resstr, TestFile(teststr,
    res));
    }

    for (int i = 0; i < numIter; i++)
    {
      int array[3][2] = {{rand() % 100, rand() % 100}, {rand() % 100, rand() %
    100}, {rand() % 100, rand() % 100}}; int idx1 = rand() % 3, idx2 = rand() %
    2, idx3 = rand() % 3, idx4 = rand() % 2; int elem1 = array[idx1][idx2],
    elem2 = array[idx3][idx4], res = elem1 + elem2; std::string a0str =
    std::to_string(array[0][0]), a1str = std::to_string(array[0][1]), a2str =
    std::to_string(array[1][0]), a3str = std::to_string(array[1][1]), a4str =
    std::to_string(array[2][0]), a5str = std::to_string(array[2][1]), idx1str =
    std::to_string(idx1), idx2str = std::to_string(idx2), idx3str =
    std::to_string(idx3), idx4str = std::to_string(idx4), resstr =
    std::to_string(res);

      std::string teststr = std::string("fn main() { a := ((")
                          + a0str + ","
                          + a1str + "),("
                          + a2str + ","
                          + a3str + "),("
                          + a4str + ","
                          + a5str + "));"
                          + "a." + idx1str + "." + idx2str + " + a." + idx3str +
    "." + idx4str + ";};";

      result &= Test(out,
                    "Allocating a recursive Tuple ((" + a0str + "," + a1str +
    "),(" + a2str + "," + a3str + "),(" + a4str + "," + a5str + ")) Adding two
    elements, t." + idx1str + "." + idx2str + " + t." + idx3str + "." + idx4str
    + " = " + resstr, TestFile(teststr, res));
    }

    for (int i = 0; i < numIter; i++)
    {
      int num1 = rand() % 50, num2 = rand() % 50;
      int cmp1 = rand() % 2,  cmp2 = rand() % 2;
      int res = cmp1 == cmp2 ? num1 : num2;

      std::string teststr = std::string("fn main() { x := ")
                          + std::to_string(cmp1)
                          + "; y := "
                          + std::to_string(cmp2)
                          + "; z := if x == y then {"
                          + std::to_string(num1)
                          + ";} else {"
                          + std::to_string(num2)
                          + ";}; z;};";

      result &= Test(out,
                    "Conditional expression, (if " + std::to_string(cmp1) + " ==
    " + std::to_string(cmp2) + " then " + std::to_string(num1) + " else " +
    std::to_string(num2) + ") = " + std::to_string(res), TestFile(teststr,
    res));

    }

    for (int z = 0; z < numIter; z++)
    {
      int e = rand() % 50, f = rand() % 50, i = rand() % 50, j = rand() % 50;
      int a = rand() % 2,  b = rand() % 2,  c = rand() % 2,  d = rand() % 2,  g
    = rand() % 2,  h = rand() % 2; int res = 0;

      if (a == b)
      {
        if (c == d)
        {
          res = e;
        }
        else
        {
          res = f;
        }
      }
      else
      {
        if (g == h)
        {
          res = i;
        }
        else
        {
          res = j;
        }
      }

      std::string teststr = std::string("fn main() { a := ") + std::to_string(a)
                                                + "; b := "  + std::to_string(b)
                                                + "; c := "  + std::to_string(c)
                                                + "; d := "  + std::to_string(d)
                                                + "; e := "  + std::to_string(e)
                                                + "; f := "  + std::to_string(f)
                                                + "; g := "  + std::to_string(g)
                                                + "; h := "  + std::to_string(h)
                                                + "; i := "  + std::to_string(i)
                                                + "; j := "  + std::to_string(j)
                                                + "; l := 0; if a == b then { if
    c == d then { l = e; } else { l = f; }; } else { if g == h then { l = i; }
    else { l = j; }; }; l;};";

      result &= Test(out,
                    "Nested conditional expression: (if "
                      + std::to_string(a)
                      + " == "
                      + std::to_string(b)
                      + " then { if "
                      + std::to_string(c)
                      + " == "
                      + std::to_string(d)
                      + " then { "
                      + std::to_string(e)
                      + " } else { "
                      + std::to_string(f)
                      + " } } else { if "
                      + std::to_string(g)
                      + " == "
                      + std::to_string(h)
                      + " then { "
                      + std::to_string(i)
                      + " } else { "
                      + std::to_string(j)
                      + " } }) = "
                      + std::to_string(res),
                    TestFile(teststr, res)
                   );


    }

    for (int i = 0; i < numIter; i++)
    {
      int num1 = rand() % 50;
      int res = 50 - num1;
      std::string num1str = std::to_string(num1), resstr = std::to_string(res);

      std::string teststr = std::string("fn main() { a := ")
                          + num1str + "; b := 0; while a < 50 do { a = a + 1; b
    = b + 1; }; b;};"; result &= Test(out, "While loop: a := " + num1str + "
    while a < 50 do { a = a + 1; b = b + 1; }, b == " + resstr,
                    TestFile(teststr, res));
    }


    for (int i = 0; i < numIter; i++)
    {
      int num1 = rand() % 50, num2 = rand() % 50; // x, y | x + y <= 100
      std::string num1str = std::to_string(num1), num2str =
    std::to_string(num2); std::string resstr  = std::to_string(num1 + num2);

      result &= Test(
        out,
        "Application of a simple Function, (\\=> x). x (" + num1str + ") + y ("
    + num2str + ") = " + resstr, TestFile( std::string("fn num(){") + num1str +
    ";};\nfn main(){ num() + " + num2str + ";};\n", num1 + num2)
        );
    }

    for (int i = 0; i < numIter; i++)
    {
      int num1 = rand() % 50, num2 = rand() % 50; // x, y | x + y <= 100
      std::string num1str = std::to_string(num1), num2str =
    std::to_string(num2); std::string resstr  = std::to_string(num1 + num2);

      result &= Test(
        out,
        "Application of an addition Function, (\\x,y => x + y). x (" + num1str +
    ") + y (" + num2str + ") = " + resstr, TestFile( std::string("fn
    add(x:Int,y:Int){x+y;};\nfn main(){add(") + num1str + "," + num2str +
    ");};\n", num1 + num2)
        );
    }

    for (int i = 0; i < numIter; i++)
    {
      int num1 = rand() % 50, num2 = rand() % 50;
      std::string num1str = std::to_string(num1), num2str =
    std::to_string(num2); std::string resstr  = std::to_string(num1 + num2);

      result &= Test(
        out,
        "Application of an addition function taking pointers to integers,
    (\\x:Int*,y:Int* => *x + *y). x (" + num1str + ") + y (" + num2str + ") = "
    + resstr, TestFile( std::string("fn add(x: Int*, y: Int*){ *x + *y;};\nfn
    main(){ a := 0; b := 0; x := &a; y := &b; a = ")
            + num1str
            + "; b = "
            + num2str
            + "; c := add(x, y);};\n",
          num1 + num2)
        );
    }

    for (int i = 0; i < numIter; i++)
    {
      int num1 = rand() % 11, num2 = rand() % 11; // x, y | x * y <= 100
      std::string num1str = std::to_string(num1), num2str =
    std::to_string(num2); std::string resstr  = std::to_string(num1 * num2);

      result &= Test(
        out,
        "Application of a multiplication Function, (\\x,y => x * y). x (" +
    num1str + ") * y (" + num2str + ") = " + resstr, TestFile( std::string("fn
    mult(x:Int,y:Int){x*y;};\nfn main(){mult(") + num1str + "," + num2str +
    ");};\n", num1 * num2)
        );
    }

    for (int i = 0; i < numIter; i++)
    {
      int num1 = rand() % 100, num2 = (rand() % 99) + 1; // x, y | x * y <= 100
      std::string num1str = std::to_string(num1), num2str =
    std::to_string(num2); std::string resstr  = std::to_string(num1 / num2);

      result &= Test(
        out,
        "Application of a Division Function, (\\x,y => x / y). x (" + num1str +
    ") / y (" + num2str + ") = " + resstr, TestFile( std::string("fn
    Div(x:Int,y:Int){x/y;};\nfn main(){Div(") + num1str + "," + num2str +
    ");};\n", num1 / num2)
        );
    }

    for (int i = 0; i < numIter; i++)
    {
      int num1 = rand() % 100, num2 = rand() % 100; // x, y | x * y <= 100
      std::string num1str = std::to_string(num1), num2str =
    std::to_string(num2); std::string resstr  = std::to_string(num1 % num2);

      result &= Test(
        out,
        "Application of a Modulus Function, (\\x,y => x % y). x (" + num1str +
    ") % y (" + num2str + ") = " + resstr, TestFile( std::string("fn
    Mod(x:Int,y:Int){x%y;};\nfn main(){Mod(") + num1str + "," + num2str +
    ");};\n", num1 % num2)
        );
    }
    */
  result &= Test(out, "pink::Ast::Codegen", result);
  out << "\n---------------------------------\n";
  return result;
}
