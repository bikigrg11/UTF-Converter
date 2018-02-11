#include "debug.h"
#include "utf.h"
#include "wrappers.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int opterr;
int optopt;
int optind;
char *optarg;

state_t *program_state;
char ** stringDivder(char *str);

void
parse_args(int argc, char *argv[])
{
  int flag = 0;
  int i;
  char option;
  char ** nowthis=NULL;
  program_state = Calloc(1, sizeof(state_t));

  for(int b = 0; b <argc; b++ ){
    if(!(strcmp(argv[b],"-e"))){
      flag ++;
    }
  }

  if(flag == 1){
    nowthis = join_string_array(argc, argv);
    char str1[30], str2[30], str3[30];
    memcpy(str1,nowthis[1],30);
    memcpy(str2,nowthis[2],30);
    memcpy(str3,nowthis[3],30);

    if ((program_state->encoding_to = determine_format(str1)) == 0){
      USAGE(argv[0]);
      exit(1);
    }

    for (i = 0; i < argc; i++) {
      if(!(strcmp(argv[i],str2))){
        program_state->in_file = argv[i];
      }
      if(!(strcmp(argv[i],str3))){
        program_state->out_file = argv[i];
      }
    }
  }
  else if(flag > 1){
    USAGE(argv[0]);
    exit(1);
  }
  else {
    for (i = 0; optind < argc; i++) {
      option = getopt(argc, argv,"e:");
      if (option != -1) {
        switch (option) {
          case '?': {
            if (optopt != 'h'){
              fprintf(stderr, KRED "-%c is not a supported argument\n" KNRM,optopt);
            }
              USAGE(argv[0]);
              exit(1);
          }
          default:{
             exit(1);
            break;
          }
        }
      }
    }
  }
    free(nowthis);
}


format_t
determine_format(char *argument)
{

  if (strcmp(argument, "UTF16LE") == 0){
    return UTF16LE;
  }
  if (strcmp(argument, "UTF16BE") == 0)
    return UTF16BE;
  if (strcmp(argument, "UTF8") == 0)
    return UTF8;
  return 0;
}

char*
bom_to_string(format_t bom){
  switch(bom){      ///
    case UTF8: return STR_UTF8;
    case UTF16BE: return STR_UTF16BE;
    case UTF16LE: return STR_UTF16LE;
  }
  return "UNKNOWN";
}

char ** stringDivder(char *str){
  int spaceNum = 0;
  char *  point    = strtok (str, " ");
  char ** result  = NULL;
  while (point) {
    result = realloc (result, sizeof (char*) * ++spaceNum);
    if (result == NULL)
      exit (0);
    result[spaceNum-1] = point;
    point = strtok (NULL, " ");
  }
  result = realloc (result, sizeof (char*) * (spaceNum+1));
  result[spaceNum] = 0;
  return result;
}

char**
join_string_array(int count, char *array[])
{
  char charArray[100];
  char *ret = charArray;
  int i;
  int len = 0, cur_str_len;
  int str_len = array_size(count, array);
  int counter = 0;
  for (i = 0; i < 5; i++) {
    if(len < str_len){
      if(!(strcmp(*(array+i),"-e"))){
        counter++;
      }else{
        counter++;
        if(counter == 6){
          break;
        }
        cur_str_len = strlen(*(array+i));
        memecpy(ret + len, *(array+i), cur_str_len);
        len += cur_str_len;
        memecpy(ret + len, " ", 1);
        len += 1;
      }
    }
  }

    char** newRet = stringDivder(ret);
    return newRet;
}

int
array_size(int count, char *array[])
{
  int i, sum = 1;
  for (i = 0; i < count; ++i) {
    sum += strlen(array[i]);
    ++sum;
  }
  return sum+1;
}

void
print_state()
{
  if (program_state == NULL) {
    error("program_state is %p", (void*)program_state);
    exit(EXIT_FAILURE);
  }
  info("program_state {\n"
         "  format_t encoding_to = 0x%X;\n"
         "  format_t encoding_from = 0x%X;\n"
         "  char *in_file = '%s';\n"
         "  char *out_file = '%s';\n"
         "};\n",
         program_state->encoding_to, program_state->encoding_from,
         program_state->in_file, program_state->out_file);
}
