/* 

  AI image generator using OpenAI API

  - You need to change {OPEN_AI_API} in function.h
  


  - Pouya Rahju:  github.com/pouyarahju
 */


#include <stdio.h>
#include <string.h>
#include "functions.h"

int main(int argc, char *argv[])
{

  FILE *fp;
  char buffer[500];
  /* check args */
  if (argc < 2)
  {
    printf("Example to run a.exe data.txt");
    return 1;
  }
  /* open data file */
  fp = fopen(argv[1], "r");
  if (!fp)
  {
    printf("Can't open this file.\n");
    return 1;
  }
  /* Read data file */
  while (fgets(buffer, 500, fp) != NULL)
  {
    buffer[strlen(buffer) - 1] = '\0';
    printf("%s\n", buffer);
    GenerateImage(buffer);
  }

  fclose(fp);

  return 0;
}
