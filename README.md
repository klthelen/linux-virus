// COMPILING
From the /source folder, all 3 executables (virus, host, seed) can be compiled with
     make all

All objects and binaries can be removed with
     make clean

// TESTING
You can test the program by executing ./seed with any writable but non-executable
files as arguments (this is according to specification).

If you use ./seed with an executable file, it will still run but the executable file
will not be infected. Please remove executable permissions from files before trying
to infect them.

It should be possible to test this program in the way described in the project
specification.

// MUTATION
This virus has a very simple mutation. Every time it infects a new host, it adds
2 empty bytes of 0s (0x00 0x00) right before the 0xDEADBEEF. That is, if you use
this virus to infect multiple copies of the same file one at a time like this:
     ./seed host1
     ./host1 host2
     ./host2 host3
Each subsequent infected file will be 2 bytes larger than the previous infected
file. Then, if you compare their hashes, such as with sha256sum, you will notice
that they have completely different hash outputs.


Thank you!
