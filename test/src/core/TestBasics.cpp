
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
// 

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
    int num1 = rand() % 50, num2 = rand() % 50; // x, y | x + y <= 100
    std::string num1str = std::to_string(num1);
    std::string num2str = std::to_string(num2);
    std::string resstr  = std::to_string(num1 + num2);

    result &= Test(
      out,
      "Addition: x (" + num1str + ") + y (" + num2str + ") = " + resstr,
      TestFile(
        std::string("fn main () { ") + num1str + " + " + num2str + "; };\n",
        num1 + num2
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
      "Multiplication: x (" + num1str + ") * y (" + num2str + ") = " + resstr,
      TestFile(
        std::string("fn main () { ") + num1str + " * " + num2str + "; };\n",
        num1 * num2
        ) 
    );
  }

  for (int i = 0; i < 10; i++)
  {
    int num1 = rand() % 50, num2 = rand() % 50; // x, y | x + y <= 100
    std::string num1str = std::to_string(num1), num2str = std::to_string(num2);
    std::string resstr  = std::to_string(num1 + num2);

    result &= Test(
      out,
      "Application of a Function, (\\x,y => x + y). x (" + num1str + ") + y (" + num2str + ") = " + resstr,
      TestFile(
        std::string("fn add(x:Int,y:Int){x+y;};\nfn main(){add(") + num1str + "," + num2str + ");};\n",
        num1 + num2)
      ); 
  }
  

  result &= Test(out, "basic core functionality", result);
  out << "\n---------------------------------\n";
  return result;
}


