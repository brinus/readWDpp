<a href="https://brinus.github.io/readWDpp/">
  <img align="center" src="https://github.com/brinus/readWDpp/actions/workflows/docs.yml/badge.svg" />
</a>

# readWDpp
Library to read binary files from WaveDreamBoard (Paul Scherrer Institut).

The library implements some classes to handle file(s) coming from DRS and/or WDB. The final aim is to provide some methods to gather informations such as charge and amplitude about the events in the file(s). 

Read the [documentation](https://brinus.github.io/readWDpp/) for more infos.

To run the examples and compile the static library you can use the `CMakeLists.txt` file
```
$ mkdir build
$ cd build
```
Run `cmake`
```
$ cmake ..
...
$ make
```
This will generate for you all the executables of the examples.
