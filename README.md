# linux-virus

This is a simple proof-of-concept virus program developed and tested on RHEL8 systems. The virus is benign, but will replicate itself (that is, it does not do anything malicious to the system beyond spreading itself to other programs). It was developed as an exercise for learning how malicious programs operate as part of a computer security course.

From the /source folder, all 3 executables (virus, host, seed) can be compiled with: </br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`make all`<br/>

All objects and binaries can be removed with: </br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`make clean`

You can test the program by executing `./seed` with any writable but non-executable
files as arguments. <b>The requirement for the arguments to be non-excutable files is to help prevent accidentally spreading the virus.</b> If you use ./seed with an executable file, it will still run but the executable file will not be infected. Please remove executable permissions from files before trying
to infect them.

After running `./seed <filename>`, the non-excutable files provided will be infected with the virus code. After adding the executable permission back to the infected files, you can run them like normal and any arguments passed to them will also be infected.

# Example
Assume you have a directory with a copy of `ls` and two other executable files `victim1` and `victim2`. First, remove the executable permission from all 3 files. Then, run <br/>
`./seed ls` </br>

Next, add the executable permission back to `ls` and run `ls victim1 victim2`. Now, all three files will have been infected with the virus code (you can verify this by comparing their file sizes before and after running the malicious code). If `victim1` or `victim2` are then made executable again and take other programs as arguments, they will spread the infection to any arguments provided to them.

### Mutation
This virus has a very simple mutation. Every time it infects a new host, it adds
2 empty bytes of 0s (0x00 0x00) right before the 0xDEADBEEF. That is, if you use
this virus to infect multiple copies of the same file one at a time like this:<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`./seed host1`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`./host1 host2`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`./host2 host3`<br/>

Each subsequent infected file will be 2 bytes larger than the previous infected
file. Then, if you compare their hashes, such as with sha256sum, you will notice
that they have completely different hash outputs.
