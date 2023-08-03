# Temporary Code

These are heavy macro code used throughout the library thats intended to be replaced with codegen done with the library itself.

The reason for this is to minimize macro generation to only trivial cases.  
This makes the library more verbose but makes it easier to debug which is of higher priority.

Any sort of verbosity cost will be mitigated with better docs and heavy usage of pragma regions.
