# cprt
A set of definitions to allow easier development of code that is portable
between Mac, Windows, and Linux (and sometimes Solaris).

# APIs

Use these instead of the OS-specific versions:

* CPRT_BASENAME
* CPRT_PERRNO
* CPRT_EOK0
* CPRT_ENULL
* CPRT_ASSERT
* CPRT_ABORT
* CPRT_STRDEF - see https://stackoverflow.com/questions/25410690
* CPRT_VOL32 - see http://blog.geeky-boy.com/2014/06/clangllvm-optimize-o3-understands-free.html
* CPRT_NET_START - use before doing any network-related functions.
* CPRT_NET_CLEANUP - use after finished doing network-related functions.
* CPRT_SNPRINTF
* CPRT_STRDUP
* CPRT_SLEEP_SEC
* CPRT_SLEEP_MS
* CPRT_STRTOK
* CPRT_MUTEX_T, CPRT_MUTEX_INIT, CPRT_MUTEX_INIT_RECURSIVE, CPRT_MUTEX_LOCK, CPRT_MUTEX_TRYLOCK, CPRT_MUTEX_UNLOCK, CPRT_MUTEX_DELETE
* CCPRT_SEM_T, CPRT_SEM_INIT, CPRT_SEM_DELETE, CPRT_SEM_POST, CPRT_SEM_WAIT
* CPRT_THREAD_T, CPRT_THREAD_ENTRYPOINT, CPRT_THREAD_CREATE, CPRT_THREAD_EXIT, CPRT_THREAD_JOIN
* CPRT_AFFINITY_MASK_T, CPRT_SET_AFFINITY
* getopt, optarg, optopt, optind, opterr

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
> ...
> It returns the error string in the user-supplied buffer buf of length buflen.
> ...
> The GNU-specific strerror_r() returns a pointer to a string containing the error message. This may be either a pointer to a string that the function stores in buf, or a pointer to some (immutable) static string (in which case buf is unused).

I.e. if you write code that calls strerror_r(), you *must* know which version
you are getting.

This is why I need a ".c" file to go with cprt.
My original cprt was fully-contained within a ".h" file,
but I ran into problems where I needed _GNU_SOURCE for setting CPU affinity,
but I did not want cprt to require users to compile their programs with
_GNU_SOURCE.

## License

I want there to be NO barriers to using this code, so I am releasing it to the public domain.  But "public domain" does not have an internationally agreed upon definition, so I use CC0:

Copyright 2021 Steven Ford http://geeky-boy.com and licensed
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
