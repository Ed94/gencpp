# Test

The following tests focus on attempting to generate some math, containers, and the memory module of zpl.

Not all the files are written how I would practically use the librarry, the containers for example would
be better on in c++ as templates, since the templates they generate are trivial symbols to inspect or debug.

An exmaple of a non-trival generation is a container for elements with SOA or AOS policy for layout.
(If a unified element syntax is desired)

The test is divided between two major sets of tests: Parsed and Nonparsed.

Parsed uses the parsing api strictly.
NonParsed only uses the upfront and incremental constructors.
