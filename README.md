# Adler's Simple Data Format

### About 
This is a library I have written to simply serialize data in a quick and convenient manner.\
The syntax isn't too far from that of JSON and other serialization formats.


### Compile From Source
The library is ANSI C compliant and doesn't use any non-standard libraries or features, it should compile under most any standard compliant compiler.\
There's a Makefile included, run `make` and it'll compile to a shared object in the current directory.


### Usage
The shared library, when placed in a project's directory, can be linked to it via the options `-L. -lasdf` or can be placed the path where your libraries are stored (along with the header file in its appropriate directory.)\
If you're looking to use this library in a project, I recommend reading through the header file as that's really the best source of documentation as of now.\
The library internally uses a Linked List structure which could potentially be a bit unwieldy, but there's a set of wrapper functions that abstract away from this (at least for any scalar operations) and some functions to operate on these lists.


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
It should be noted that indentation doesn't matter given its ASCII tabs or spaces, though there should be no trailing spaces in array key names.


### TO-DO
As of now, I have a number of features that I may potentially add in the future:
- Floating Point Integers
- Associative Arrays
- Inline Arrays

Beyond these additional features to the ASDF syntax, the structure and functionality of the library may end up changing in the future as there's a number of other things I'm dissatisfied with currently.


### Contact Me
You can find me here:

Email: raucheradler@gmail.com

Discord: Raucher Adler#2085

Git: [GitLab](https://gitgud.io/RaucherAdler) | [GitHub](https://www.github.com/RaucherAdler)
