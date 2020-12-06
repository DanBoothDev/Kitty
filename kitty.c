#include "kitty.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define asciiMinus 45
#define asciiControlCode 33
#define asciiSymbolEnd 64

FILE *inputFile;
int input;
int nflag = 0;    // for -n - display line numbers
int Eflag = 0;    // for -E - display $ at the end of each line
int bflag = 0;    // for -b - number non-blank output lines
int sflag = 0;    // for -s - squeeze multiple adjacent empty lines into one
int vflag = 0;    // for -v - visibly display non-printing char
int tflag = 0;    // for -t - equivalent to -v, also displaying tab char as ^I
int eflag = 0;    // for -e - equivalent to -vE
int fileIdx = 1;  // position within the argv array where the file exists

int main(int argc, char *argv[]) {
  // char to read in temporarily
  int c = 0;
  char option[] = {"benstuvEh"};
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
      case 'h':
        fprintf(stderr, "usage: cat [-%s] [file ...]\n", option);
        return 0;
      default:
        break;
        return 0;
    }
  }

  // check where the filename begins
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] != asciiMinus) {
      break;
    }
    fileIdx++;
  }

  if (argc == 1 || argv[fileIdx] == NULL) {
    // if no file specified stdin is then suggested
    readFile(NULL);
  } else {
    // read multiple files with supplied filename
    for (int i = fileIdx; i < argc; i++) {
      readFile(argv[i]);
    }
  }
  return 0;
}

/* handles files being read in */
void readFile(char *file) {
  // setup input source
  inputFile = file != NULL ? fopen(file, "r") : stdin;
  if (inputFile == NULL) {
    // file doesn't exist or cannot be opened for some reason
    fprintf(stderr, "Error opening file: %s\n", strerror(errno));
    return;
  }

  // variable to print number BEFORE any input is read
  bool beginning = nflag || bflag;

  // resolve conflicting options if present
  if (nflag && bflag) {
    nflag = false;
  } else if (tflag && vflag) {
    vflag = false;
  }

  int lineNum = 1;
  int blankLines = 1;

  while (EOF != (input = fgetc(inputFile))) {
    if (input == '\n') {
      if (Eflag || eflag) {
        if (sflag && blankLines < 2) {
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
        blankLines++;
      }
    } else {
      // reset new line count as there is a character
      blankLines = 0;
      // print line number at beginning of file
      if (beginning) {
        printf("%6d\t", lineNum);
        lineNum++;
        beginning = false;
      }
    }

    // to do while characters and not a series of blank lines
    if (blankLines <= 2) {
      if (blankLines > 1) {
        // print a line number if sflag AND nflag enabled
        if (sflag && nflag) {
          printf("%6d\t", lineNum);
          lineNum++;
        }
      }

      if (vflag || tflag || eflag) {
        if ((input < asciiControlCode && input >= 1) && (input != '\n')) {
          if (input == '\t') {
            // tab it accross so it is formatted as required
            printf("\t");
          } else {
            // forcing the special character to be a printable ascii character
            input += asciiSymbolEnd;
            // print a hat to indicate it's escaped
            printf("^%c", input);
          }
        } else {
          // print regular ascii characters
          fputc(input, stdout);
        }
      } else {
        // print regular ascii characters
        fputc(input, stdout);
      }
    }

    // test for new line after the character has been printed
    if (input == '\n') {
      if (sflag && nflag) {
        beginning = true;  // return to print number at beginning of line
        // before the next character
      } else if (nflag) {
        printf("%6d\t", lineNum);  // print immediately the line number
        lineNum++;
      }
    }
  }
  fclose(inputFile);
  return;
}
