#include "utf.h"
#include "debug.h"
#include "wrappers.h"
#include <unistd.h>

int
from_utf16be_to_utf16le(int infile, int outfile)
{
  int bom;
  utf16_glyph_t buf;
  ssize_t bytes_read;
  size_t bytes_to_write;
  int ret = 0;
  bom = UTF16LE;
  write_to_bigendian(outfile, &bom, 2);
  while ((bytes_read = read_to_bigendian(infile, &(buf.upper_bytes), 2)) > 0) {
    bytes_to_write = 2;
    reverse_bytes(&(buf.upper_bytes), 2);
    if(is_lower_surrogate_pair(buf)) {
      if((bytes_read = read_to_bigendian(infile, &(buf.lower_bytes), 2)) < 0) {
        break;
      }
      reverse_bytes(&(buf.lower_bytes), 2);
      bytes_to_write += 2;
    }
    write_to_bigendian(outfile, &buf, bytes_to_write);
  }
  ret = bytes_read;
  return ret;
}

//NEED TO CODE HERE
int
from_utf16be_to_utf8(int infile, int outfile)
{
  int bom;
  utf16_glyph_t buf;
  ssize_t bytes_read;
  size_t bytes_to_write;
  int ret = 0;
  bom = UTF8;
  write_to_bigendian(outfile, &bom, 3);
  while ((bytes_read = read_to_bigendian(infile, &(buf.upper_bytes), 2)) > 0) {
    bytes_to_write = 2;
    reverse_bytes(&(buf.upper_bytes), 2);
    if(is_upper_surrogate_pair(buf)) {
      if((bytes_read = read_to_bigendian(infile, &(buf.lower_bytes), 2)) > 0) {
        // do nothing
      }
      reverse_bytes(&(buf.lower_bytes), 2);
      bytes_to_write += 2;
    }
    code_point_t cp =utf16_glyph_to_code_point(&buf);
    size_t s = utf8_glyph_size_of_code_point(cp);
    utf8_glyph_t utf8_gl = code_point_to_utf8_glyph(cp,&s);
    write_to_bigendian(outfile, &utf8_gl, s);
  }
  ret = bytes_read;
  return ret;
}

utf16_glyph_t
code_point_to_utf16be_glyph(code_point_t code_point, size_t *size_of_glyph)
{
  utf16_glyph_t ret;

  memeset(&ret, 0, sizeof ret);
  if(is_code_point_surrogate(code_point)) {
    code_point -= 0x10000;
    ret.upper_bytes = (code_point >> 10) + 0xD800;
    ret.lower_bytes = (code_point & 0x3FF) + 0xDC00;
    reverse_bytes(&ret.upper_bytes, 2);
    reverse_bytes(&ret.lower_bytes, 2);
    *size_of_glyph = 4;
  }
  else {
    ret.upper_bytes |= code_point;
    reverse_bytes(&ret.upper_bytes, 2);
    *size_of_glyph = 2;
  }
  return ret;
}
