# Taki

Taki is a very bad but pretty simple scripting language inspired heavily by
the earlier versions of Tcl except that everything in Taki is awful.  It
could definitely be a lot better with some work, but since it's already
meeting its dubious requirements, that seems unlikely!

In the unlikely event that you want to add more functionality to it or
embed it in another program, it shouldn't be too hard.

# Invoking Taki

The `taki` interpreter can either run a Taki script file, or can be run
interactively.  To run a file, simply use the filename as the first
commandline argument.  To run in interactive mode, pass `-i` as the first
argument.  Additional commandline arguments are made available to Taki
(see the example program).

In interactive mode, you can enter multiple lines by ending each one with
a backslash.  Quit with Ctrl-D.

# Example

Here's a simple example program:
```
func excite n {
  set r ""
  while {[> $n 0]} {
    decr n
    set r [. $r !]
  }
  return $r
}

print "Hello, World"
println [excite 3]

if [> $argc 1] {
  println "\nCommandline arguments:"
  set i 1
  while {[< $i $argc]} {
    print "  $i - "
    println [eval "\$argv$i"]
    incr i
  }
}  { # else
  println "(No commandline arguments.)"
}
```
