doxygen documentation available at http://84.42.186.246/ctb-doc

repository hosted at http://84.42.186.246/ctb

alternative mirror is at http://github.com/kareltucek/ctb

About
-----

Ctb provides a system for conversion of a nonsequenced intermediate code representation of some pipeline into its realization on some target platform. The current motivation is converting a graph representation of a CIL code to vectorized c++ modules which could then run in the bobox paralel environment (ctb - CIL to Bobox). 

Installation/Usage
------------------

This library can be used from an existing c++11 code by including the "ctb.h" header file. No compilation needed.

The packed makefile produces:
  - A test executable, which serves as a unit test. It produces some sample outputs into the 'output' folder.
  - A command line interface 'ctb'.

Usage
-----

The default project accepts a xml representation of an instruction table (which describes the conversion of instructions) and of a graph, which uses opcodes defined by the instruction table, and produces a simple c++ test unit.

For further information refer to the 'ctb --help' or the ctb class documentation. For input file format refer to the example files under 'xml/' and the xml loader documentation.

About documentation
-------------------
Code is documented in a markdown like, doxygen friendly notation directly in the source files. Every file contains an introductory section with structure definitions which provides abstract description of the structures defined.

Where to continue
-----------------
Good places to begin with are:
- ctb.h documentation - there is a nice paragraph describings how things work together
- aliasenv_generator.h to get an idea how the generation is driven
- getting familiar with what generator, aliasenv, writer and instruction_table classes actually do in order to get a full image of how generating works
- unit test examples

TODOs
-----
Todos are scattered throughout entire project. One a bit more centralized place where these can be found is end of the 'journal'.

Customization
-------------

In order to change the input interface, you will need to register a custom loader in the ctb class. See the example loader class and interface description of the instruction table/generator classes. 

In case you have a generic way of generating the target instructions, you may want to parametrize the ctb by a custom implementation of the instruction table. In that case you still need to provide a loader function, but the interface between the instruction table (respectively its input) is not fixed (e.g. the loader method may be empty).

In order to change the output architecture, you are supposed to provide different instruction tables.

In order to specify a different output environment (e.g. different lanuage/execution environment (such as bobox)), you will need to provide your custom descendant of the model(==aliasenv) class.



