#include "kitty.h"
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

FILE *inputFile;  // file to store the filename in
int input;
int nflag = 0;    // for -n
int Eflag = 0;    // for -E
int eflag = 0;    // for -E
int bflag = 0;    // for -b
int sflag = 0;    // for -s
int vflag = 0;    // for -v
int tflag = 0;    // for -t
int fileIdx = 1;  // position within the argv array where the file exists

int main(int argc, char *argv[]) {
  // char to read in temporarily
  int c = 0;
  char option[] = {"benstuvE"};
  // scan single option commands in
  while ((c = getopt(argc, argv, option)) != -1) {
    switch (c) {
      case 'n':
        nflag = 1;
        break;
      case 'E':
        Eflag = 1;
        break;
      case 'e':
        eflag = 1;
        break;
      case 'b':
        bflag = 1;
        break;
      case 's':
        sflag = 1;
        break;
      case 'v':
        vflag = 1;
        break;
      case 't':
        tflag = 1;
        break;
      case '?':
        fprintf(stderr, "usage: cat [-%s] [file ...]\n",
                option);  // error handling for unknown parameter passed
        return 0;
      default:
        break;
        return 0;
    }
  }

  int minus = 45;  // ascii value for - (arguement)

  // find where the file begins by checking if it begins with - signifying an
  // arguement
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == minus) {
      fileIdx++;
    } else {
      break;
    }
  }
  // testing arguements for method type to trigger
  if (argc == 1) {
    // just plain Kitty
    readFile(NULL);
  } else if (argv[fileIdx] == NULL) {
    // if no file specified stdin is then suggested
    readFile(NULL);
  } else {
    // begin function with supplied filename which is located at fileIdx in the
    // argv array
    for (int i = fileIdx; i < argc; i++) {
      readFile(argv[i]);
    }
  }
  return 0;
}

/* handles files being read in */
void readFile(char *file) {
  // check if file is available
  if (file != NULL) {
    inputFile = fopen(file, "r");
  } else {
    inputFile = stdin;
  }

  int errnum;
  if (inputFile == NULL) {
    // file doesn't exist or cannot be opened for some reason - print error and
    // immediately terminate execution
    errnum = errno;
    fprintf(stderr, "Error opening file: %s\n", strerror(errnum));
    return;
  }

  while (1) {
    bool beginning =
        false;  // variable to print number BEFORE any input is read
    if (nflag || bflag) {
      beginning = true;
    }

    // resolve conflicting options if present
    if (nflag && bflag) {
      nflag = false;
    } else if (tflag && vflag) {
      vflag = false;
    }

    int line = 1;   // line number variable
    int count = 1;  // variable that counts the amount of blank lines - used for
                    // -s command

    while (EOF != (input = fgetc(inputFile))) {
      if (input == '\n') {
        if (Eflag || eflag) {
          if (sflag && count < 2) {
            // to prevent $ being put too many times
            fputc('$', stdout);
          } else if (!sflag) {
            // plain e/E operations
            fputc('$', stdout);
          }
        }
        if (bflag) {
          // enable beginning line number print
          beginning = true;
        }
        if (sflag) {
          // increment count of new line
          count++;
        }
      } else {
        // set new line count to 0 as there is a character and not a new line to
        // squeeze together
        count = 0;
        // print line number at beginning of file
        if (beginning) {
          printf("%6d\t", line);
          line++;
          beginning = false;
        }
      }
      // to do while characters and not a series of blank lines
      if (count <= 2) {
        if (count > 1) {
          // print a line number if sflag AND nflag enabled
          if (sflag && nflag) {
            printf("%6d\t", line);
            line++;
          }
        }

        /*
         Ascii actions based on the index of the character.
            Before 33 is special characters
            A = 65 TO Z = 90
            a = 97 TO z = 122
         */
        //
        if (vflag || tflag || eflag) {
          if ((input < 33 && input >= 1) && (input != '\n')) {
            if (tflag && input == '\t') {
              input = input + 64;    /* forcing the special character to be a
                                        printable ascii character by casting it 64
                                        places to be in the uppercase range */
              printf("^%c", input);  // print a hat then the assocated ascii
                                     // character for the key pressed
            } else if (input != '\t') {
              input = input + 64;    /* forcing the special character to be a
                                        printable ascii character by casting it 64
                                        places to be in the uppercase range */
              printf("^%c", input);  // print a hat then the assocated ascii
                                     // character for the key pressed
            } else {
              printf("\t");  // if its a tab character tab it accross so it is
                             // formatted as required
            }
          } else {
            fputc(input, stdout);  // print normal ascii characters
          }
        } else {
          fputc(input, stdout);  // Where ALL characters that are standardly
                                 // read in are printed out!
        }
      }

      // test for new line after the character has been printed
      if (input == '\n') {
        if (sflag && nflag) {
          beginning = true;  // return to print number at beginning of line
                             // before the next character
        } else if (nflag) {
          printf("%6d\t", line);  // print immediately the line number
          line++;
        }
      } else {
        // do nothing as everything else will be caught
      }
    }
    fclose(inputFile);  // close the file
    return;             // return to main
  }
}
