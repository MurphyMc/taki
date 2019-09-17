#include <string>
#include <vector>
#include <map>
#include <memory>

class ReturnException
{
public:
  std::string rv;
  ReturnException (std::string rv)
  : rv(rv)
  {
  }
};


class TakiContext : public std::enable_shared_from_this<TakiContext>
{
public:
  std::map<std::string, std::string> vars;
  std::shared_ptr<TakiContext> global;
  std::shared_ptr<TakiContext> parent;

  std::string get_var (std::string name)
  {
    auto it = vars.find(name);
    if (it == vars.end())
    {
      if (parent) return parent->get_var(name);
      if (global && global != shared_from_this()) return parent->get_var(name);
      throw "No such variable";
    }
    return it->second;
  }

  bool has_var (std::string name)
  {
    auto it = vars.find(name);
    if (it == vars.end()) return false;
    return true;
  }

  bool set_var (std::string name, std::string value, bool allow = true)
  {
    if (has_var(name))
    {
      vars[name] = value;
      return true;
    }
    if (parent)
    {
      if (parent->set_var(name, value, false)) return true;
    }
    if (allow)
    {
      vars[name] = value;
      return true;
    }
    return false;
  }

  std::shared_ptr<TakiContext> new_child ()
  {
    auto child = std::make_shared<TakiContext>();
    child->global = global;
    child->parent = shared_from_this();
    return child;
  }
};


class Taki
{
  std::map<std::string, std::function<std::string(std::vector<std::string>, Taki *, std::shared_ptr<TakiContext>)>> builtins;

  std::vector<std::string> lex (std::shared_ptr<TakiContext> ctxt, std::string code, int start, int & end);

public:
  std::shared_ptr<TakiContext> global;

  std::string exec (std::shared_ptr<TakiContext> ctxt, std::vector<std::string> cmds);
  std::string eval (std::shared_ptr<TakiContext> ctxt, std::string code);
  std::string run (std::shared_ptr<TakiContext> ctxt, std::string code);

  std::string run (std::string code) { return run(global, code); }
  Taki ();
};
