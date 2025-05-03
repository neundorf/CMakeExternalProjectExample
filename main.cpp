#include <iostream>

#include <foo/foo.h>

int main()
{
   std::cout << "Hello world\n";
   Foo foo(123);
   foo.print();
   return 0;
}
