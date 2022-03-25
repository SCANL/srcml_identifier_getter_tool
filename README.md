# srcml_identifier_getter_tool
SCANL's srcml_identifier_getter_tool. reads a srcML archive and outputs all identifiers in that archive through standard out.

## Cloning the repo
Please clone recursive since we are currently using submodules.

`git clone --recursive git@github.com:SCANL/srcml_identifier_getter_tool.git`

## Setup and Run
1. To make this run, you need *libXML2-dev* and *cmake* installed. Do so using `apt-get install libxml2-dev` and `apt-get install cmake`.
2. `mkdir build` in the root directory
3. `cd build`
4. `cmake ..`
5. `make`
6. `./bin/grabidentifiers <name_of_srcML_archive>`

## Help, I don't know what srcML is
You can get srcML from here -- https://www.srcml.org/
Once you download it and install (or you can just use the executable), you do `srcml <name_of_file_or_directory_containing_code>` and it will create a srcML archive from that directory or code file. You can redirect its output into a file using `>`, or use `srcml -o <FILE>` to output to a specific file. If you need help, use `srcml --help`.

# Limitations
Not tested on windows yet :c -- works on Ubuntu, probably most linux distros, and probably mac