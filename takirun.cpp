#include "taki.h"

#include <stdio.h>
#include <iostream>

int main (int argc, char * argv[])
{
  if (argc < 2)
  {
    std::cout << "Put the name of a Taki script on the commandline or -i for interactive.\n";
    return 1;
  }

  // Set up interpreter and add commandline arg info to it
  Taki t;
  t.global->set_var("argc", std::to_string(argc-1));
  for (int i = 1; i < argc; i++)
  {
    t.global->set_var(std::string("argv") + std::to_string(i-1), argv[i]);
  }


  if (std::string("-i") == argv[1])
  {
    std::cout << "Taki interactive mode." << std::endl;
    std::cout << "To enter multiple lines, end a line with a backslash." << std::endl;
    while (std::cin)
    {
      std::string prog = "";
      std::cout << ">>> ";
      while (std::cin)
      {
        std::string line;
        std::getline(std::cin, line);
        if (!std::cin) break;
        if (line.size() && line[line.size()-1] == '\\')
        {
          line = line.substr(0, line.size()-1) + "\n";
          prog += line;
          std::cout << "... ";
          continue;
        }
        prog += line;
        break;
      }
      if (prog.size())
      {
        try
        {
          auto r = t.run(prog);
          if (r.length())
          {
            std::cout << "--> " << r << std::endl;
          }
        }
        catch (const char * s)
        {
          std::cout << "Error: " << s << std::endl;
        }
        catch (ReturnException e)
        {
          std::cout << "--> " << e.rv << std::endl;
        }
      }
    }
    std::cout << std::endl << "Bye!" << std::endl;
  }
  else
  {
    FILE * f = fopen(argv[1], "r");
    fseek(f, 0, SEEK_END);
    int length = ftell(f);
    fseek(f, 0, SEEK_SET);
    std::string s("\0", length);
    fread((char *)s.c_str(), 1, length, f); // Bad!
    fclose(f);

    try
    {
      t.run(s);
    }
    catch (const char * s)
    {
      std::cout << "Error: " << s << std::endl;
      return 1;
    }
  }

  return 0;
}
