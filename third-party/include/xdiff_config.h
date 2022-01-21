#if !defined(LIBXDIFF_CONFIG_H)
#define LIBXDIFF_CONFIG_H

/* Define to 1 if you have the <dlfcn.h> header file. */
/* #undef HAVE_DLFCN_H */

/* Define to 1 if you have the `free' function. */
#define HAVE_FREE

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H

/* Define to 1 if you have the <limits.h> header file. */
#define HAVE_LIMITS_H

/* Define to 1 if you have the `malloc' function. */
#define HAVE_MALLOC

/* Define to 1 if you have the `memchr' function. */
#define HAVE_MEMCHR

/* Define to 1 if you have the `memcmp' function. */
#define HAVE_MEMCMP

/* Define to 1 if you have the `memcpy' function. */
#define HAVE_MEMCPY

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H

/* Define to 1 if you have the `memset' function. */
#define HAVE_MEMSET

/* Define to 1 if you have the `realloc' function. */
#define HAVE_REALLOC

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H

/* Define to 1 if you have the <stdio.h> header file. */
#define HAVE_STDIO_H

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H

/* Define to 1 if you have the `strlen' function. */
#define HAVE_STRLEN

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H

/* Define to the full name of this package. */
#define PACKAGE_NAME ""

/* Define to the full name and version of this package. */
#define PACKAGE_STRING ""

/* Define to the version of this package. */
#define PACKAGE_VERSION ""

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

#if defined(_MSC_VER)
#   define XRABPLY_TYPE64 __int64
#   define XV64(v) ((xply_word) v ## UI64)
#endif /* _MSC_VER */

#endif /* LIBXDIFF_CONFIG_H */
