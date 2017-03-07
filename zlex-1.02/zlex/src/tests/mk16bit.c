/* 

Program to generate 16-bit files.  Accepts input files (with extension
.in1) containing numbers in ASCII which are output as 16-bit numbers.  
Each number < 0 is output as a 16-bit '\n'.  

Input should end without any extraneous whitespace after last number.

Uses buffers of fixed size, assumes entire file fits within buffer.

*/

#include <stdio.h>
#include <string.h>

static char usage[]= 
  "mk16bit NAME\n"
  "where NAME has no extension.  Reads NAME.in1 and writes NAME.in.\n";

int 
main(argc, argv)
  int argc;
  char *argv[];
{
  enum {
    BUF_LEN= 10000,
    FNAME_LEN= 100
  };
  unsigned n= 0;		/* # of numbers read. */
  unsigned short buf[BUF_LEN];
  char fName[FNAME_LEN];
  FILE *src, *dest;
  if (argc != 2) {
    fprintf(stderr, usage); exit(1);
  }
  if (strlen(argv[1]) + 4 + 1 >= FNAME_LEN) {
    fprintf(stderr, "Name %s too long.\n", argv[1]); exit(1);
  }
  strcpy(fName, argv[1]); strcpy(fName + strlen(argv[1]), ".in1");
  if (!(src= fopen(fName, "r"))) {
    fprintf(stderr, "Could not open %s.\n", fName); exit(1);
  }
  strcpy(fName, argv[1]); strcpy(fName + strlen(argv[1]), ".in");
  if (!(dest= fopen(fName, "wb"))) {
    fprintf(stderr, "Could not open %s.\n", fName); exit(1);
  }

  while (!feof(src)) {
    int c;
    if (n == BUF_LEN) {
      fprintf(stderr, "Buffer overflow.\n"); exit(1);
    }
    fscanf(src, "%i", &c);
    if (c < 0) c= '\n';
    buf[n++]= c;
  }
  fclose(src); 
  fwrite(buf, sizeof(unsigned short), n, dest);
  fclose(dest);
  return 0;
}
