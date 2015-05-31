doxygen documentation available at http://84.42.186.246/ctb-doc

repository hosted at http://84.42.186.246/ctb

alternative mirror is at http://github.com/kareltucek/ctb

About
=====

Ctb provides a system for conversion of a nonsequenced intermediate code representation of some pipeline into its realization on some target platform. The current motivation is converting a graph representation of a CIL code to vectorized c++ modules which could then run in the bobox paralel environment (ctb = CIL to Bobox). 

Installation/Usage
==================

This library can be used from an existing c++11 code by including the "ctb.h" header file. No compilation needed.

The packed makefile produces:
  - A test executable, which serves as a unit test. It produces some sample outputs into the 'output' folder.
  - A command line interface 'ctb'.

Usage
=====

The default project accepts a xml representation of an instruction table (which describes the conversion of instructions) and of a graph, which uses opcodes defined by the instruction table, and produces a simple c++ test unit.

In order to change the input interface, you will need to parametrize the ctb class by your custom loader class. See the example loader class and interface description of the instruction table/generator classes. 

In case you have a generic way of generating the target instructions, you may want to parametrize the ctb by a custom implementation of the instruction table. In that case you still need to provide a loader function, but the interface between the instruction table (respectively its input) is not fixed (e.g. the loader method may be empty).

In order to change the output architecture, you are supposed to provide different instruction tables.

In order to specify a different output environment (e.g. different lanuage/execution environment (such as bobox)), you will need to provide your custom descendant of the model class and to call the ctb::process with this class as a template parameter.


