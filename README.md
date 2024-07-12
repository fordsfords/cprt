# cprt
A set of definitions to allow easier development of code that is portable
between Windows and Linux (and sometimes Solaris).

# Table of contents

<!-- mdtoc-start -->
&bull; [cprt](#cprt)  
&bull; [Table of contents](#table-of-contents)  
&bull; [INTRODUCTION](#introduction)  
&bull; [APIs](#apis)  
&nbsp;&nbsp;&nbsp;&nbsp;&bull; [CPRT_GETTIME](#cprt_gettime)  
&nbsp;&nbsp;&nbsp;&nbsp;&bull; [cprt_getopt](#cprt_getopt)  
&bull; [GNU Extensions](#gnu-extensions)  
&bull; [License](#license)  
<!-- TOC created by '/home/sford/bin/mdtoc.pl README.md' (see https://github.com/fordsfords/mdtoc) -->
<!-- mdtoc-end -->

# INTRODUCTION

A set of definitions to allow easier development of code that is portable
between Windows and Linux (and usually Solaris and Mac).

# APIs

Use these instead of the OS-specific versions:

* CPRT_BASENAME
* CPRT_PERRNO
* CPRT_EOK0
* CPRT_ENULL
* CPRT_ASSERT
* CPRT_ABORT
* CPRT_ATOI - use instead of atoi(), see https://blog.geeky-boy.com/2014/04/strtoul-preferred-over-atoi.html
* CPRT_STRDEF - see https://stackoverflow.com/questions/25410690
* CPRT_VOL32 - see http://blog.geeky-boy.com/2014/06/clangllvm-optimize-o3-understands-free.html
* CPRT_NET_START - use before doing any network-related functions.
* CPRT_NET_CLEANUP - use after finished doing network-related functions.
* CPRT_SNPRINTF - use instead of snprintf() / _snprintf()
* CPRT_STRDUP - use instead of strdup() / _strdup()
* CPRT_SLEEP_SEC - use instead of sleep() / Sleep()
* CPRT_SLEEP_MS - use instead of usleep() / Sleep()
* CPRT_GETTIME, CPRT_INITTIME, cprt_timeval - use instead of clock_gettime() / QueryPerformanceCounter()
* CPRT_STRTOK
* CPRT_ATOMIC_INC_VAL, CPRT_ATOMIC_DEC_VAL, CPRT_ATOMIC_EXCHANGE
* CPRT_MUTEX_T, CPRT_MUTEX_INIT, CPRT_MUTEX_INIT_RECURSIVE, CPRT_MUTEX_LOCK, CPRT_MUTEX_TRYLOCK, CPRT_MUTEX_UNLOCK, CPRT_MUTEX_DELETE
* CPRT_SPIN_T, CPRT_SPIN_INIT, CPRT_SPIN_LOCK, CPRT_SPIN_TRYLOCK, CPRT_SPIN_UNLOCK, CPRT_SPIN_DELETE
* CPRT_SEM_T, CPRT_SEM_INIT, CPRT_SEM_DELETE, CPRT_SEM_POST, CPRT_SEM_WAIT
* CPRT_THREAD_T, CPRT_THREAD_ENTRYPOINT, CPRT_THREAD_CREATE, CPRT_THREAD_EXIT, CPRT_THREAD_JOIN
* CPRT_AFFINITY_MASK_T, CPRT_SET_AFFINITY
* CPRT_TIMEOFDAY, cprt_timeval - equiv of gettimeofday
* CPRT_LOCALTIME_R - equiv of localtime_r
* cprt_getopt, cprt_optarg, cprt_optopt, cprt_optind, cprt_opterr -
portable getopt.
See [cprt_getopt](#cprt_getopt).

## CPRT_GETTIME

I wanted a portable nanosecond-resolution timer API.
Unix has clock_gettime() and Windows has QueryPerformanceCounter().
That only has 100ns resolution, but it's the best you can get without
using the rdtsc instruction directly.

Most of the performance work I do is on Linux,
so I decided to make CPRT_GETTIME most efficient on Unixes,
where it simply translates to a direct call to clock_gettime().

On Windows it CPRT_GETTIME calls a C function in cprt.c that calls
QueryPerformanceCounter() and makes the result look like clock_gettime().
This is less efficient since it requires some math, which means that the
CPRT_GETTIME function affects the execution time being measured more
than is necessary.
But since QueryPerformanceCounter() only resolves to 100ns,
that extra execution time doesn't show up in the measurements.

## cprt_getopt

I wanted a public domain (CC0) version of getopt.
All the open source versions I could find have some kind of attribution
requirement, and I want there to be NO barriers to using this code.
So I wrote my own from scratch.

It is used the same as regular getopt() except all the symbols are prefixed
with "cprt_".
Thus, the API consists of:
* cprt_getopt() - callable function.
* cprt_optind - index of next argv.
* cprt_opterr - set to zero to suppress printing errors.
* cprt_optarg - points to option value.
* cprt_optopt - if cprt_getopt() returns '?',
this gives the option character that produced the error.

The cprt_getopt() API provides a subset of the behaviors of a "full"
getopt() that is typically provided by the OS's runtime library.
But there is no universal agreement on the exact behaviors of getopt().
BSD is different from GNU is different from POSIX is different from...

Part of the reason for the variation is that many ancient Unix programs
were developed without a standard getopt() parser,
and instead just parsed their command lines with their own custom code.
This has led to different programs supporting different behaviors.
Multiply that by the number of different Unixes out there and you get a
basic agreement on how options are parsed, but disagreement on some details.

This could cause a problem if a bunch of shell scripts were written on
one Unix is tried on a different Unix - command invocations that work on
one don't work on another.
For example, see [Using sed "in place" (gnu vs bsd)](http://blog.geeky-boy.com/2020/11/using-sed-in-place-gnu-vs-bsd.html).

So anyway, when I write a program,
I don't feel the need to be fully consistent with every other program,
so I don't mind supporting a subset of use patterns.
For example, long options are not supported.

To illustrate some supported use patterns,
here is the usage string for the `cprt_test` program:

````
Usage: cprt_test [-h] [-t testnum] [unused_arg]
````

Here are some valid ways to use it:
* `cprt_test -t 10 -h xyz` - xyz is the unused_arg.
* `cprt_test -h -t10 xyz` - can eliminate space between -t and it value.
* `cprt_test -t10 -- -x` - can have "-x" as the unused_arg, but need to use
"--" to end option processing.

Here is an invalid way to use it:
* `cprt_test -ht 10` - some option parsers allow multiple options to be supplied
together.
For those that require values, subsequent argvs are used to satisfy them.
The `cprt_getopt()` does not support this.
Each option must be in its own argv.

A few other misc features typical of getopt():
* By default, `cprt_getopt()` prints any errors to standard error.
If you want to suppress those prints,
change the global cprt_opterr to 0.
* If you want to re-scan the command line,
you can just set `cprt_optind` to 1 before your next call to `cprt_getopt()`.
* You can also change the globals `argv` and `argc` to parse other
tokenized input.
Again, set `cprt_optind` to 1.

# GNU Extensions

When writing code for Linux, you can choose between two overall sets of
runtime library APIs: portable or GNU-specific.
GNU-specific code generally has the line:
````c
#define _GNU_SOURCE
````
at the top of the source file, preceided only by whitespace and/or
comments.

To [quote Rich Felker](https://stackoverflow.com/a/5583764):
> If you define _GNU_SOURCE, you will get:
> 
> 1. access to lots of nonstandard GNU/Linux extension functions
> 2. access to traditional functions which were omitted from the POSIX standard (often for good reason, such as being replaced with better alternatives, or being tied to particular legacy implementations)
> 3. access to low-level functions that cannot be portable, but that you sometimes need for implementing system utilities like mount, ifconfig, etc.
> 4. broken behavior for lots of POSIX-specified functions, where the GNU folks disagreed with the standards committee on how the functions should behave and decided to do their own thing.

The whole point of cprt is to provide a common method of accessing
functionality that is generally not portable across platforms.
It's not necessarily that programs using cprt should be written portably,
just that they can use non-portable functionality in a common way.

This means that cprt should be usable by programmers who define _GNU_SOURCE
in their source files, and by programmers who do not.

This becomes problematic for certain corner cases where cprt uses an API
that changes behavior between _GNU_SOURCE and not _GNU_SOURCE.
For example, according to https://linux.die.net/man/3/strerror_r

> int strerror_r(int errnum, char *buf, size_t buflen); /* XSI-compliant */
>
> char *strerror_r(int errnum, char *buf, size_t buflen); /* GNU-specific */
>
> ... It returns the error string in the user-supplied buffer buf of length buflen.
>
> ... The GNU-specific strerror_r() returns a pointer to a string containing the error message. This may be either a pointer to a string that the function stores in buf, or a pointer to some (immutable) static string (in which case buf is unused).

I.e. if you write code that calls strerror_r(), you *must* know which version
you are getting.

This is why I need a ".c" file to go with cprt.

My original cprt was fully-contained within a ".h" file,
but I ran into problems where I needed _GNU_SOURCE for setting CPU affinity,
but I did not want cprt to require users to compile their programs with
_GNU_SOURCE.

So I had to move some cprt functionality into a C module and explicitly set
_GNU_SOURCE.
So long as cprt [is reasonably careful](https://stackoverflow.com/a/44199427),
"cprt.c" can safely be linked with code compiled without _GNU_SOURCE.

# License

I want there to be NO barriers to using this code, so I am releasing it to the public domain.  But "public domain" does not have an internationally agreed upon definition, so I use CC0:

Copyright 2021-2022 Steven Ford http://geeky-boy.com and licensed
"public domain" style under
[CC0](http://creativecommons.org/publicdomain/zero/1.0/):
![CC0](https://licensebuttons.net/p/zero/1.0/88x31.png "CC0")

To the extent possible under law, the contributors to this project have
waived all copyright and related or neighboring rights to this work.
In other words, you can use this code for any purpose without any
restrictions.  This work is published from: United States.  The project home
is https://github.com/fordsfords/cprt

To contact me, Steve Ford, project owner, you can find my email address
at http://geeky-boy.com.  Can't see it?  Keep looking.
