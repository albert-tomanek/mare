Mare
====

Mare (Make replacement) is a build automation tool similar to Make. It can be used to automatize the build process of software projects. Based on a set of build rules (or recipes), which specifies build commands and dependencies between source files and targets, Mare determines which files to compile in order to create a build target or to apply changes in source files. Unlike Make, Mare relies on a file format that was designed with the needs of modern software projects in mind.


Motivation
----------

Make is part of almost every build system for UNIX-like systems. It is simple, versatile and easy to use but it is fairly limited in its functionality. Hence in modern software projects, Make is mostly used in conjunction with a Makefile-generator (like automake, cmake, qmake, prebuild etc.). 

* The file system and version control systems like SVN or Git are extensively used in many software projects. Files are stored in directories and rules to compile source files can be derived from the path of the directory in which the files are stored. However, Make requires a list of source files that cannot be generated automatically without using any extensions or external command line tools like "find". Often these file lists are managed manually or generated by a Makefile-generator.

* In a Makefile the rules on how to compile files are specified without any conventions or additional information. The structure of the project can hence not be deduced from the Makefile alone, since some information like the purpose of a rule is missing. Other third party tools like IDEs maintain such information in separate files. This leads to non-centralized project configuration management that is unnecessarily complex.

* In many software projects, different configuration sets (e.g. for debuggable and for optimized code) are used. Although Make provides means to handle multiple configurations, using them can lead to badly structured Makefiles. Even the support for multiple configurations in some of the popular build systems is limited to some degree.

* While Makefile-generators add most of the functionality that is missing in Make, they compromise the simplicity of the build system. Many Makefile-generators were build to address a certain issue and the result is not always in favor of the developers of the software project that ends up using them.

How does Mare work?
-------------------

Mare is a small stand alone tool. Once executed in its working directory, it searches for a file with name "Marefile". This file specifies rules to compile the source files of a software project into build targets. Mare determines which targets to recreate by comparing the file modification timestamp of the source files and previously generated build targets. In case the build target is missing or older than one of its source files it is recreated by executing a build command as specified by the build rules. Instead of managing the build process directly, Mare can also be used to generate project files for other tools like Visual Studio, CodeBlocks, CodeLite, NetBeans, Make and cmake.

A Marefile consists of three lists: "configurations", "targets" and "platforms". "configurations" lists different build configurations (e.g. "Debug" for debuggable code and "Release" for optimized code). "targets" lists all the build targets (executables, libraries, etc.) of a software project. Each build target contains a list of source files, the rules to compile them and a rule to create the target. "platforms" is normally not used unless the target platform differs from the host platform.

Here is an example of a Marefile for a simple c++ application where all source files are stored in the directory "src":

```
configurations = { Debug, Relase }
targets = {
  myApplication = cppApplication + {
    files = {
      "src/**.cpp" = cppSource
    }
  }
}
```

Compiling Mare
--------------

### Windows

Compiling Mare on Windows requires Git and Visual Studio 2010 (or newer, e.g Visual Studio 2013 Express). You should start off with cloning https://github.com/craflin/mare.git to create your working copy of the source tree. Once your working copy is ready, navigate to its root directory and call "generate.bat --vcxproj=2010" (or 2012, or 2013). This will create an unoptimized build of Mare (Debug/mare.exe) and generate project files for Visual Studio of the given version. The project files can be opened in Visual Studio to create a build with optimized (Release) code.

### GNU/Linux

Git, a compiler compatible with g++ and Make are required to compile Mare on GNU/Linux (or another GNU/Linux-like operating system). Mare can then be cloned and compiled with:

```
$ cd /your/working/directory
$ git clone https://github.com/craflin/mare.git mare
$ cd mare
$ make
```

The generated executable (mare) will be located in the directory "Debug". It can be used to compile Mare without debug symbols:

```
$ cd /your/working/directory/mare
$ Debug/mare config=Release
```

The Marefile
------------

A Marefile contains nested associative lists where each key is a string. A value linked to a key can be another associative list. However, keys do not have to be linked to a subordinate list. An associate list can also be interpreted as a string by concatenating each key separated by a space character. Within an associative list, multiple subordinate associative lists can be defined by separating multiple keys with space characters. An associative list interpreted as string can therefore be inserted into a key. Associative lists can also be composed of other associative lists and different cases for multiple configurations can be handled when declaring a list.

A target declared in a Marefile (e.g. "Example1") consists of a list of input files, a list of output files and the build command that processes the input files in order to create the output files:

```
targets = {
  Example1 = {
    input = "file1.o file2.o"
    output = "Example1"
    command = "g++ file1.o file2.o -o Example1"
  }
}
```

In this example, the files listed as "input" and "output" files do also appear in the build command. To avoid this "input" and "output" can be inserted into the build command with the syntax "$(variable)":

```
targets = {
  Example1 = {
    input = "file1.o file2.o"
    output = "Example1"
    command = "g++ $(input) -o $(output)"
  }
}
```

The object files (intermediate targets) should be created from source files. Each target contains a list with the name "files", which describes rules to create them:

```
targets = {
  Example1 = {
    input = "file1.o file2.o"
    output = "Example1"
    command = "g++ $(input) -o $(output)"
    
    cppFlags = "-O3"
    
    files = {
      "file1.cpp" = {
        input = "file1.cpp"
        output = "file1.o"
        command = "g++ $(cppFlags) -c $(input) -o $(output)"
      }
      "file2.cpp" = {
        input = "file2.cpp"
        output = "file2.o"
        command = "g++ $(cppFlags) -c $(input) -o $(output)"
      }
    }
  }
}
```

The rules to compile "file1.cpp" and "file2.cpp" can be combined:

```
targets = {
  Example1 = {
    input = "file1.o file2.o"
    output = "Example1"
    command = "g++ $(input) -o $(output)"
    
    cppFlags = "-O3"
    
    files = {
      "file1.cpp file2.cpp" = {
        input = "$(file)"
        output = "$(patsubst %.cpp,%.o,$(file))"
        command = "g++ $(cppFlags) -c $(input) -o $(output)"
      }
    }
  }
}
```

To improve clearness, the rule to compile a .cpp file can be swapped out by declaring it next to the list of targets or list of files:

```
targets = {
  Example1 = {
    input = "file1.o file2.o"
    output = "Example1"
    command = "g++ $(input) -o $(output)"
    
    cppFlags = "-O3"
    
    files = {
      "file1.cpp file2.cpp" = myCppSource
    }
  }
}

myCppSource = {
  input = "$(file)"
  output = "$(patsubst %.cpp,%.o,$(file))"
  command = "g++ $(cppFlags) -c $(input) -o $(output)"
}
```

The list of object files ("file1.o file2.o") can be generated automatically from the list of source files and the name of the output file ("Example") can be derived from name of the target:

```
targets = {
  Example1 = {
    input = "$(patsubst %.cpp,%.o,$(files))"
    output = "$(target)"
    command = "g++ $(input) -o $(output)"
    
    cppFlags = "-O3"
    
    files = {
      "file1.cpp file2.cpp" = myCppSource
    }
  }
}
...
```

That way the rule to link the program can also be swapped out:

```
targets = {
  Example1 = myCppApplication + {
    cppFlags = "-O3"
    files = {
      "file1.cpp file2.cpp" = myCppSource
    }
  }
}

myCppSource = {
  input = "$(file)"
  output = "$(patsubst %.cpp,%.o,$(file))"
  command = "g++ $(cppFlags) -c $(input) -o $(output)"
}

myCppApplication = {
  input = "$(patsubst %.cpp,%.o,$(files))"
  output = "$(target)"
  command = "g++ $(input) -o $(output)"
}
```

The list of source files can be generated automatically using a list of files found in the file system that matches a wildcard pattern:

```
targets = {
  Example1 = myCppApplication + {
    cppFlags = "-O3"
    files = {
      "**.cpp" = myCppSource
    }
  }
}
...
```

When using swapped out lists the keys of these lists can be overwritten. For instance "output" could subsequently be overwritten if the name of the output file should be changed to "Example1.blah":

```
targets = {
  Example1 = myCppApplication + {
    output = "Example1.blah"
    cppFlags = "-O3"
    files = {
      "**.cpp" = myCppSource
    }
  }
}
...
```

There is set of build-in rules (cApplication, cppApplication, cDynamicLibrary, cppDynamicLibrary, cStaticLibrary, cppStaticLibrary, cSource und cppSource) available which allow creating simple c and c++ applications. (see section "Build-in Rules")

### Specialization

An "if &lt;expr&gt; &lt;statements&gt; [else &lt;statements&gt;]" expression within the declaration of a list allows customizing lists for certain configurations:

```
cppFlags = "-mmmx"
if configuration == "Release" {
  cppFlags += "-O3"
}
...
```

or

```
cppFlags = {
  "-mmmx"
  if configuration == "Release" { "-O3" }
}
...
```

or

```
cppFlags = "-mmmx -O3"
if configuration != "Release" {
  cppFlags -= "-O3"
}
...
```

or

```
cppFlags = {
  "-mmmx -O3"
  if configuration != "Release" { -"-O3" }
}
...
```

In the &lt;expr&gt; part of an if-statement, lists can be compared with another or with strings, using the operators ==, !=, &gt;, &lt;, &gt;= and &lt;=. Expressions can be enclosed in parenthesis and multiple expressions can be chained using boolean operators (&& and ||).

To differentiate between configurations and other environmental conditions, Mare provides the following "variables":
* "configuration" � the name of the configuration currently built (e.g. "Debug", "Release", ...) 
* "platform" � the name of the target platform (e.g. "Win32", "Linux", "MacOSX", ...) 
* "host" � the name of the host platform (e.g. "Win32", "Linux", "MacOSX", ...) 
* "tool" � the name of a translator (declared when the Marefile is translated into another format) (e.g. "vcxproj", "codelite", "codeblocks", "cmake", "netbeans") 
* "target" � the name of the target currently handled
* "architecture" � the architecture of the host system (e.g. "i686", "x86_64")

### Including Files

Instead of declaring a list, it is also possible to include a list from an external Marefile:

```
targets = {
  include "Example1.mare"
}
...
```

### File Name Wildcards

When wildcards are used in file names, the wildcard pattern will be replaced with a list of matching files found in the file system. For instance, the "**.cpp" pattern will be replaced with "file1.cpp file2.cpp" given these two files exist:

```
targets = {
  Example1 = myCppApplication + {
    files = {
      "**.cpp" = myCppSource
    }
  }
}
...
```

The wildcard pattern may contain the following placeholders:
* * - matches any string within the name of a file (e.g. "*.cpp" matches "ab.cpp", "bcd.cpp")
* ? - matches a single character within the name of a file (e.g. "a?.cpp" matches "ab.cpp", "ac.cpp" but not "aef.cpp") 
* ** - matches any string (including slashes) within the path of a file (e.g. "**.cpp" matches "aa.cpp", "bb.cpp", "subdir/bbws.cpp", "subdir/subdir/bassb.cpp") 

### Space Characters in Keys

The space character with in a key can be used to assign multiple keys at once. However, if a key should actually contain a space character (for instance for a file name that contains a space character), the whole string can be enclosed with escaped quotation marks:

```
myKey = "\"file name.txt\""
```

(This feature is highly experimental.)

### Commas and Semicolons

Each key declaration in a Marefile can be separated with optional commas or semicolons:

```
targets = {
  Example1 = myCppApplication + {
    files = {
      "*.cpp" = myCppSource;
    },;;,,
  },
};
...
```

### Variables

Lists can be used like variables and the keys of a list can be inserted into a string with the syntax $(variable). Environment variables are used in case a list with the given name cannot be found.

The environment variables of external tools called when executing a rule can be altered by environment variable declarations succeeding the command line:

```
command = "MYENV=hallo bash -c \"echo $$(MYENV)\""
```

### Functions

Within keys, a functions can be used with the syntax "$(function arguments)". The functions available in Mare are similar to the functions that can be used in a (GNU-)Makefile (see http://www.gnu.org/software/make/manual/make.html#Functions) but some of these are not yet implemented. For now, the following functions can be used:
* subst, patsubst, filter, filter-out, firstword, lastword, dir, notdir, suffix, basename, addsuffix, addprefix, if, foreach, origin 

Additionally, Mare introduces some new functions:
* lower � transforms a string into lower case letters ("$(lower AbC)" becomes "abc")
* upper � transforms a string into upper case letters ("$(upper dDd)" becomes "DDD")
* readfile � inserts the content of plain text file (e.g. "$(readfile anyfile.d)") 

### Build-in Rules

Mare provides a set of build-in rules, which can be used for simple c and c++ applications, dynamic libraries and static libraries. The Translators (see section "Translators") interpret them accordingly to convert a Marefile as close to the target environment as possible.
* cppSource, cSource - rules for c/cpp source files 
* cppApplication, cApplication - rules for c/cpp executables
* cppDynamicLibrary, cDynamicLibrary � rules for c/cpp DLLs or "shared objects" 
* cppStaticLibrary, cStaticLibrary � rules for static c/cpp libraries 

These rules can be customized by overwriting or extending the following lists:
* linker � the program used to link a c/cpp application or DLL/shared object (default is "gcc" for cApplication or cDynamicLibrary, "g++" for cppApplication or cppDynamicLibrary and "ar" for cppStaticLibrary or cStaticLibrary) 
* linkFlags, libPaths, libs - flags passed to the linker
* cCompiler, cppCompiler � the compiler used to compile c/cpp files (default is "gcc" for cApplication, cDynamicLibrary or cStaticLibrary and "g++" for cppApplication, cppDynamicLibrary or cppStaticLibrary)
* cFlags, cppFlags, defines, includePaths � flags passed to the compiler
* buildDir � the directory used for intermediate files (default is "$(configuration)") 

A simple Marefile like

```
targets = {
  Example1 = cppApplication + {
    defines = { "NDEBUG" }
    libs = { "jpeg" }
    includePaths = { "anypath1", "anypath2" }
    files = {
      "*.cpp" = cppSource
    }
  }
}
```

can be handled by Mare directly or translated into project files for Visual Studio and build files for Make or cmake. The other translators (for CodeLite, CodeBlocks, NetBeans) are not yet advanced enough to support the build-in rules properly. However, these IDEs allow using Mare as an external build system. The translators for these IDEs interpret the keys "buildCommand", "reBuildCommand" and "cleanCommand" within a target specification as commands for build, rebuild and clean actions:

```
targets = {
  Example1 = cppApplication + {
    defines = { "NDEBUG" }
    libs = { "jpeg" }
    includePaths = { "anypath1", "anypath2" }
    files = {
      "*.cpp" = cppSource
    }
    
    if tool = "codelite" || tool == "codeblocks" {
      buildCommand = "./mare $(target) config=$(configuration)"
      cleanCommand = "./mare clean $(target) config=$(configuration)"
      reBuildCommand = "./mare rebuild $(target) config=$(configuration)"
    }
  }
}
```

Translators
-----------

A Marefile can be translated into project files for Visual Studio, CodeLite, CodeBlocks and NetBeans and into build files for cmake and Make. However currently, these translators are not fully functional. Here is a brief overview over the current state of the development:
<table>
  <tr>
    <td></td>
    <td>mare&nbsp;1)</td>
    <td>vcxproj</td>
    <td>vcproj</td>
    <td>make</td>
    <td>codelite</td>
    <td>codeblocks</td>
    <td>cmake</td>
    <td>netbeans</td>
  </tr>
  <tr>
    <td>configurations</td>
    <td>works</td>
    <td>works</td>
    <td>works</td>
    <td>works</td>
    <td>works</td>
    <td>works</td>
    <td></td>
    <td>works</td>
  </tr>
  <tr>
    <td>platforms</td>
    <td>works</td>
    <td>works</td>
    <td>works</td>
    <td>works</td>
    <td>?</td>
    <td>?</td>
    <td></td>
    <td></td>
  </tr>
  <tr>
    <td>{c,cpp}{Source,Application,DynamicLibrary,StaticLibrary}</td>
    <td>works</td>
    <td>works</td>
    <td>works</td>
    <td>works</td>
    <td></td>
    <td></td>
    <td>works</td>
    <td></td>
  </tr>
  <tr>
    <td>{c,cpp}Compiler</td>
    <td>works</td>
    <td></td>
    <td></td>
    <td>works</td>
    <td></td>
    <td></td>
    <td>works</td>
    <td></td>
  </tr>
  <tr>
    <td>linker</td>
    <td>works</td>
    <td></td>
    <td></td>
    <td>works</td>
    <td></td>
    <td></td>
    <td></td>
    <td></td>
  </tr>
  <tr>
    <td>{build,clean,reBuild}Command</td>
    <td></td>
    <td>works</td>
    <td></td>
    <td></td>
    <td>works</td>
    <td>works</td>
    <td></td>
    <td>works</td>
  </tr>
</table>

1) not a translator
