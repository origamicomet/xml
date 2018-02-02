/*
 * Copyright 2017 G. David Butler. All rights reserved.
 * License: https://github.com/gdavidbutler/xmlTrivialCallbackParser/blob/master/LICENSE
 */

#include "xml.h"

XML_BEGIN_EXTERN_C

#include <stdio.h>
#include <stdlib.h>

/* Various helper macros employed to reduce repetition and increase clarity. */

#if 1

#define EMIT(Literal, Length) do { \
  if (out_len >= (Length)) {       \
    EMIT_##Length(Literal);        \
    out_len -= (Length);           \
  }                                \
  length += (Length);              \
} while (0,0)

#define EMIT_1(L) do {            *out++ = 0[L]; } while (0,0)
#define EMIT_2(L) do { EMIT_1(L); *out++ = 1[L]; } while (0,0)
#define EMIT_3(L) do { EMIT_2(L); *out++ = 2[L]; } while (0,0)
#define EMIT_4(L) do { EMIT_3(L); *out++ = 3[L]; } while (0,0)
#define EMIT_5(L) do { EMIT_4(L); *out++ = 4[L]; } while (0,0)
#define EMIT_6(L) do { EMIT_5(L); *out++ = 5[L]; } while (0,0)
#define EMIT_7(L) do { EMIT_6(L); *out++ = 6[L]; } while (0,0)
#define EMIT_8(L) do { EMIT_7(L); *out++ = 7[L]; } while (0,0)
#define EMIT_9(L) do { EMIT_8(L); *out++ = 8[L]; } while (0,0)

#else

/* Determine if we can coerce compilers into emitting `rep movsb`. */
#define EMIT(Literal, Length) do {     \
  if (out_len >= (Length))             \
    for (int I = 0; I < (Length); ++I) \
      *out_len--, *out++ = I[Literal]; \
  length += (Length);                  \
} while (0,0)

#endif

#define COPY(Count) do {   \
  if (out_len >= (Count))  \
    while (*out++ = *in++) \
      out_len--;           \
  else                     \
    in += (Count);         \
  length += (Count);       \
} while (0,0)

#define ADJUST(Pointer, Offset) \
  (void *)(xml_uintptr_t(Pointer) + (Offset))

static int xml__fallback(xml_event_t e,
                         xml_size_t n,
                         const xml_fragment_t *const tag,
                         const xml_fragment_t *const name,
                         const xml_fragment_t *const body_or_value,
                         void *context)
{
  return 0;
}

xml_result_t xml_parse(const char *document,
                       void *scratch,
                       xml_size_t amount_of_scratch,
                       xml_callback_fn *callback,
                       void *context)
{
  /* Alias to reduce verbosity. */
  const char *s = document;

  xml_size_t tgM = 0; /* Maximum level allocated. */
  xml_size_t tgL = 0; /* Current level. */
  xml_size_t tgD = 0; /* Gone deeper? */

  xml_fragment_t *tags = (xml_fragment_t *)scratch;

  xml_fragment_t name;
  xml_fragment_t value;

  xml_size_t declaration = 0, /* Tracks if we're in a declaration. */
             doctype     = 0; /* Tracks if we're in a DOCTYPE tag. */

  if (!document)
    /* No document provided? */
    return XML_EARGUMENT;

  if (!callback)
    /* Rather than checking if we were passed a callback prior to each call, we
       substitute a dummy callback. */
    callback = &xml__fallback;

tgEnd:
  value.s = s;
  goto bgn;

err:
  return XML_EPARSE;

atrEq:
  for (;;) switch (*s++) {
  case '\0':
    goto rtn;

  case '\t': case '\n': case '\r': case ' ':
    break;

  case '"':
    goto atrValDq;

  case '\'':
    goto atrValSq;

  default:
    goto err;
  }

nlTg:
  value.l = 0;
  if (callback(XML_ELEMENT_END, tgL, tags, 0, &value, context) != 0)
    return XML_EUSER;
  if (tgL)
    tgL--;
  for (;;) switch (*s++) {
  case '\0':
    goto rtn;

  case '>':
    goto tgEnd;

  default:
    goto err;
  }

nlAtrVal:
  value.l = 0;
  if (callback(XML_ATTRIBUTE, tgL, tags, &name, &value, context) != 0)
    return XML_EUSER;
  name.l = 0;
  s--;
  for (;;) switch (*s++) {
  case '\0':
    goto rtn;

  case '\t': case '\n': case '\r': case ' ':
    break;

  case '"':
    goto atrValDq;

  case '\'':
    goto atrValSq;

  case '/':
    if (declaration)
      goto atr;
    else
      goto nlTg;

  case '?':
    if (declaration) {
      declaration--;
      goto nlTg;
    } else
      goto atr;

  case '<':
    goto err;

  case '>':
    if (doctype) {
      doctype--;
      s--;
      goto nlTg;
    } else
      goto tgEnd;

  case '[':
    if (doctype)
      goto tgEnd;
    else
      goto atr;

  default:
    goto atr;
  }

atrNm:
  name.l = s - name.s - 1;
  s--;
  for (;;) switch (*s++) {
  case '\0':
    goto rtn;

  case '\t': case '\n': case '\r': case ' ':
    break;

  case '<':
    goto err;

  case '=':
    goto atrEq;

  default:
    goto nlAtrVal;
  }

atr:
  name.s = s - 1;
  for (;;) switch (*s++) {
  case '\0':
    goto rtn;

  case '\t': case '\n': case '\r': case ' ':
  case '"': case '\'': case '/': case '?': case '=': case '>':
    goto atrNm;

  case '<':
    goto err;

  default:
    break;
  }

atrVal:
  value.l = s - value.s - 1;
  if (name.l) {
    if (callback(XML_ATTRIBUTE, tgL, tags, &name, &value, context) != 0) {
      return XML_EUSER;
    }
  } else if (callback(XML_ATTRIBUTE, tgL, tags, &value, &name, context) != 0) {
    return XML_EUSER;
  }
  for (;;) switch (*s++) {
  case '\0':
    goto rtn;

  case '\t': case '\n': case '\r': case ' ':
    break;

  case '"':
    goto atrValDq;

  case '\'':
    goto atrValSq;

  case '/':
    if (declaration)
      goto atr;
    else
      goto nlTg;

  case '?':
    if (declaration) {
      declaration--;
      goto nlTg;
    } else
      goto atr;

  case '<': case '=':
    goto err;

  case '>':
    if (doctype) {
      doctype--;
      s--;
      goto nlTg;
    } else
      goto tgEnd;

  case '[':
    if (doctype)
      goto tgEnd;
    else
      goto atr;

  default:
    goto atr;
  }

atrValDq:
  value.s = s;
  for (;;) switch (*s++) {
  case '\0':
    goto rtn;

  case '"':
    goto atrVal;

  case '<': case '>':
    goto err;

  default:
    break;
  }

atrValSq:
  value.s = s;
  for (;;) switch (*s++) {
  case '\0':
    goto rtn;

  case '\'':
    goto atrVal;

  case '<': case '>':
    goto err;

  default:
    break;
  }

eTgNm:
  if (tgL)
    tags[tgL-1].l = s - tags[tgL-1].s - 1;
  else {
    tags[tgL].l = s - tags[tgL].s - 1;
    tgL++;
  }
  if (tgL <= tgD)
    value.l = 0;
  if (callback(XML_ELEMENT_END, tgL, tags, 0, &value, context) != 0)
    return XML_EUSER;
  if (tgL)
    tgL--;
  s--;
  for (;;) switch (*s++) {
  case '\0':
    goto rtn;

  case '>':
    goto tgEnd;

  default:
    goto err;
  }

eNm:
  if (tgL)
    tags[tgL-1].s = s - 1;
  else {
    if (tgL == tgM) {
      if (sizeof(xml_fragment_t) > amount_of_scratch)
        return XML_EMEMORY;
      amount_of_scratch -= sizeof(xml_fragment_t);
      tgM++;
    }
    tags[tgL].s = s - 1;
  }
  for (;;) switch (*s++) {
  case '\0':
    goto rtn;

  case '\t': case '\n': case '\r': case ' ':
  case '"': case '\'': case '/': case '<':
    goto err;

  case '>':
    goto eTgNm;

  default:
    break;
  }

eTg:
  for (;;) switch (*s++) {
  case '\0':
    goto rtn;

  case '\t': case '\n': case '\r': case ' ':
  case '"': case '\'':
  case '<': case '>':
    goto err;

  default:
    goto eNm;
  }

sTgNm:
  tags[tgL].l = s - tags[tgL].s - 1;
  if (tags[tgL].l == 4
   && *(tags[tgL].s + 0) == '?'
   && *(tags[tgL].s + 1) == 'x'
   && *(tags[tgL].s + 2) == 'm'
   && *(tags[tgL].s + 3) == 'l')
    declaration = 1;
  else if (tags[tgL].l == 8
   && *(tags[tgL].s + 0) == '!'
   && *(tags[tgL].s + 1) == 'D'
   && *(tags[tgL].s + 2) == 'O'
   && *(tags[tgL].s + 3) == 'C'
   && *(tags[tgL].s + 4) == 'T'
   && *(tags[tgL].s + 5) == 'Y'
   && *(tags[tgL].s + 6) == 'P'
   && *(tags[tgL].s + 7) == 'E')
    doctype = 1;
  tgD = tgL++;
  if (callback(XML_ELEMENT_BEGIN, tgL, tags, 0, 0, context) != 0)
    return XML_EUSER;
  s--;
  for (;;) switch (*s++) {
  case '\0':
    goto rtn;

  case '\t': case '\n': case '\r': case ' ':
    break;

  case '"':
    goto atrValDq;

  case '\'':
    goto atrValSq;

  case '/':
    if (declaration)
      goto atr;
    else
      goto nlTg;

  case '?':
    if (declaration) {
      declaration--;
      goto nlTg;
    } else
      goto atr;

  case '<': case '=':
    goto err;

  case '>':
    if (doctype) {
      doctype--;
      s--;
      goto nlTg;
    } else
      goto tgEnd;

  case '[':
    if (doctype)
      goto tgEnd;
    else
      goto atr;

  default:
    goto atr;
  }

sNm:
  if (tgL == tgM) {
    if (sizeof(xml_fragment_t) > amount_of_scratch)
      return XML_EMEMORY;
    amount_of_scratch -= sizeof(xml_fragment_t);
    tgM++;
  }
  tags[tgL].s = s - 1;
  if (doctype)
    doctype++;
  for (;;) switch (*s++) {
  case '\0':
    goto rtn;

  case '\t': case '\n': case '\r': case ' ':
  case '"': case '\'': case '>':
    goto sTgNm;

  case '/':
    if (declaration)
      break;
    else
      goto sTgNm;

  case '?':
    if (declaration) {
      declaration--;
      goto sTgNm;
    } else
      break;

  case '<':
    goto err;

  default:
    break;
  }

sTg:
  value.l = s - value.s - 1;
  for (;;) switch (*s++) {
  case '\0':
    goto rtn;

  case '\t': case '\n': case '\r': case ' ':
  case '"': case '\'':
  case '<': case '>':
    goto err;

  case '!':
    if (*(s + 0) == '-'
     && *(s + 1) == '-'
     && (*(s + 2) == ' ' || *(s + 2) == '\n' || *(s + 2) == '\r')) {
      for (s += 2; *s; s++)
        if ((*(s + 0) == ' ' || *(s + 0) == '\n' || *(s + 0) == '\r')
         && *(s + 1) == '-'
         && *(s + 2) == '-'
         && *(s + 3) == '>') {
          s += 4;
          goto tgEnd;
        }
      s++;
      goto rtn;
    }
    goto sNm;

  case '/':
    goto eTg;

  default:
    goto sNm;
  }

bgn:
  for (;;) switch (*s++) {
  case '\0':
    goto rtn;

  case '<':
    if (*(s + 0) == '!'
     && *(s + 1) == '['
     && *(s + 2) == 'C'
     && *(s + 3) == 'D'
     && *(s + 4) == 'A'
     && *(s + 5) == 'T'
     && *(s + 6) == 'A'
     && *(s + 7) == '[') {
      for (s += 7; *s; s++)
        if (*(s + 0) == ']'
         && *(s + 1) == ']'
         && *(s + 2) == '>') {
          s += 3;
          goto bgn;
        }
      s++;
      goto rtn;
    }
    goto sTg;

  case '>':
    if (doctype) {
      doctype--;
      s--;
      goto nlTg;
    } else
      break;

  default:
    break;
  }

rtn:
  return XML_OK;
}

#if 0

int
xmlDecodeBody(
  unsigned char *out
 ,int olen
 ,const unsigned char *in
 ,int ilen
){
  int len;

  len = 0;
  for (; ilen--;) switch (*in) {
  case '<':
    if (ilen > 10
     && *(in + 1) == '!'
     && *(in + 2) == '['
     && *(in + 3) == 'C'
     && *(in + 4) == 'D'
     && *(in + 5) == 'A'
     && *(in + 6) == 'T'
     && *(in + 7) == 'A'
     && *(in + 8) == '[') {
      for (in += 9, ilen -= 8; ilen--; in++, len++) {
        if (*(in + 0) == ']'
         && *(in + 1) == ']'
         && *(in + 2) == '>') {
          in += 3, ilen -= 2;
          break;
        }
        if (olen > 0) {
          *out++ = *in;
          olen--;
        }
      }
    } else {
      if (olen > 0) {
        *out++ = *in;
        olen--;
      }
      in++;
      len++;
    }
    break;
  case '&':
    if (!(in++,ilen--)) goto err; else switch (*in) {
    case 'a':
      if (!(in++,ilen--)) goto err; else switch (*in) {
      case 'm':
        if (!(in++,ilen--)) goto err; else switch (*in) {
        case 'p':
          if (!(in++,ilen--)) goto err; else switch (*in) {
          case ';':
            if (olen > 0) {
              *out++ = '&';
              olen--;
            }
            in++;
            len++;
            continue;
          default:
            goto err;
          }
        default:
          goto err;
        }
      case 'p':
        if (!(in++,ilen--)) goto err; else switch (*in) {
        case 'o':
          if (!(in++,ilen--)) goto err; else switch (*in) {
          case 's':
            if (!(in++,ilen--)) goto err; else switch (*in) {
            case ';':
              if (olen > 0) {
                *out++ = '\'';
                olen--;
              }
              in++;
              len++;
              continue;
            default:
              goto err;
            }
          default:
            goto err;
          }
        default:
          goto err;
        }
      default:
        goto err;
      }
    case 'g':
      if (!(in++,ilen--)) goto err; else switch (*in) {
      case 't':
        if (!(in++,ilen--)) goto err; else switch (*in) {
        case ';':
          if (olen > 0) {
            *out++ = '>';
            olen--;
          }
          in++;
          len++;
          continue;
        default:
          goto err;
        }
      default:
        goto err;
      }
    case 'l':
      if (!(in++,ilen--)) goto err; else switch (*in) {
      case 't':
        if (!(in++,ilen--)) goto err; else switch (*in) {
        case ';':
          if (olen > 0) {
            *out++ = '<';
            olen--;
          }
          in++;
          len++;
          continue;
        default:
          goto err;
        }
      default:
        goto err;
      }
    case 'q':
      if (!(in++,ilen--)) goto err; else switch (*in) {
      case 'u':
        if (!(in++,ilen--)) goto err; else switch (*in) {
        case 'o':
          if (!(in++,ilen--)) goto err; else switch (*in) {
          case 't':
            if (!(in++,ilen--)) goto err; else switch (*in) {
            case ';':
              if (olen > 0) {
                *out++ = '"';
                olen--;
              }
              in++;
              len++;
              continue;
            default:
              goto err;
            }
          default:
            goto err;
          }
        default:
          goto err;
        }
      default:
        goto err;
      }
    case '#':
      if (!(in++,ilen--)) goto err; else switch (*in) {
        unsigned long c;

      case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
        c = *in - '0';
        for (in++; ilen; in++, ilen--) switch (*in) {
        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
          c *= 10;
          c += *in - '0';
          break;
        case ';':
          goto enc;
        default:
          goto err;
        }
        goto err;
      case 'x':
        if (!(in++,ilen--)) goto err; else switch (*in) {
        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
          c = *in - '0';
          goto nxtH;
        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
          c = 10 + (*in - 'A');
          goto nxtH;
        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
          c = 10 + (*in - 'a');
nxtH:
          for (in++; ilen; in++, ilen--) switch (*in) {
          case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
            c *= 16;
            c += *in - '0';
            break;
          case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
            c *= 16;
            c += 10 + (*in - 'A');
            break;
          case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
            c *= 16;
            c += 10 + (*in - 'a');
            break;
          case ';':
            goto enc;
          default:
            goto err;
          }
          goto err;
enc:
          /* https://en.wikipedia.org/wiki/UTF-8 */
          if (!ilen--)
            goto err;
          if (c <= 0x7f) { /* 7 bits */
            if (olen > 0) {
              *out++ = c;
              olen--;
            }
            len++;
          } else if (c <= 0x7ff) { /* 11 bits */
            if (olen > 1) {
              *out++ = 0xc0 | (c >> 6);
              *out++ = 0x80 | (c & 0x3f);
              olen -= 2;
            }
            len += 2;
          } else if (c <= 0xffff) { /* 16 bits */
            if (olen > 2) {
              *out++ = 0xe0 | (c >> 12);
              *out++ = 0x80 | (c >> 6 & 0x3f);
              *out++ = 0x80 | (c & 0x3f);
              olen -= 3;
            }
            len += 3;
          } else if (c <= 0x1fffff) { /* 21 bits */
            if (olen > 3) {
              *out++ = 0xf0 | (c >> 18);
              *out++ = 0x80 | (c >> 12 & 0x3f);
              *out++ = 0x80 | (c >> 6 & 0x3f);
              *out++ = 0x80 | (c & 0x3f);
              olen -= 4;
            }
            len += 4;
          } else if (c <= 0x3ffffff) { /* 26 bits */
            if (olen > 4) {
              *out++ = 0xf8 | (c >> 24);
              *out++ = 0x80 | (c >> 18 & 0x3f);
              *out++ = 0x80 | (c >> 12 & 0x3f);
              *out++ = 0x80 | (c >> 6 & 0x3f);
              *out++ = 0x80 | (c & 0x3f);
              olen -= 5;
            }
            len += 5;
          } else if (c <= 0x3ffffff) { /* 31 bits */
            if (olen > 5) {
              *out++ = 0xfc | (c >> 30);
              *out++ = 0x80 | (c >> 24 & 0x3f);
              *out++ = 0x80 | (c >> 18 & 0x3f);
              *out++ = 0x80 | (c >> 12 & 0x3f);
              *out++ = 0x80 | (c >> 6 & 0x3f);
              *out++ = 0x80 | (c & 0x3f);
              olen -= 6;
            }
            len += 6;
          } else
            goto err;
          in++;
          continue;
        default:
          goto err;
        }
      default:
        goto err;
      }
    default:
      goto err;
    }
  default:
    if (olen > 0) {
      *out++ = *in;
      olen--;
    }
    in++;
    len++;
    break;
  }
  return len;
err:
  return -1;
}

#endif

int xml_encode_string(const char *in, xml_size_t in_len,
                      char *out, xml_size_t out_len)
{
  int length = 0;

  while (in_len--) {
    switch (*in) {
      /* Escape special characters. */
      case '&':  EMIT("&amp;",  5); break;
      case '\'': EMIT("&apos;", 6); break;
      case '>':  EMIT("&gt;",   4); break;
      case '<':  EMIT("&lt;",   4); break;
      case '"':  EMIT("&quot;", 6); break;

      /* Otherwise, copy through. */
      default:
        if (out_len > 0)
          out_len--, *out++ = *in;
        length++;
    }

    in++;
  }

  return length;
}

int xml_encode_cdata(const char *in, xml_size_t in_len,
                     char *out, xml_size_t out_len)
{
  int length = 0;

  EMIT("<![CDATA[", 9);

  /* PERF(mtwilliams): Replace with copy. */
  while (in_len--) {
    if (out_len > 0)
      out_len--, *out++ = *in;
    length++;
    in++;
  }

  EMIT("]]>", 3);

  return length;
}

int xml_encode_uri(const char *in, xml_size_t in_len,
                   char *out, xml_size_t out_len)
{
  static const char hex[] = "0123456789ABCDEF";

  int length = 0;

  while (in_len--) {
    switch (*in) {
      /* Copy through unreserved characters. */
      case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
      case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
      case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U':
      case 'V': case 'W': case 'X': case 'Y': case 'Z': case 'a': case 'b':
      case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i':
      case 'j': case 'k': case 'l': case 'm': case 'n': case 'o': case 'p':
      case 'q': case 'r': case 's': case 't': case 'u': case 'v': case 'w':
      case 'x': case 'y': case 'z': case '0': case '1': case '2': case '3':
      case '4': case '5': case '6': case '7': case '8': case '9': case '-':
      case '_': case '.': case '~':
        COPY(1);
        break;

      /* Otherwise, percent encode. */
      default:
        if (out_len >= 3) {
          *out++ = '%';
          *out++ = hex[*in >> 4];
          *out++ = hex[*in & 0x0f];
          out_len -= 3;
        }

        in++;
        length += 3;
    }
  }

  return length;
}

int xml_decode_uri(const char *in, xml_size_t in_len,
                   char *out, xml_size_t out_len)
{
  int length = 0;

  while (in_len--) {
    unsigned char c;

    switch (*in) {
      /* Decode percent encoded character. */
      case '%': {
        if (!(in++, in_len--))
          return -1;

        switch (*in) {
          case '0': case '1': case '2': case '3': case '4':
          case '5': case '6': case '7': case '8': case '9':
            c = *in - '0';
            goto hex;
          case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
          case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
            c = 10 + ((*in & 32) - 'A');
            goto hex;

          hex: {
            if (!(in++, in_len--))
              return -1;

            switch (*in) {
              case '0': case '1': case '2': case '3': case '4':
              case '5': case '6': case '7': case '8': case '9':
                c *= 16;
                c += *in - '0';
                break;
              case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
              case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
                c *= 16;
                c += 10 + ((*in & 32) - 'A');
                break;
              default:
                return -1;
            }

            if (out_len > 0) {
              *out++ = c;
              out_len--;
            }

            in++;
            length++;
          } break;

          default:
            return -1;
        }
      } break;

      /* Otherwise, copy through. */
      default:
        COPY(1);
    }
  }

  return length;
}

int xml_encode_base64(const unsigned char *in, xml_size_t in_len,
                      char *out, xml_size_t out_len)
{
  static const char alphabet[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

  int length;

  for (length = 0; in_len >= 3; in += 3, in_len -= 3, length += 4) {
    if (out_len >= 4) {
      *out++ = alphabet[in[0] >> 2];
      *out++ = alphabet[((in[0] << 4) & 0x30) | (in[1] >> 4)];
      *out++ = alphabet[((in[1] << 2) & 0x3c) | (in[2] >> 6)];
      *out++ = alphabet[in[2] & 0x3f];
      out_len -= 4;
    }
  }

  if (in_len) {
    if (out_len >= 4) {
      unsigned char fragment;
      *out++ = alphabet[in[0] >> 2];
      fragment = (in[0] << 4) & 0x30;
      if (in_len > 1)
        fragment |= in[1] >> 4;
      *out++ = alphabet[fragment];
      *out++ = (in_len > 1) ? alphabet[(in[1] << 2) & 0x3c] : '=';
      *out++ = '=';
    }
    length += 4;
  }

  return length;
}

int xmL_decode_base64(const char *in, xml_size_t in_len,
                      unsigned char *out, xml_size_t out_len)
{
  static unsigned char const decode[] = {
    66, 66, 66, 66,  66, 66, 66, 66,  66, 64, 64, 66,  66, 64, 66, 66,
    66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,
    64, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 62,  66, 66, 66, 63,
    52, 53, 54, 55,  56, 57, 58, 59,  60, 61, 66, 66,  66, 65, 66, 66,
    66,  0,  1,  2,   3,  4,  5,  6,   7,  8,  9, 10,  11, 12, 13, 14,
    15, 16, 17, 18,  19, 20, 21, 22,  23, 24, 25, 66,  66, 66, 66, 66,
    66, 26, 27, 28,  29, 30, 31, 32,  33, 34, 35, 36,  37, 38, 39, 40,
    41, 42, 43, 44,  45, 46, 47, 48,  49, 50, 51, 66,  66, 66, 66, 66,
    66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,
    66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,
    66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,
    66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,
    66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,
    66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,
    66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,
    66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66
  };

  unsigned buffer = 1;
  int length = 0;

  while (in_len-- > 0) {
    unsigned char c;

    switch ((c = decode[*(unsigned char *)in++])) {
      case 66: /* Invalid. */
        return -1;
      case 64: /* Whitespace. */
        continue;
      case 65: /* Padding. */
        in_len = 0;
        break;
      default:
        buffer = buffer << 6 | c;
        if (buffer & 0x1000000) {
          if (out_len >= 3) {
            *out++ = buffer >> 16;
            *out++ = buffer >> 8;
            *out++ = buffer;
            out_len -= 3;
          }
          length += 3;
          buffer = 1;
        }
    }
  }

  if (buffer & 0x40000) {
    if (out_len >= 2) {
      *out++ = buffer >> 10;
      *out++ = buffer >> 2;
    }
    length += 2;
  } else if (buffer & 0x1000) {
    if (out_len >= 1)
      *out++ = buffer >> 4;
    length += 1;
  }

  return length;
}

int xml_encode_hex(const unsigned char *in, xml_size_t in_len,
                   char *out, xml_size_t out_len)
{
  static const char alphabet[] = "0123456789ABCDEF";

  int length = 0;

  for (length = 0; in_len > 0; in++, in_len--, length += 2) {
    if (out_len > 1) {
      *out++ = alphabet[*in >> 4];
      *out++ = alphabet[*in & 0x0F];
      out_len -= 2;
    }
  }

  return length;
}

int xml_decode_hex(const char *in, xml_size_t in_len,
                   unsigned char *out, xml_size_t out_len)
{
  static const unsigned char decode[] = {
    17, 17, 17, 17,  17, 17, 17, 17,  17, 16, 16, 17,  17, 16, 17, 17,
    17, 17, 17, 17,  17, 17, 17, 17,  17, 17, 17, 17,  17, 17, 17, 17,
    16, 17, 17, 17,  17, 17, 17, 17,  17, 17, 17, 17,  17, 17, 17, 17,
     0,  1,  2,  3,   4,  5,  6,  7,   8,  9, 17, 17,  17, 17, 17, 17,
    17, 10, 11, 12,  13, 14, 15, 17,  17, 17, 17, 17,  17, 17, 17, 17,
    17, 17, 17, 17,  17, 17, 17, 17,  17, 17, 17, 17,  17, 17, 17, 17,
    17, 10, 11, 12,  13, 14, 15, 17,  17, 17, 17, 17,  17, 17, 17, 17,
    17, 17, 17, 17,  17, 17, 17, 17,  17, 17, 17, 17,  17, 17, 17, 17,
    17, 17, 17, 17,  17, 17, 17, 17,  17, 17, 17, 17,  17, 17, 17, 17,
    17, 17, 17, 17,  17, 17, 17, 17,  17, 17, 17, 17,  17, 17, 17, 17,
    17, 17, 17, 17,  17, 17, 17, 17,  17, 17, 17, 17,  17, 17, 17, 17,
    17, 17, 17, 17,  17, 17, 17, 17,  17, 17, 17, 17,  17, 17, 17, 17,
    17, 17, 17, 17,  17, 17, 17, 17,  17, 17, 17, 17,  17, 17, 17, 17,
    17, 17, 17, 17,  17, 17, 17, 17,  17, 17, 17, 17,  17, 17, 17, 17,
    17, 17, 17, 17,  17, 17, 17, 17,  17, 17, 17, 17,  17, 17, 17, 17,
    17, 17, 17, 17,  17, 17, 17, 17,  17, 17, 17, 17,  17, 17, 17, 17
  };

  unsigned long buffer = 1;
  int length = 0;

  while (in_len-- > 0) {
    unsigned char c;

    switch ((c = decode[*(unsigned char*)in++])) {
      case 17: /* Invalid. */
        return -1;
      case 16: /* Whitespace. */
        continue;
      default:
        buffer = buffer << 4 | c;
        if (buffer & 0x100) {
          if (out_len > 0) {
            *out++ = buffer;
            out_len--;
          }
          length++;
          buffer = 1;
        }
    }
  }

  if (buffer != 1)
    /* Split byte. */
    return -1;

  return length;
}

XML_END_EXTERN_C
