#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include "strbuf.h"
#include <assert.h>

#define READLEN 256

int stdfile(int fdr, int fdw, int ws);
int readfile(int fd, char* byte, int size);
void writefile(int fd, strbuf_t* buffer);

int main(int argc, char *argv[])
{
	int longword = 0;
	if (argc<3){ // reading from stdin if only 2 args
		assert(atoi(argv[1]) > 0);
		longword = stdfile(0, 1, atoi(argv[1]));// read input from standard input, gets the wraplength from input arguements/terminal 
		if(longword){  // checking to see if stdfile came across a word longer than wraplength
			exit(EXIT_FAILURE);
		}else{
			exit(EXIT_SUCCESS);
		}
		// stops the program here so that none of the other sections get run
	}

	struct stat st;
	stat(argv[2], &st);

	if(!S_ISREG(st.st_mode) && !S_ISDIR(st.st_mode)){
		perror("file/directory does not exist!");
		exit(EXIT_FAILURE);
	}

    if(S_ISREG(st.st_mode)){
    	char inputname[512];// initializes input name
		sprintf(inputname, "./%s", argv[2]);// edits the name so that it will be a file location in the current directory
		int fdr = open(inputname, O_RDONLY);//makes a file descriptor out of it and sets that file descriptor to read only
		if(fdr == -1){
			perror("Unable to open file");
		}
		assert(atoi(argv[1]) > 0);
        longword = stdfile(fdr, 1, atoi(argv[1]));// runs main function on it and outputs to std out based on wraplength
        close(fdr);
		if(longword){
			exit(EXIT_FAILURE);
		}else{
			exit(EXIT_SUCCESS);
		} 
		// stops the program here.
    }
    if(S_ISDIR(st.st_mode)){
    	struct dirent *de;  // Pointer for directory
    	DIR *dr = opendir(argv[2]); // opens the directory based on input 2
  
    	if (dr == NULL)  // opendir returns NULL if couldn't open directory, so the program failed
    	{
      	 	exit(EXIT_FAILURE);
    	}

    	while ((de = readdir(dr)) != NULL) {
			if(de->d_type == DT_REG && memcmp("wrap.", de->d_name, 5)!=0 && memcmp(".", de->d_name, 1)!=0){ // main loop for making directory calls on wordwrap.
				char outputname[512]; // iniitializes name for the output file for this iteration
				char inputname[512]; //initializes name for the input file name, reads from readdir
				sprintf(outputname, "%s/wrap.%s", argv[2], de->d_name);// appends the correct .wrap poriton
				sprintf(inputname, "%s/%s", argv[2],de->d_name); // gets file dest. name from the inputted directory 
				int fdr = open(inputname, O_RDONLY); // sets the file descriptor that we're going to run stdfile() from 
				if(fdr == -1){
					perror("Unable to open file");
				} // stops the program if somehow there is not a null value for the file you were looking for
				int fdw = open(outputname, O_CREAT|O_WRONLY|O_TRUNC, S_IRUSR|S_IWUSR);//same as two lines up, creates output file descriptor
				assert(fdw != -1);// makes sure the file descriptor is not null
				int wraplength = atoi(argv[1]);// gets the wrap length from terminal input
				assert(wraplength>=1); //makes sure you did not input a negative wrap length because then the program wouldnt run
				longword = longword | stdfile(fdr, fdw, wraplength);// runs input file and outputs it based on wraplength to output file.
				close(fdr);// done with current output so we close it. 
				close(fdw);// done with output file so we close it.
			}
		}
    	closedir(dr);     // closes the directory so that it's no longer open.
    	if(longword){
			exit(EXIT_FAILURE);
		}else{
			exit(EXIT_SUCCESS);
		}  // main ends here. 
    }
    
}

/*
	word wrapping function
	@param fdr : input file descriptor
	@param fdw : output file descriptor
	@param ws  : wrap length
	@return	   : 0 if no word longer than wrap length, -1 otherwise
 */
int stdfile(int fdr, int fdw, int ws){
		//return initialization, won't be touched unless word longer than ws
	int err = 0;
		//initialize buffers, space and new are used to print spaces since writefile() takes strbuf*
	strbuf_t buffer;
	sb_init(&buffer, 2);
	strbuf_t space;
	sb_init(&space, 2);
	sb_append(&space, ' ');
	strbuf_t new;
	sb_init(&new, 2);
	sb_append(&new, '\n');
		//initialize char array that will be filled with readfile()
	char val[READLEN];
	char *byte = val;
		//two flags to detect a new paragraph
	int prevn = 0;
	int prevn2 = 0;
		//used is how many characters are currently used in the line
	int used = 0;
		//at the end we'll get less than 256 back, so this makes sure we don't outrange the read bytes
	int readbytes = 0;
		//to make sure the first thing we print out is a word, and not a paragraph break
	int first = 1;
		//flag to ensure that if we get two(or more) '\n's then a word > ws, we don't print another newline
	int firstword = 0;

		//main reading loop
	while((readbytes = readfile(fdr, byte, READLEN))){
			//main loop for going through bytes we've read
		for(int i = 0; i < readbytes; i++){
				//check to see if it's a whitespace char
			if(isspace(val[i])){
					//see if we've already read in a non-whitespace char
					//to ensure we don't print a paragraph break as the first thing
				if(!first){
						//check for '\n', need to treat those special for paragraph breaks
					if(val[i] == '\n'){
							//if we get more than 2 '\n's in a whitespace sequence,
							//ignore the extras ensuring only 2 '\n's printed
						if(prevn2){
							continue;
						}
							//check to see if we've already had 1 '\n' in a whitespace string, if so paragraph break
						else if(prevn){
							writefile(fdw, &new);
							writefile(fdw, &new);
							used = 0;
							prevn2 = 1;
							firstword = 0;
						}
							//first '\n' in whitespace sequence, treat as normal space
						else{
								//make sure there's actually something to write
							if(buffer.used > 1){
								//see if the word will fit in this line, if not print a newline then the word
								if(used + buffer.used <= ws){
										//don't want to print a space before the first word in a line
									if (used!=0){
										writefile(fdw, &space);
									}
									else{
											//buffer.used is 1 greater than the wordlength, so if it's the first
											//word on the line and doesn't print a space we need to compensate
										used--;
									}
									writefile(fdw, &buffer);
										//increment the amount of chars printed in the line (buffer.used is 1 when it's empty
										//so this accounts for the space)
									used += buffer.used;
										//reset the buffer after we've written it
									sb_destroy(&buffer);
									sb_init(&buffer, 2);
								}else{
									if (firstword != 0){
										writefile(fdw, &new);
									}
									writefile(fdw, &buffer);
									used = buffer.used - 1;
									sb_destroy(&buffer);
									sb_init(&buffer, 2);
										//check to see if the word is longer than the wrap length, if so change return to -1
									if(used > ws){
										err = -1;
									}
								}
								firstword = 1;
							}
							prevn = 1;
						}
					}
						//non '\n' whitespace, treat as normal space
					else{
						if(buffer.used > 1){
							if(used + buffer.used <= ws){
								if (used!=0){
									writefile(fdw, &space);
								}
								else{
									used--;
								}
								writefile(fdw, &buffer);
								used += buffer.used;
								sb_destroy(&buffer);
								sb_init(&buffer, 2);
							}else{
								if (firstword != 0){
									writefile(fdw, &new);
								}
								writefile(fdw, &buffer);
								used = buffer.used - 1;
								sb_destroy(&buffer);
								sb_init(&buffer, 2);
								if(used > ws){
									err = -1;
								}
							}
							firstword = 1;
						}
					}
				}	
			}
				//non-whitespace character, load it into the buffer, clear any '\n' flags 
				//and tell the program we've had at least our first word
			else{
				first = 0;
				prevn = 0;
				prevn2 = 0;
				sb_append(&buffer, val[i]);
			}
		}
	}
		//after the reading finished, we may still have a word that wasn't followed by a whitespace,
		//so it never got written
	if(buffer.used > 1){
		if(used + buffer.used <= ws){
			if (used!=0){
				writefile(fdw, &space);
			}
			writefile(fdw, &buffer);
			sb_destroy(&buffer);
			sb_init(&buffer, 2);
		}else{
			if (firstword != 0){
				writefile(fdw, &new);
			}
			writefile(fdw, &buffer);
			sb_destroy(&buffer);
			sb_init(&buffer, 2);
			if(used > ws){
				err = -1;
			}
		}
	}
		//final newline at the end
	writefile(fdw, &new);
		//free the buffers
	sb_destroy(&buffer);
	sb_destroy(&space);
	sb_destroy(&new);
 
	return err;
}
 
int readfile(int fd, char* byte, int size){
	return read(fd, byte, size);
}
 
void writefile(int fd, strbuf_t* buffer){
	write(fd, buffer->data, buffer->used - 1);
}
