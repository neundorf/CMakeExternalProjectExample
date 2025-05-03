#include "foo.h"

#include <iostream>

Foo::Foo(int _i)
:m_i(_i)
{
}


void Foo::print()
{
  std::cout << "Hello " << m_i << ", build type = "<< CFG << std::endl;
}

