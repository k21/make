make
====

This project implements a subset of the make program functionality. It supports
among else dependencies between targets, macro expansion, automatic variables
and parallel job execution.

COMPILATION
-----------

To compile the project simply run the `make` command in its root directory.
It has no dependencies apart from an ANSI C compiler and a POSIX compliant
standard library.

The project is self-hosting, meaning that the `make` executable created as
a result of the compilation can be used to read the project's makefile and
it can build itself.

Many modern systems support sub-second file modification time precision, which
can be useful in certain situations. This project however does not use it by
default, because it is not supported on all systems. If you want to use it,
use one of the following commands:

    CFLAGS='-DSTAT_MTIME_NSEC=st_mtim.tv_nsec' make
    CFLAGS='-DSTAT_MTIME_NSEC=st_mtimespec.tv_nsec' make

The first command is tested to work on Linux, the second one on Solaris,
OpenBSD and OS X.

COMMAND LINE USAGE
------------------

The program accepts the following optional switches:

 * `-C` *dir*

   Change current directory to *dir* before doing anything else. This option
   can appear multiple times. In that case, all directory changes are done in
   the order in which they appear on the command line.

 * `-f` *makefile*

   Read the rules from *makefile*. If this option is not used, the default name
   `Makefile` is used. This option can be used at most once.

 * `-j` *jobs*

   This option specifies how many jobs can be executed in parallel at the same
   time. If it is not specified, the default value of `1` is used. If it is
   passed more than once, the last occurrence takes precedence.

The parameters passed after the switches are assumed to be the names of the
targets that should be built. If there are no such parameters, the first target
found in the makefile is built.

The return value of the program is `0` upon successful completion or a nonzero
number in case of error.

MAKEFILE SYNTAX
---------------

### Comments ###

Any occurrence of `#` character in a makefile signifies the beginning of a
comment. The comment ends at the next unescaped new-line character (unescaped
means that it is not preceded by a backslash character).

### Escaped line breaks ###

If a line in the file ends with a backslash, it is considered to continue on
the next line. If this happens outside of a command list then
the backslash, the newline and any number
of blank characters at the beginning of the next line are replaced with a single
space. Otherwise the backslash and the newline are kept
and if the next line starts with a tabulator character, it is removed.

### Macros ###

A makefile can contain rules and macro definitions. A macro definition looks
like this:

    NAME = value

This assigns the contents `value` to a macro named `NAME`. The spaces or other
blank characters around the equals sign are optional and ignored. After its
definition, macro can be used in the following forms:

    $(NAME)
    ${NAME}
    $N

The last form can only be used for macros whose name consists of a single
letter. `$$` always expands to a single dollar sign.

A macro can be used in a value of another macro, in which case it is evaluated
after that macro is used and evaluated, or in a rule outside of a command list,
in which case it is evaluated when the rule is read and before it is further
processed, or in a command list, in which case it is evaluated just before the
command is executed.

If a macro expansion is done in a command list, automatic variables are
available. This is the list of supported automatic variables:

 * `$@` - the name of the current target
 * `$<` - the name of the first dependency
 * `$^` - a list of all dependencies of the current target separated by spaces
 * `$?` - a list of dependencies of the current target that are newer than
          the target separated by spaces
 * `$+` - like `$^`, but keeps the exact order and number of occurrences of the
          dependencies from the makefile

### Rules ###

Rules consist of a target dependency specification and a command list. They have
the following syntax:

    target1 target2: dependency1 dependency2
    	command1
    	command2

There has to be one or more targets, zero or more dependencies, and zero or more
commands. Dependencies are names of other targets or files that have to be built
or exist before the current target can be built.

The commands associated with the rule have to start with a tabulator character.

When a target is being built, each command is (after macro expansion)
first printed to the standard output and then interpreted by `/bin/sh`. If the
exit code of the shell is nonzero, it is considered an error and `make`
aborts its processing.

To change the default behavior described above, the following prefixes can be
used:

 * `@`

   If this prefix is used, the command is not printed to the standard output
   before it is executed.

 * `-`

   This tells `make` to ignore the return value of the launched process and
   continue building even if it is nonzero.

 * `+`

   This prefix is ignored for compatibility reasons.

Those prefixes are removed from the commands before their further processing.

PARALLEL EXECUTION
------------------

If the `-j` option is used, more than one job can be run at the same time.
If there are multiple commands provided to build a single target, those are
guaranteed to be processed in order. All target's dependencies also have to be
built before the target's commands can start executing. That means that the
number of simultaneously running jobs can be lower that the number specified
with `-j` if the dependencies in the makefile require so.

Please note that if there are no dependencies between two targets and `-j` is
used, there is no guarantee that any of those two targets will be built before
the other starts being built. They can even be built at the same time. That
requires certain caution when writing makefiles to make sure that independent
targets do not overwrite each others files and do not use resources in a
conflicting manner.

UNIMPLEMENTED FEATURES
----------------------

This is a list of features that have not been implemented so far. They might be
implemented in the future.

 * Many command line switches
 * Implicit rules
 * Wild card and inference rules
 * Macro substitutions and other macro processing functions
 * `:=`, `::=`, `+=` and other assignment operators
 * Working with archives
 * Order-only dependencies
 * Special targets
   (e. g. `.DEFAULT`, `.IGNORE`, `.PRECIOUS`, `.SILENT`, `.PHONY`)
 * Probably other features
