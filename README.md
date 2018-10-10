# WinWord Converter GC1039

## Compilation

You'll need MinGW and CMake. Alternatively you can simply open `main.cpp` with Visual Studio,
the project has no dependencies.

```
mkdir build && cd build
cmake ..
make
```

## Running

```
word-converter {cnv file} {input file} {output RTF file}
```

For example:

```
word-converter Doswrd32.cnv some-word-for-dos-document.txt output.rtf
```
