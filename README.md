# cprt
A set of definitions to allow easier development of code that is portable
between Mac, Windows, and Linux (and sometimes Solaris).

# APIs

Use these instead of the OS-specific versions:

* CPRT_NET_START - use before doing any network-related functions.
* CPRT_NET_CLEANUP - use after finished doing network-related functions.
* CPRT_BASENAME
* CPRT_PERR
* CPRT_EOK0
* CPRT_ENULL
* CPRT_ABORT
* CPRT_ASSERT
* CPRT_STRDEF - see https://stackoverflow.com/questions/25410690
* CPRT_VOL32 - see http://blog.geeky-boy.com/2014/06/clangllvm-optimize-o3-understands-free.html
* CPRT_SNPRINTF
* CPRT_STRDUP
* CPRT_SLEEP_SEC
* CPRT_SLEEP_MS
* CPRT_STRTOK_PORT
* CPRT_MUTEX_T
* CPRT_MUTEX_INIT
* CPRT_MUTEX_INIT_RECURSIVE
* CPRT_MUTEX_LOCK
* CPRT_MUTEX_TRYLOCK
* CPRT_MUTEX_UNLOCK
* CPRT_MUTEX_DELETE
* CCPRT_SEM_T
* CPRT_SEM_INIT
* CPRT_SEM_DELETE
* CPRT_SEM_POST
* CPRT_SEM_WAIT
* CPRT_THREAD_T
* CPRT_THREAD_ENTRYPOINT
* CPRT_THREAD_CREATE
* CPRT_THREAD_EXIT
* CPRT_THREAD_JOIN

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
