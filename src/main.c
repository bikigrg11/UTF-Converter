#include "debug.h"
#include "utf.h"
#include "wrappers.h"
#include <stdlib.h>

int
main(int argc, char *argv[])
{
  int infile, outfile, in_flags, out_flags;
  parse_args(argc, argv);
  check_bom();
  in_flags = O_RDONLY;
  out_flags = O_WRONLY | O_CREAT;
  infile = Open(program_state->in_file, in_flags);
  outfile = Open(program_state->out_file, out_flags);
  //print_state();
  lseek(infile, program_state->bom_length, SEEK_SET);
  get_encoding_function()(infile, outfile);
  if(program_state != NULL) {
    free(program_state);
  }
  close(infile);
  close(outfile);
  return EXIT_SUCCESS;
}
