#include "taki.h"

#include <functional>
#include <iostream>


#define ARGS(N) if (args.size() != (N)+1) throw "Got wrong number of arguments";
#define MINARGS(N) if (args.size() < (N)+1) throw "Got too few arguments";
#define MAXARGS(N) if (args.size() > (N)+1) throw "Got too many arguments";


static std::string _join (std::string sep, std::vector<std::string> strs)
{
  if (strs.size() == 0) return "";
  if (strs.size() == 1) return strs[0];
  std::string r = strs[0];
  for (int i = 1; i < strs.size(); i++) r += sep + strs[i];
  return r;
}


static std::string _to_string (double d)
{
  if (d == (long long)d) return std::to_string((long long)d);
  return std::to_string(d);
}


static std::string _print (std::vector<std::string> args, Taki * taki, std::shared_ptr<TakiContext> ctxt)
{
  for (int i = 1; i < args.size(); i++)
  {
    if (i != 1) std::cout << " ";
    std::cout << args[i];
  }
  return "";
}

static std::string _println (std::vector<std::string> args, Taki * taki, std::shared_ptr<TakiContext> ctxt)
{
  _print(args, taki, ctxt);
  std::cout << std::endl;
  return "";
}

static std::string _strcat (std::vector<std::string> args, Taki * taki, std::shared_ptr<TakiContext> ctxt)
{
  std::string r;
  for (int i = 1; i < args.size(); i++)
  {
    r += args[i];
  }
  return r;
}

static std::string _strjoin (std::vector<std::string> args, Taki * taki, std::shared_ptr<TakiContext> ctxt)
{
  std::string r;
  MINARGS(1);
  for (int i = 2; i < args.size(); i++)
  {
    if (i != 2) r += args[1];
    r += args[i];
  }
  return r;
}

static std::string _int (std::vector<std::string> args, Taki * taki, std::shared_ptr<TakiContext> ctxt)
{
  std::string r;
  ARGS(1);
  return _to_string(std::stoi(args[1]));
}

static std::string _sysexit (std::vector<std::string> args, Taki * taki, std::shared_ptr<TakiContext> ctxt)
{
  std::string r;
  ARGS(1);
  exit(std::stoi(args[1]));
  return ""; // Unreachable
}

static std::string _set (std::vector<std::string> args, Taki * taki, std::shared_ptr<TakiContext> ctxt)
{
  ARGS(2);
  ctxt->set_var(args[1], args[2]);
  return "";
}

static std::string _sum (std::vector<std::string> args, Taki * taki, std::shared_ptr<TakiContext> ctxt)
{
  MINARGS(2);
  char * endptr;
  double r = std::strtod(args[1].c_str(), &endptr);

  for (int i = 2; i < args.size(); i++) r += std::strtod(args[i].c_str(), &endptr);

  return _to_string(r);
}

static std::string _while (std::vector<std::string> args, Taki * taki, std::shared_ptr<TakiContext> ctxt)
{
  ARGS(2);
  auto cctxt = ctxt->new_child();
  while (true)
  {
    auto r = taki->eval(cctxt, args[1]);
    if (r == "0" || r == "") break;

    taki->run(cctxt, args[2]);
  }
  return "";
}

static std::string _if (std::vector<std::string> args, Taki * taki, std::shared_ptr<TakiContext> ctxt)
{
  MINARGS(2);
  int i = 1;
  for (; i < args.size()-1; i+=2)
  {
    auto r = taki->eval(ctxt, args[i]);
    if (r == "0" || r == "") continue;

    // Got it!
    return taki->run(ctxt, args[i+1]);
  }
  if ((args.size() % 1) != 1)
  {
    // Else!
    return taki->run(ctxt, args[args.size()-1]);
  }
  return "";
}

static std::string _return (std::vector<std::string> args, Taki * taki, std::shared_ptr<TakiContext> ctxt)
{
  ARGS(1);
  throw ReturnException(args[1]);
}

static std::string _void (std::vector<std::string> args, Taki * taki, std::shared_ptr<TakiContext> ctxt)
{
  ARGS(1);
  return args[1];
}

static std::string _eval (std::vector<std::string> args, Taki * taki, std::shared_ptr<TakiContext> ctxt)
{
  ARGS(1);

  return taki->eval(ctxt, args[1]);
}

static std::string _func (std::vector<std::string> args, Taki * taki, std::shared_ptr<TakiContext> ctxt)
{
  MINARGS(2);

  std::string x = "{";
  for (int i = 2; i < args.size()-1; i++)
  {
    x += args[i];
    x += "}{";
  }
  x += args[args.size()-1] + "}";

  ctxt->set_var(args[1], x);
  return "";
}

#define MATHOP(N,O) \
static std::string N (std::vector<std::string> args, Taki * taki, std::shared_ptr<TakiContext> ctxt) \
{                                                                                                    \
  ARGS(2);                                                                                           \
  char * endptr;                                                                                     \
  double a1 = std::strtod(args[1].c_str(), &endptr);                                                 \
  double a2 = std::strtod(args[2].c_str(), &endptr);                                                 \
  return _to_string(a1 O a2);                                                                        \
}

#define IMATHOP(N,O) \
static std::string N (std::vector<std::string> args, Taki * taki, std::shared_ptr<TakiContext> ctxt) \
{                                                                                                    \
  ARGS(2);                                                                                           \
  char * endptr;                                                                                     \
  auto a1 = std::strtoll(args[1].c_str(), &endptr, 0);                                               \
  auto a2 = std::strtoll(args[2].c_str(), &endptr, 0);                                               \
  return _to_string(a1 O a2);                                                                        \
}

static std::string _decr (std::vector<std::string> args, Taki * taki, std::shared_ptr<TakiContext> ctxt)
{
  MINARGS(1);
  MAXARGS(2);
  char * endptr;
  double a1 = 1;
  if (args.size() == 3) a1 = std::strtod(args[2].c_str(), &endptr);
  auto v = std::strtod(ctxt->get_var(args[1]).c_str(), &endptr);
  v -= a1;
  auto r = _to_string(v);
  ctxt->set_var(args[1], r);
  return r;
}

static std::string _incr (std::vector<std::string> args, Taki * taki, std::shared_ptr<TakiContext> ctxt)
{
  MINARGS(1);
  MAXARGS(2);
  char * endptr;
  double a1 = 1;
  if (args.size() == 3) a1 = std::strtod(args[2].c_str(), &endptr);
  auto v = std::strtod(ctxt->get_var(args[1]).c_str(), &endptr);
  v += a1;
  auto r = _to_string(v);
  ctxt->set_var(args[1], r);
  return r;
}

MATHOP(_ne, !=)
MATHOP(_eq, ==)
MATHOP(_gt, >)
MATHOP(_ge, >=)
MATHOP(_lt, <)
MATHOP(_le, <=)
MATHOP(_sub, -)
MATHOP(_mul, *)
MATHOP(_div, /)

IMATHOP(_idiv, /)
IMATHOP(_mod, %)

IMATHOP(_shl, <<)
IMATHOP(_shr, >>)

IMATHOP(_lor, |)
IMATHOP(_land, &)


std::vector<std::string> Taki::lex (std::shared_ptr<TakiContext> ctxt, std::string code, int start, int & end)
{
  std::vector<std::string> out;
  std::string word;
  bool haveword = false;
  int i;
  bool initial = true;

  #define ACCEPT if (haveword) {haveword=false; out.push_back(word); word = ""; }

  for (i = start; ; i++)
  {
    end = i;
    switch (code[i])
    {
      case '#':
      {
        for (; ; i++)
        {
          end = i;
          if (code[i] == 0)
          {
            ACCEPT;
            return out;
          }
          else if (code[i] == '\n')
          {
            if (haveword || out.size())
            {
              ACCEPT;
              return out;
            }
            break;
          }
        }
      }
      case '}':
      case ']':
      case '\n':
      case '\0':
        ACCEPT;
        return out;
        break;
      case ' ':
      case '\t':
        //if (initial) break;
        ACCEPT;
        break;
      case '"':
        i++;
        for (; ; i++)
        {
          if (code[i] == '\\')
          {
            i++;
            switch (code[i])
            {
              case 'n':
                word += "\n";
                break;
              case '\\':
                word += "\\";
                break;
              default:
                word += code[i];
            }
          }
          else if (code[i] == '"')
          {
            haveword = true;
            ACCEPT;
            break;
          }
          else if (code[i] == '$')
          {
            i++;

            std::string v;
            for (; i <= code.size(); i++)
            {
              if (!std::isalnum(code[i])) break;
              v += code[i];
            }
            word += ctxt->get_var(v);
            haveword = true;
            i--;
          }
          else if (code[i] == 0)
          {
            throw "Expected closing quotation mark";
          }
          else
          {
            word += code[i];
          }
        }
        break;
      case '{':
      {
        i++;
        int braces = 1;
        haveword = true;
        for (; i <= code.size(); i++)
        {
          if (code[i] == '{')
          {
            braces++;
            word += code[i];
          }
          else if (code[i] == '}')
          {
            braces--;
            if (braces == 0)
            {
              break;
            }
            word += code[i];
          }
          else if (code[i] == 0)
          {
            throw "Expected '}'";
          }
          else
          {
            word += code[i];
          }
        }
        ACCEPT;
        break;
      }
      case '$':
      {
        i++;
        for (; i <= code.size(); i++)
        {
          if (!std::isalnum(code[i])) break;
          word += code[i];
        }

        word = ctxt->get_var(word);
        haveword = true;
        ACCEPT;
        i--;
        break;
      }
      case '[':
      {
        i++;
        auto cmd = lex(ctxt, code, i, i);
        if (code[i] != ']') throw "Expected ']'";
        if (cmd.size())
        {
          word = exec(ctxt, cmd);
          haveword = true;
          ACCEPT;
        }
        break;
      }
      default:
        haveword = true;
        word += code[i];
        break;
    }
  }

  return out;
}


std::string Taki::exec (std::shared_ptr<TakiContext> ctxt, std::vector<std::string> cmds)
{
  if (cmds.size() == 0) return "";
  auto it = builtins.find(cmds[0]);
  if (it != builtins.end())
  {
    return it->second(cmds, this, ctxt);
  }

  auto f = ctxt->get_var(cmds[0]);
  int endpos = 0;
  auto parts = lex(ctxt, f, endpos, endpos);

  if (f[endpos] != '\0') throw "Bad function definition";

  if (parts.size() != cmds.size()) throw "Wrong number of arguments for user function";

  auto nctxt = ctxt->new_child();
  for (int i = 0; i < parts.size() - 1; i++)
  {
    nctxt->set_var(parts[i], cmds[i+1]);
  }

  try
  {
    return run(nctxt, parts[parts.size()-1]);
  }
  catch (ReturnException e)
  {
    return e.rv;
  }
}


std::string Taki::eval (std::shared_ptr<TakiContext> ctxt, std::string code)
{
  int endpos = 0;
  auto cmds = lex(ctxt, code, 0, endpos);
  if (code[endpos] != 0) throw "Parsing ended prematurely";
  for (int i = 1; i < cmds.size(); i++) cmds[0] = cmds[0] + " " + cmds[i];
  return cmds[0];
}


std::string Taki::run (std::shared_ptr<TakiContext> ctxt, std::string code)
{
  int endpos = 0;
  std::string last;
  while (code[endpos])
  {
    auto cmds = lex(ctxt, code, endpos, endpos);

    last = exec(ctxt, cmds);
    if (code[endpos] == 0) break;
    endpos++;
  }

  return last;
}

Taki::Taki ()
: global(std::make_shared<TakiContext>())
{
  global->global = global; // global global global
  builtins["print"] = _print;
  builtins["println"] = _println;
  builtins["."] = _strcat;
  builtins["join"] = _strjoin;
  builtins["int"] = _int;
  builtins["set"] = _set;
  builtins["+"] = _sum;
  builtins["-"] = _sub;
  builtins["/"] = _div;
  builtins["//"] = _idiv;
  builtins["*"] = _mul;
  builtins["%"] = _mod;
  builtins["!="] = _ne;
  builtins["=="] = _eq;
  builtins["<"] = _lt;
  builtins[">"] = _gt;
  builtins[">="] = _ge;
  builtins["<="] = _le;
  builtins["<<"] = _shl;
  builtins[">>"] = _shr;
  builtins["|"] = _lor;
  builtins["&"] = _land;
  builtins["func"] = _func;
  builtins["return"] = _return;
  builtins["void"] = _void;
  builtins["eval"] = _eval;
  builtins["while"] = _while;
  builtins["if"] = _if;
  builtins["incr"] = _incr;
  builtins["decr"] = _decr;
  builtins["exit"] = _sysexit;
}
