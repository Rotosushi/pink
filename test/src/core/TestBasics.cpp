
#include <unistd.h>
#include <sys/wait.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <fstream>
#include <algorithm>

#include "Test.h"
#include "core/TestBasics.h"

#include "aux/Error.h"



// this function is nearly identical to system(),
// except it runs the given process directly, 
// and does not invoke the shell to do it.
// #PORTABILITY: This function is not portable.
//    The functions which allow you to spawn new 
//    processes are not the same accross OS's, and
//    it is for this reason that this section of 
//    tests might be better written in a more cross 
//    platform language like python.
int Run(const char* file, char** argv)
{
  int result = 0;

  pid_t pid = fork();

  if (pid < 0)
  {
    pink::FatalError(std::string("A call to fork failed: ") + strerror(errno), __FILE__, __LINE__);
  }
  else if (pid == 0) // we are executing the child process
  {
    execv(file, argv);
    exit(1); // exec never returns.
  }
  else // we are executing the parent process
  {
    int status = 0;
    waitpid(pid, &status, 0);
    result = WEXITSTATUS(status); // we can only retreive the least significant 8 bits 
                                  // of the exit status of the executed
                                  // program. and there is no way to get a full
                                  // integer without setting up a signal
                                  // handler to catch the exit code when
                                  // SIGCHLD is raised when the child process
                                  // exits.
  }

  return result;
}


std::string StripFilenameExtensions(std::string filename)
{
  auto it = std::find(filename.begin(), filename.end(), '.');
  return std::string(filename.begin(), it);
}

/*  
 * This function wraps the core test actions for a file within a function,  
 * Since we are testing the execution of the compiler itself, we are going 
 * to have to run the compiler multiple times against multiple files to 
 * test multiple things. the general process is as follows:
 *  1: emit the test_contents into a file
 *  2: compile that file into an executable
 *  3: run that executable and check the exit code matches expected_value.
 *
 */
bool TestFile(std::string test_contents, int expected_value)
{
  bool result = true;

  std::string filename = "test.p";

  std::fstream outfile;

  outfile.open(filename, std::ios_base::in | std::ios_base::out | std::ios_base::trunc);

  if (!outfile.is_open())
  {
    pink::FatalError("Failed to open file: " + filename, __FILE__, __LINE__);
  }

  outfile << test_contents;
  
  outfile.close();

  char pink[] = "./pink";

  char* pink_args[] = {
    pink,
    filename.data(),
    (char*) nullptr
  };
  
  Run(pink, pink_args);


  std::string exe_name = StripFilenameExtensions(filename) + ".exe";

  char* exe_args[] = {
    exe_name.data(),
    (char*) nullptr
  };

  int prog_result = Run(exe_name.data(), exe_args);
  
  std::string obj_name = StripFilenameExtensions(filename) + ".o";

  std::remove(obj_name.data());
  std::remove(filename.data());
  std::remove(exe_name.data());

  if (prog_result != expected_value)
  {
    result = false;
  }

  return result;
}


/*
 *  TestBasic tests that the compiler can emit a correct program,
 *    given the input file. The file is not the most complex
 *    and only really tests that the main subroutine can be converted 
 *    into something that the OS can run, and returns a known value.
 *
 *  
 *
 *
 *
 */
bool TestBasics(std::ostream& out)
{
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
  out << "Testing pink basic core functionality: \n";

  srand(time(0));

  for (int i = 0; i < 10; i++)
  {
    int num = rand() % 100; // random number between 0 and 100
    std::string numstr = std::to_string(num);
    result &= Test(
        out, 
        "Simple main [" + numstr + "]",
        TestFile(
          std::string("fn main () { ") + numstr + "; };\n",
          num
        )
    );
  }

  for (int i = 0; i < 10; i++)
  {
    int num1 = rand() % 100;
    std::string num1str = std::to_string(num1);

    result &= Test(
          out,
          "Bind: x := " + num1str,
          TestFile(
              std::string("fn main() { x := ") + num1str + ";x;};\n",
              num1
            )
        );
  }

  for (int i = 0; i < 10; i++)
  {
    int num1 = rand() % 100;
    std::string num1str = std::to_string(num1);

    result &= Test(
          out,
          "Compound Bind: x := y := " + num1str,
          TestFile(
              std::string("fn main() { x := y := ") + num1str + ";x;};\n",
              num1
            )
        );
  }
  
  for (int i = 0; i < 10; i++)
  {
    int num1 = rand() % 100;
    std::string num1str = std::to_string(num1);

    result &= Test(
          out,
          "Assignment: x = " + num1str,
          TestFile(
              std::string("fn main() { x := 0; x =") + num1str + ";x;};\n",
              num1
            )
        );
  }
  
  for (int i = 0; i < 10; i++)
  {
    int num1 = rand() % 100;
    std::string num1str = std::to_string(num1);

    result &= Test(
          out,
          "Compound Assignment: x = y = " + num1str,
          TestFile(
              std::string("fn main() { x := y := 0; x = y = " + num1str + "; x == y;};\n"),
              true
            )
        );
  }
  
  for (int i = 0; i < 10; i++)
  {
    int num1 = rand() % 50, num2 = rand() % 50; // x, y | x + y <= 100
    std::string num1str = std::to_string(num1);
    std::string num2str = std::to_string(num2);
    std::string resstr  = std::to_string(num1 + num2);

    result &= Test(
      out,
      "Addition: x = (" + num1str + "); y = (" + num2str + "); x + y = " + resstr,
      TestFile(
        std::string("fn main () { x := 0; y := 0; c := 0; x = ") + num1str + "; y = " + num2str + "; c = x + y; c;};\n",
        num1 + num2
        ) 
    );
  }

  for (int i = 0; i < 10; i++)
  {
    int num1 = rand() % 50;
    int res  = num1 + num1;
    std::string num1str = std::to_string(num1), resstr = std::to_string(res);

    result &= Test(
        out,
        "Assignment to modified self: x = " + num1str + ", x + x = " + resstr,
        TestFile(
          std::string("fn main () { x := ") + num1str + "; x = x + x; x;};\n",
          res
          )
        );
  }
  
  for (int i = 0; i < 10; i++)
  {
    int num1 = rand() % 11, num2 = rand() % 11; // x, y | x * y <= 100
    std::string num1str = std::to_string(num1);
    std::string num2str = std::to_string(num2);
    std::string resstr  = std::to_string(num1 * num2);

    result &= Test(
      out,
      "Multiplication: x = (" + num1str + "); y = (" + num2str + "); x * y = " + resstr,
      TestFile(
        std::string("fn main () { x := y := c := 0; x = ") + num1str + "; y = " + num2str + "; c = x * y; c; };\n",
        num1 * num2
        ) 
    );
  }
  
  for (int i = 0; i < 10; i++)
  {
    int num1 = rand() % 100, num2 = (rand() % 99) + 1; // x, y | x / y <= 100
    std::string num1str = std::to_string(num1);
    std::string num2str = std::to_string(num2);
    std::string resstr  = std::to_string(num1 / num2);

    result &= Test(
        out,
        "Division: x (" + num1str + ") / y (" + num2str + ") = " + resstr,
        TestFile(
          std::string("fn main(){ x := y := c := 0; x = ") + num1str + "; y = " + num2str + "; c = x / y; c; };\n",
          num1 / num2
          )
        );
  }

  for (int i = 0; i < 10; i++)
  {
    int num1 = rand() % 100, num2 = rand() % 100; // x, y | x / y <= 100
    std::string num1str = std::to_string(num1);
    std::string num2str = std::to_string(num2);
    std::string resstr  = std::to_string(num1 % num2);

    result &= Test(
        out,
        "Modulus: x (" + num1str + ") % y (" + num2str + ") = " + resstr,
        TestFile(
          std::string("fn main() { x := y := c := 0; x = ") + num1str + "; y = " + num2str + "; c = x % y; c; };\n",
          num1 % num2
          )
        );
  }

  for (int i = 0; i < 10; i++)
  {
    int num1 = rand() % 100, num2 = rand() % 100; // x, y | x / y <= 100
    std::string num1str = std::to_string(num1);
    std::string num2str = std::to_string(num2);
    std::string resstr  = std::to_string(num1 == num2);

    result &= Test(
        out,
        "Integer Equality: x (" + num1str + ") == y (" + num2str + ") = " + resstr,
        TestFile(
          std::string("fn main(){ x := y := 0; c := false; x = ") + num1str + "; y = " + num2str + "; c = x == y; c; };\n",
          num1 == num2
          )
        );
  }

  for (int i = 0; i < 10; i++)
  {
    int num1 = rand() % 100, num2 = rand() % 100; // x, y | x / y <= 100
    std::string num1str = std::to_string(num1);
    std::string num2str = std::to_string(num2);
    std::string resstr  = std::to_string(num1 != num2);

    result &= Test(
        out,
        "Integer Inequality: x (" + num1str + ") != y (" + num2str + ") = " + resstr,
        TestFile(
          std::string("fn main(){ x := y := 0; c := false; x = ") + num1str + "; y = " + num2str + "; c = x != y; c; };\n",
          num1 != num2
          )
        );
  }

  for (int i = 0; i < 10; i++)
  {
    int num1 = rand() % 100, num2 = rand() % 100; // x, y | x / y <= 100
    std::string num1str = std::to_string(num1);
    std::string num2str = std::to_string(num2);
    std::string resstr  = std::to_string(num1 < num2);

    result &= Test(
        out,
        "Integer less than: x (" + num1str + ") < y (" + num2str + ") = " + resstr,
        TestFile(
          std::string("fn main(){ x := y := 0; c := false; x = ") + num1str + "; y = " + num2str + "; c = x < y; c; };\n",
          num1 < num2
          )
        );
  }

  for (int i = 0; i < 10; i++)
  {
    int num1 = rand() % 100, num2 = rand() % 100; // x, y | x / y <= 100
    std::string num1str = std::to_string(num1);
    std::string num2str = std::to_string(num2);
    std::string resstr  = std::to_string(num1 <= num2);

    result &= Test(
        out,
        "Integer less than or equal: x (" + num1str + ") <= y (" + num2str + ") = " + resstr,
        TestFile(
          std::string("fn main(){ x := y := 0; c := false; x = ") + num1str + "; y = " + num2str + "; c = x <= y; c; };\n",
          num1 <= num2
          )
        );
  }

  for (int i = 0; i < 10; i++)
  {
    int num1 = rand() % 100, num2 = rand() % 100; // x, y | x / y <= 100
    std::string num1str = std::to_string(num1);
    std::string num2str = std::to_string(num2);
    std::string resstr  = std::to_string(num1 > num2);

    result &= Test(
        out,
        "Integer greater than: x (" + num1str + ") > y (" + num2str + ") = " + resstr,
        TestFile(
          std::string("fn main(){ x := y := 0; c := false; x = ") + num1str + "; y = " + num2str + "; c = x > y; c; };\n",
          num1 > num2
          )
        );
  }

  for (int i = 0; i < 10; i++)
  {
    int num1 = rand() % 100, num2 = rand() % 100; // x, y | x / y <= 100
    std::string num1str = std::to_string(num1);
    std::string num2str = std::to_string(num2);
    std::string resstr  = std::to_string(num1 >= num2);

    result &= Test(
        out,
        "Integer greater than or equal: x (" + num1str + ") >= y (" + num2str + ") = " + resstr,
        TestFile(
          std::string("fn main(){ x := y := 0; c := false; x = ") + num1str + "; y = " + num2str + "; c = x >= y; c; };\n",
          num1 >= num2
          )
        );
  }

  for (int i = 0; i < 10; i++)
  {
    bool b1 = rand() % 2, b2 = rand() % 2;
    std::string b1str = b1 == 0 ? "false" : "true";
    std::string b2str = b2 == 0 ? "false" : "true";
    std::string resstr  = b1 || b2 ? "true" : "false";
    
    result &= Test(
        out,
        "Boolean Or: x (" + b1str + ") | y (" + b2str + ") = " + resstr,
        TestFile(
         std::string("fn main(){") + b1str + " | " + b2str + ";};\n",
         b1 | b2
         )
        ); 
  }
  
  for (int i = 0; i < 10; i++)
  {
    bool b1 = rand() % 2, b2 = rand() % 2;
    std::string b1str = b1 == 0 ? "false" : "true";
    std::string b2str = b2 == 0 ? "false" : "true";
    std::string resstr  = b1 && b2 ? "true" : "false";
    
    result &= Test(
        out,
        "Boolean And: x (" + b1str + ") & y (" + b2str + ") = " + resstr,
        TestFile(
         std::string("fn main(){") + b1str + " & " + b2str + ";};\n",
         b1 && b2
         )
        ); 
  }
  
  for (int i = 0; i < 10; i++)
  {
    bool b1 = rand() % 2, b2 = rand() % 2;

    std::string b1str = b1 == 0 ? "false" : "true";
    std::string b2str = b2 == 0 ? "false" : "true";
    std::string resstr  = b1 == b2 ? "true" : "false";
    
    result &= Test(
        out,
        "Booleans Equality: x (" + b1str + ") == y (" + b2str + ") = " + resstr,
        TestFile(
         std::string("fn main(){") + b1str + " == " + b2str + ";};\n",
         b1 == b2
         )
        ); 
  }

  for (int i = 0; i < 10; i++)
  {
    int num1 = rand() % 50, num2 = rand() % 50; // x, y | x + y <= 100
    std::string num1str = std::to_string(num1);
    std::string num2str = std::to_string(num2);

    result &= Test(
      out,
      "Create a Pointer, Assign through a Pointer: x = (" + num1str + "); y = &x; y = " + num2str ,
      TestFile(
        std::string("fn main () { x := 0; y := &x; x = ") + num1str + "; *y = " + num2str + "; x;};\n",
        num2
        ) 
    );
  }

  for (int i = 0; i < 10; i++)
  {
    int num1 = rand() % 50, num2 = rand() % 50; // x, y | x + y <= 100
    std::string num1str = std::to_string(num1);
    std::string num2str = std::to_string(num2);
    std::string resstr  = std::to_string(num1 + num2);

    result &= Test(
      out,
      "Arithmetic through a Pointer: x = (" + num1str + "); y = (" + num2str + "); x + y = " + resstr ,
      TestFile(
        std::string("fn main () { a := b := c := 0; x := &a; y := &b; z := &c; *x = ") 
          + num1str 
          + "; *y = " 
          + num2str 
          + "; *z = *x + *y; c;};\n",
        num1 + num2
        ) 
    );
  }

  for (int i = 0; i < 10; i++)
  {
    int num1 = rand() % 2, num2 = rand() % 2; // x, y | x + y <= 100
    std::string num1str = num1 == 0 ? "false" : "true"; 
    std::string num2str = num2 == 0 ? "false" : "true";
    std::string resstr  = num1 == num2 ? "true" : "false";

    result &= Test(
      out,
      "Boolean arithmetic through a pointer: x = (" + num1str + "); y = (" + num2str + "); x == y = " + resstr ,
      TestFile(
        std::string("fn main () { a := b := c := false; x := &a; y := &b; z := &c; *x = ") 
          + num1str 
          + "; *y = " 
          + num2str 
          + "; *z = *x == *y; c;};\n",
        num1 == num2
        ) 
    );
  }

  for (int i = 0; i < 10; i++)
  {
    int array[5] = {rand() % 50, rand() % 50, rand() % 50, rand() % 50, rand() % 50};
    int idx1 = rand() % 5, idx2 = rand() % 5;
    std::string idx1str = std::to_string(idx1), 
                idx2str = std::to_string(idx2);
    std::string num1str = std::to_string(array[idx1]), 
                num2str = std::to_string(array[idx2]),
                resstr  = std::to_string(array[idx1] + array[idx2]);
    std::string arr0str = std::to_string(array[0]),
                arr1str = std::to_string(array[1]),
                arr2str = std::to_string(array[2]),
                arr3str = std::to_string(array[3]),
                arr4str = std::to_string(array[4]);
    
    std::string teststr = "fn main() { a := [" + arr0str + "," + arr1str + "," + arr2str + "," + arr3str + "," + arr4str + "];"
                                    + "b := *(a + " + idx1str + ");"
                                    + "c := *(a + " + idx2str + ");"
                                    + "b + c;};";
    result &= Test(out,
            "Allocating an Integer array, and performing arithmetic on two of it's elements: " + num1str + " + " + num2str + " = " + resstr,
            TestFile(teststr, array[idx1] + array[idx2])
            );

  }
  
  for (int i = 0; i < 10; i++)
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
                                    + "b := a + " + idx1str + "; *b = " + num1str + ";"
                                    + "c := a + " + idx2str + "; *c = " + num2str + ";"
                                    + "*b + *c;};";
    result &= Test(out,
            "Allocating an Integer array, Assigning new numbers to two of it's elements, " + idx1str + ", " + idx2str + ", and performing arithmetic those two elements: " + num1str + " + " + num2str + " = " + resstr,
            TestFile(teststr, num1 + num2)
            );

  }

  for (int i = 0; i < 10; i++)
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
                  "Conditional expression, (if " + std::to_string(cmp1) + " == " + std::to_string(cmp2) + " then " + std::to_string(num1) + " else " + std::to_string(num2) + ") = " + std::to_string(res),
                  TestFile(teststr, res));

  }

  for (int z = 0; z < 10; z++)
  {
    int e = rand() % 50, f = rand() % 50, i = rand() % 50, j = rand() % 50;
    int a = rand() % 2,  b = rand() % 2,  c = rand() % 2,  d = rand() % 2,  g = rand() % 2,  h = rand() % 2;
    int res = 0;
    
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
                                              + "; l := if a == b then { if c == d then { e; } else { f; }; } else { if g == h then { i; } else { j; }; }; l;};";
    
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
  
  for (int i = 0; i < 10; i++)
  {
    int num1 = rand() % 50, num2 = rand() % 50; // x, y | x + y <= 100
    std::string num1str = std::to_string(num1), num2str = std::to_string(num2);
    std::string resstr  = std::to_string(num1 + num2);

    result &= Test(
      out,
      "Application of a simple Function, (\\=> x). x (" + num1str + ") + y (" + num2str + ") = " + resstr,
      TestFile(
        std::string("fn num(){") + num1str + ";};\nfn main(){ num() + " + num2str + ";};\n",
        num1 + num2)
      ); 
  }
  
  for (int i = 0; i < 10; i++)
  {
    int num1 = rand() % 50, num2 = rand() % 50; // x, y | x + y <= 100
    std::string num1str = std::to_string(num1), num2str = std::to_string(num2);
    std::string resstr  = std::to_string(num1 + num2);

    result &= Test(
      out,
      "Application of an addition Function, (\\x,y => x + y). x (" + num1str + ") + y (" + num2str + ") = " + resstr,
      TestFile(
        std::string("fn add(x:Int,y:Int){x+y;};\nfn main(){add(") + num1str + "," + num2str + ");};\n",
        num1 + num2)
      ); 
  }

  for (int i = 0; i < 10; i++)
  {
    int num1 = rand() % 50, num2 = rand() % 50;
    std::string num1str = std::to_string(num1), num2str = std::to_string(num2);
    std::string resstr  = std::to_string(num1 + num2);

    result &= Test(
      out,
      "Application of an addition function taking pointers to integers, (\\x:Int*,y:Int* => *x + *y). x (" + num1str + ") + y (" + num2str + ") = " + resstr,
      TestFile(
        std::string("fn add(x: Int*, y: Int*){ *x + *y;};\nfn main(){ a := 0; b := 0; x := &a; y := &b; a = ") 
          + num1str 
          + "; b = " 
          + num2str 
          + "; c := add(x, y);};\n",
        num1 + num2)
      );
  }
  
  for (int i = 0; i < 10; i++)
  {
    int num1 = rand() % 11, num2 = rand() % 11; // x, y | x * y <= 100
    std::string num1str = std::to_string(num1), num2str = std::to_string(num2);
    std::string resstr  = std::to_string(num1 * num2);

    result &= Test(
      out,
      "Application of a multiplication Function, (\\x,y => x * y). x (" + num1str + ") * y (" + num2str + ") = " + resstr,
      TestFile(
        std::string("fn mult(x:Int,y:Int){x*y;};\nfn main(){mult(") + num1str + "," + num2str + ");};\n",
        num1 * num2)
      ); 
  }

  for (int i = 0; i < 10; i++)
  {
    int num1 = rand() % 100, num2 = (rand() % 99) + 1; // x, y | x * y <= 100
    std::string num1str = std::to_string(num1), num2str = std::to_string(num2);
    std::string resstr  = std::to_string(num1 / num2);

    result &= Test(
      out,
      "Application of a Division Function, (\\x,y => x / y). x (" + num1str + ") / y (" + num2str + ") = " + resstr,
      TestFile(
        std::string("fn Div(x:Int,y:Int){x/y;};\nfn main(){Div(") + num1str + "," + num2str + ");};\n",
        num1 / num2)
      ); 
  }
  
  for (int i = 0; i < 10; i++)
  {
    int num1 = rand() % 100, num2 = rand() % 100; // x, y | x * y <= 100
    std::string num1str = std::to_string(num1), num2str = std::to_string(num2);
    std::string resstr  = std::to_string(num1 % num2);

    result &= Test(
      out,
      "Application of a Modulus Function, (\\x,y => x % y). x (" + num1str + ") % y (" + num2str + ") = " + resstr,
      TestFile(
        std::string("fn Mod(x:Int,y:Int){x%y;};\nfn main(){Mod(") + num1str + "," + num2str + ");};\n",
        num1 % num2)
      ); 
  }
  
  result &= Test(out, "basic core functionality", result);
  out << "\n---------------------------------\n";
  return result;
}


