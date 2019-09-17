#include "taki.h"

#include <stdio.h>
#include <iostream>

int main (int argc, char * argv[])
{
  if (argc < 2)
  {
    std::cout << "Put the name of a Taki script on the commandline.\n";
    return 1;
  }
  FILE * f = fopen(argv[1], "r");
  fseek(f, 0, SEEK_END);
  int length = ftell(f);
  fseek(f, 0, SEEK_SET);
  std::string s("\0", length);
  fread((char *)s.c_str(), 1, length, f); // Bad!
  fclose(f);

  Taki t;

  t.global->set_var("argc", std::to_string(argc-1));
  for (int i = 1; i < argc; i++)
  {
    t.global->set_var(std::string("argv") + std::to_string(i-1), argv[i]);
  }

  try
  {
    t.run(s);
  }
  catch (const char * s)
  {
    std::cout << "Error: " << s << std::endl;
  }
}
