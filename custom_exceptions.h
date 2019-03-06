#include <exception>

using namespace std;

struct IOException : public exception
{
  const char * what () const throw ()
  {
    return "C++ Exception";
  }
};