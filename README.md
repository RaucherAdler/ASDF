# Adler's Simple Data Format

### About 
This is a library I have written to serialize data in a simple and user-friendly manner.\
The syntax isn't too far from that of JSON and other serialization formats.


### Compile From Source
The library is ANSI C compliant, it should compile under most any standard compliant compiler.\
There's a (GNU) Makefile included, run `make` to build the library, and `make install` to automatically install it to your library directory (/usr/lib by default), and the header file to the corresponding include directory (/usr/include) - these directories may be changed by setting the PREFIX variable (ex: `make install PREFIX=/usr/local` to install to /usr/local/lib and /usr/local/include). The library may be uninstalled with `make uninstall` - specify PREFIX if you did so when installing.\
Note: `make install` and `make uninstall` weren't written with Windows in mind and likely won't work outside some Cygwin-like environment.


### Usage
The library may be linked via `-lasdf` and the header file included with `#include <asdf/asdf.h>`. If you installed it to a directory besides the one your compiler uses, use the `-L` and `-I` options.\
If you're looking to use this library in a project, I recommend reading through the header file as that's really the best source of documentation as of now.\
The library internally uses a Linked List structure which could potentially be a bit unwieldy, but there's a set of wrapper functions that abstract away from this and some utility functions to operate on these lists.


### Syntax
The syntax is rather simple - scalar values (of type string or integer) can be assigned as follows:
```
key = "this is a string"
int = 86278
```
Arrays are also supported and follow the following syntax:
```
[key
    "a value"
    263789
]
```
Indentation does not matter given its ASCII tabs or spaces, though there should be no trailing whitespace in array key names.\
It should be noted that I haven't tested this library with non-ASCII text and can't guarantee it will work.


### TO-DO
As of now, I have a number of features that I may potentially add in the future:
- Floating Point Integers
- Associative Arrays
- Inline Arrays
- Variable Referencing
- UTF-8 Support\
I plan to largely rewrite this library eventually as there's a number of things I am dissatisfied with - to add the features listed above, to fix some syntactic issues, among a host of other issues I have presently.


### Contact Me
Email: raucheradler@gmail.com\
Discord: Raucher Adler#2085\
Git: [GitLab](https://gitgud.io/RaucherAdler) | [GitHub](https://www.github.com/RaucherAdler)
