#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "xml.h"

static size_t size_of_file(FILE *file) {
  fseek(file, 0, SEEK_END);
  const size_t size = ftell(file);
  fseek(file, 0, SEEK_SET);
  return size;
}

static const char *read_into_memory(const char *path) {
  FILE *file = fopen(path, "rb");
  const size_t length = size_of_file(file);
  char *buffer = (char *)calloc(length + 1, 1);
  fread((void *)buffer, 1, length, file);
  fclose(file);
  return buffer;
}

int empty(const xml_fragment_t *fragment) {
  const char *s = fragment->s;
  for (xml_size_t n = fragment->l; n; --n)
    if (!isspace(*s++))
      return 0;
  return 1;
}

int callback(xml_event_t e,
             xml_size_t n,
             const xml_fragment_t *tag,
             const xml_fragment_t *name,
             const xml_fragment_t *body_or_value,
             void *context)
{
  switch (e) {
    case XML_ELEMENT_BEGIN:
      printf("B /%.*s", tag->l, tag->s);
      for (n--, tag++; n; n--, tag++)
        printf("/%.*s", tag->l, tag->s);
      printf("\n");
      break;
    case XML_ELEMENT_END:
      printf("E /%.*s", tag->l, tag->s);
      for (n--, tag++; n; n--, tag++)
        printf("/%.*s", tag->l, tag->s);
      if (body_or_value)
        if (!empty(body_or_value))
          printf("=%.*s", body_or_value->l, body_or_value->s);
      printf("\n");
      break;
    case XML_ATTRIBUTE:
      printf("   %.*s = %.*s", name->l, name->s, body_or_value->l, body_or_value->s);
      printf("\n");
      break;
  }

  return 0;
}

int main(int argc, const char *argv[]) {
  static const char path[] = "example.xml";

  const char *document = read_into_memory(path);

  static unsigned char scratch[1 * 1024 * 1024];

  switch (xml_parse(document, (void *)scratch, sizeof(scratch), &callback, NULL)) {
    case XML_EMEMORY:
      fprintf(stderr, "Ran out of scratch!\n");
      return EXIT_FAILURE;

    case XML_EARGUMENT:
      fprintf(stderr, "One or more bad arguments\n");
      return EXIT_FAILURE;

    case XML_EPARSE:
      fprintf(stderr, "Parsing failed!\n");
      return EXIT_FAILURE;

    case XML_EUSER:
      fprintf(stderr, "Callback requested parsing halt.\n");
      return EXIT_FAILURE;

    case XML_OK:
      fprintf(stdout, "Done.\n");
  }

  return EXIT_SUCCESS;
}
