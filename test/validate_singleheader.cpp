// Constructs an AST from the singlheader generated gen files, then serializes it to a set of files.
// Using the new set of serialized files, reconstructs the AST and then serializes it again (to different set of files).
// The two sets of serialized files should be identical. (Verified by comparing the file hashes)
