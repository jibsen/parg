
[![Build Status](https://travis-ci.org/jibsen/parg.svg?branch=master)](https://travis-ci.org/jibsen/parg) [![codecov.io](http://codecov.io/github/jibsen/parg/coverage.svg?branch=master)](http://codecov.io/github/jibsen/parg?branch=master)

About
-----

Most command-line programs have to parse options, so there are a lot of
different solutions to this problem. Some offer many features, while others
are more basic.

One of the simpler solutions for C is the [getopt][] function, and its
extension `getopt_long`. They iterate over the options in `argv`, returning
them one at a time on successive calls.

One nice thing about them is that they are available on most Unix-like
operating systems (and usually accompany GCC elsewhere, like Windows).
Unfortunately, some implementation details vary between platforms.

A potential question is what license the version you get when you include
them is available under. Some are GPL, others LGPL. There are also ports of
`getopt` that use more liberal licenses.

`parg` is a parser for `argv` that works similarly to `getopt`, but does not
aim to be a direct replacement. It attempts to make some choices about how to
handle the extensions and idiosyncrasies of other `getopt` implementations,
and document them.

It consists of a single source and include file, written in portable ANSI C.
It is made available under [CC0][].

[getopt]: https://en.wikipedia.org/wiki/Getopt
[CC0]: http://creativecommons.org/publicdomain/zero/1.0/


Usage
-----

The include file `parg.h` contains documentation in the form of [doxygen][]
comments. A configuration file is included, so you can simply run `doxygen`
to generate documentation in HTML format.

`parg` uses [CMake][] to generate build systems. To create one for the tools
on your platform, and build `parg`, use something along the lines of:

~~~sh
mkdir build
cd build
cmake ..
cmake --build .
~~~

You can also simply compile the source files and link them into your project.
CMake just provides an easy way to build and test across various platforms and
toolsets.

[doxygen]: http://www.doxygen.org/
[CMake]: http://www.cmake.org/


Example
-------

Here is an example that parses command-line options using `parg_getopt()`:

~~~c
#include <stdio.h>
#include <stdlib.h>

#include "parg.h"

int main(int argc, char *argv[])
{
	struct parg_state ps;
	int c;

	parg_init(&ps);

	while ((c = parg_getopt(&ps, argc, argv, "hs:v")) != -1) {
		switch (c) {
		case 1:
			printf("nonoption '%s'\n", ps.optarg);
			break;
		case 'h':
			printf("Usage: testparg [-h] [-v] [-s STRING]\n");
			return EXIT_SUCCESS;
			break;
		case 's':
			printf("option -s with argument '%s'\n", ps.optarg);
			break;
		case 'v':
			printf("testparg 1.0.0\n");
			return EXIT_SUCCESS;
			break;
		case '?':
			if (ps.optopt == 's') {
				printf("option -s requires an argument\n");
			}
			else {
				printf("unknown option -%c\n", ps.optopt);
			}
			return EXIT_FAILURE;
			break;
		default:
			printf("error: unhandled option -%c\n", c);
			return EXIT_FAILURE;
			break;
		}
	}

	for (c = ps.optind; c < argc; ++c) {
		printf("nonoption '%s'\n", argv[c]);
	}

	return EXIT_SUCCESS;
}
~~~


Comparison to `getopt`
----------------------

### Use of global variables

`getopt` uses global variables to store its state between calls. `parg` uses
a struct `parg_state`, which you must pass with each call.

### Handling of nonoptions

POSIX and BSD `getopt` return `-1` on the first nonoption argument. GNU
`getopt` by default reorders `argv` (even though it is passed as const), so
all options come first.

`parg` does not change `argv`, and returns each nonoption as the option
argument of an option with value `1` (like GNU `getopt`, if `optstring` were
prefixed by '`-`').

If you wish to process all options first, and have the nonoptions ordered at
the end of `argv`, you can use `parg_reorder()`:

~~~c
	optend = parg_reorder(argc, argv, optstring, NULL);

	while ((c = parg_getopt(&ps, optend, argv, optstring)) != -1) {
		/* ... */
	}

	/* elements of argv[] from optend to argc are nonoptions */
~~~

### Value of `optind` on error

When there are multiple short options in one argument, `getopt` does not
increment `optind` until the last one is processed. This makes it harder to
tell which argument an unknown option came from (if `a` is an unknown option,
`-a` and `-ab` will return '`?`' with different values in `optind`).

`parg` always increments the `optind` value in it's state so it points to the
next `argv` element to be processed. So when `parg` returns '`?`' (or '`:`'),
the element that contains the error is `argv[optind - 1]`.

### Value of `optopt` on error

With `getopt_long`, it varies what the values of `optopt` and `longindex` are
when an error is found with option arguments of long options. Sometimes these
values are not documented.

`parg` sets `optopt` to `val` if `flag` is `NULL`, and `0` otherwise (which
equals the return value on successful match), and `longindex` is set to the
index of the entry in `longopts` that matched.

### Return value on option argument error

When the first character of `optstring` is '`:`', it varies what `getopt`
returns on extraneous option arguments.

In this case, `parg` returns '`?`' if no option match is found, and '`:`' if
a match is found, but is missing a required argument, or has an extraneous
argument.


Alternatives
------------

Some ports of `getopt`:

  - [Free Getopt](http://freegetopt.sourceforge.net/)
  - [ya_getopt](http://github.com/kubo/ya_getopt/)
  - [getopt_port](http://github.com/kimgr/getopt_port/)

Other command-line parsing libraries that support C:

  - [Gengetopt](http://www.gnu.org/software/gengetopt/)
  - [Argp](http://www.gnu.org/software/libc/manual/html_node/Argp.html)
  - [popt](http://en.wikipedia.org/wiki/Popt)
  - [argtable](http://argtable.sourceforge.net/)
  - [optlist](http://michael.dipperstein.com/optlist/)
  - [Arg_parser](http://www.nongnu.org/arg-parser/arg_parser.html)
  - [Gopt](http://www.purposeful.co.uk/software/gopt/)
  - [docopt](http://docopt.org/)

A few C++ command-line parsing libraries:

  - [TCLAP](http://tclap.sourceforge.net/)
  - [program_options](http://www.boost.org/doc/libs/1_58_0/doc/html/program_options.html)
  - [CommandLine](http://llvm.org/docs/CommandLine.html)
