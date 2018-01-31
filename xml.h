//===-- xml.h ---------------------------------------------*- mode: C99 -*-===//
//
//                            __ __ _____ __
//                           |  |  |     |  |
//                           |-   -| | | |  |__
//                           |__|__|_|_|_|_____|
//
//       This file is distributed under the terms described in LICENSE.
//          https://github.com/origamicomet/xml/blob/master/LICENSE
//
//===----------------------------------------------------------------------===//

#ifndef XML_H
#define XML_H

/* PERF(mtwilliams): Restrict (and aliasing) hints. */

#if defined(_MSC_VER)
  #define XML_ON_MSVC  1
  #define XML_ON_CLANG 0
  #define XML_ON_GCC   0
#elif defined(__GNUC__)
  #if defined(__clang__)
    #define XML_ON_MSVC  0
    #define XML_ON_CLANG 1
    #define XML_ON_GCC   1
  #else
    /* HACK(mtwilliams): We assume that we're being compiled with GCC. */
    #define XML_ON_MSVC  0
    #define XML_ON_CLANG 0
    #define XML_ON_GCC   1
  #endif
#endif

#if defined(_WIN32) || defined(_WIN64)
  #define XML_ON_WINDOWS 1
  #define XML_ON_MAC 0
  #define XML_ON_LINUX 0
  #define XML_ON_ANDROID 0
  #define XML_ON_IOS 0
#elif defined(__APPLE__)
  #include <TargetConditionals.h>
  #if TARGET_OS_MAC
    #define XML_ON_WINDOWS 0
    #define XML_ON_MAC 1
    #define XML_ON_LINUX 0
    #define XML_ON_ANDROID 0
    #define XML_ON_IOS 0
  #elif TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
    #define XML_ON_WINDOWS 0
    #define XML_ON_MAC 0
    #define XML_ON_LINUX 0
    #define XML_ON_ANDROID 0
    #define XML_ON_IOS 1
  #elif TARGET_OS_TVOS
    /* Realistically, we could support tvOS... */
    #error ("We don't support tvOS!")
  #else
    #error ("We don't support this platform!")
  #endif
#elif defined(__linux__)
  #if defined(ANDROID)
    #define XML_ON_WINDOWS 0
    #define XML_ON_MAC 0
    #define XML_ON_LINUX 0
    #define XML_ON_ANDROID 1
    #define XML_ON_IOS 0
  #else
    #define XML_ON_WINDOWS 0
    #define XML_ON_MAC 0
    #define XML_ON_LINUX 1
    #define XML_ON_ANDROID 0
    #define XML_ON_IOS 0
  #endif
#else
  #error ("We don't support this platform!")
#endif

#if XML_ON_WINDOWS
  #define XML_ON_POSIX 0
#elif XML_ON_MAC || XML_ON_LINUX
  #define XML_ON_POSIX 1
#elif XML_ON_ANDROID || XML_ON_IOS
  /* Ostensibly... */
  #define XML_ON_POSIX 1
#endif

#if defined(_M_IX86) || defined(__i386__)
  #define XML_ON_X86 1
  #define XML_ON_X86_64 0
#elif defined(_M_X64) || defined(__x86_64__)
  #define XML_ON_X86 0
  #define XML_ON_X86_64 1
#endif

#if XML_ON_WINDOWS
  #define XML_EXPORT(Return) Return __stdcall
  #define XML_CALLBACK(Return) Return __cdecl
#elif XML_ON_MAC
  #define XML_EXPORT(Return) Return
  #define XML_CALLBACK(Return) Return
#elif XML_ON_LINUX
  #define XML_EXPORT(Return) Return
  #define XML_CALLBACK(Return) Return
#elif XML_ON_ANDROID
  #define XML_EXPORT(Return) Return
  #define XML_CALLBACK(Return) Return
#elif XML_ON_IOS
  #define XML_EXPORT(Return) Return
  #define XML_CALLBACK(Return) Return
#endif

#if defined(__cplusplus)
  #define XML_BEGIN_EXTERN_C extern "C" {
  #define XML_END_EXTERN_C }
#else
  #define XML_BEGIN_EXTERN_C
  #define XML_END_EXTERN_C
#endif

XML_BEGIN_EXTERN_C

typedef signed char    xml_int8_t;
typedef unsigned char  xml_uint8_t;
typedef signed short   xml_int16_t;
typedef unsigned short xml_uint16_t;
typedef signed int     xml_int32_t;
typedef unsigned int   xml_uint32_t;

#if defined(_MSC_VER)
  typedef signed __int64 xml_int64_t;
  typedef unsigned __int64 xml_uint64_t;
#else
  typedef signed long long xml_int64_t;
  typedef unsigned long long xml_uint64_t;
#endif

#if (_MSC_VER >= 1300 && _Wp64)
  #if XML_ON_X86
    typedef __w64 signed long xml_intptr_t;
    typedef __w64 unsigned long xml_uintptr_t;
    typedef __w64 unsigned long xml_size_t;
  #elif XML_ON_X86_64
    typedef __w64 signed __int64 xml_intptr_t;
    typedef __w64 unsigned __int64 xml_uintptr_t;
    typedef __w64 unsigned __int64 xml_size_t;
  #endif
#else
  #if XML_ON_X86
    typedef xml_int32_t xml_intptr_t;
    typedef xml_uint32_t xml_uintptr_t;
    typedef xml_uint32_t xml_size_t;
  #elif XML_ON_X86_64
    typedef xml_int64_t xml_intptr_t;
    typedef xml_uint64_t xml_uintptr_t;
    typedef xml_uint64_t xml_size_t;
  #endif
#endif

/* We (have to) define our own boolean type regardless of compiler support
   to prevent its width from being changed on us. Be careful not mix our type
   with others, since it's not compatible! */
typedef xml_uint32_t xml_bool_t;

#define XML_TRUE  ((xml_bool_t)1)
#define XML_FALSE ((xml_bool_t)0)

/* This will cause a negative subscript error if the sizes of our types don't
   match our expectations. */
#define XML_CHECK_SIZE_OF_TYPE(expression) \
  typedef char xml__size_matches_expectation[(expression) ? 1 : -1]

/* Ensure fixed-width types match our expectations. */
XML_CHECK_SIZE_OF_TYPE(sizeof(xml_int8_t)   == 1);
XML_CHECK_SIZE_OF_TYPE(sizeof(xml_uint8_t)  == 1);
XML_CHECK_SIZE_OF_TYPE(sizeof(xml_int16_t)  == 2);
XML_CHECK_SIZE_OF_TYPE(sizeof(xml_uint16_t) == 2);
XML_CHECK_SIZE_OF_TYPE(sizeof(xml_int32_t)  == 4);
XML_CHECK_SIZE_OF_TYPE(sizeof(xml_uint32_t) == 4);
XML_CHECK_SIZE_OF_TYPE(sizeof(xml_int64_t)  == 8);
XML_CHECK_SIZE_OF_TYPE(sizeof(xml_uint64_t) == 8);

/* Ensure pointer-width integer types can actually hold pointers. */
XML_CHECK_SIZE_OF_TYPE(sizeof(xml_intptr_t) == sizeof(void *));
XML_CHECK_SIZE_OF_TYPE(sizeof(xml_uintptr_t) == sizeof(void *));

#undef XML_CHECK_SIZE_OF_TYPE

/* TODO(mtwilliams): Errors. */

/* Callback event types. */
typedef enum xml_event {
  XML_BEGIN     = 1, /* Element begin. */
  XML_ATTRIBUTE = 2, /* Element attribute. */
  XML_END       = 3, /* Element end. */
} xml_event_t;

/* Reference to fragment of input. */
typedef struct {
  const unsigned char *s; /* Pointer to fragment. */
  xml_size_t l;           /* Length of fragment, as we don't mutate input. */
} xml_fragment_t;

/* Signature of the callback you pass to the parser. You should return zero to
 * continue parsing or any other value to halt (which results in XML_EUSER). */
typedef XML_CALLBACK(int)
  xml_callback_fn(/* Event type. */
                  xml_event_t e,
                  /* TODO(mtwilliams): Determine what this is. */
                  xml_size_t n,
                  /* Tag name. */
                  const xml_fragment_t *const tag,
                  /* Attribute name. */
                  const xml_fragment_t *const name,
                  /* Element body or attribute value. */
                  const xml_fragment_t *const body_or_value,
                  /* User-provided pointer. */
                  void *context);

/* Possible results of parsing. */
typedef enum xml_result {
  XML_OK      =  0, /* Okay! */
  XML_EMEMORY = -1, /* Ran out of scratch memory while parsing. */
  XML_EPARSE  = -2, /* Parsing failed because given document is malformed. */
  XML_EUSER   = -3, /* User requested that parsing halt for some reason. */
} xml_result_t;

/* Parses a document, calling the provided callback for the beginning and end
 * of every element as well as for every attribute.
 *
 * The document *must* be null-terminated!
 *
 * Returns XML_OK on success or another `xml_result_t` on failure.
 */
extern XML_EXPORT(xml_result_t) xml_parse(/* Document to be parsed. */
                                          const char *document,
                                          /* Pointer to scratch memory. */
                                          void *scratch,
                                          /* Amount of scratch memory in bytes. */
                                          xml_size_t amount_of_scratch,
                                          /* User-provided callback and pointer. */
                                          xml_callback_fn *callback,
                                          void *context);

/* Decodes a body into its raw representation.
 *
 * Returns the length of decoded body or a negative value on error.
 *
 * If the returned length is larger than the output buffer provided, you can
 * allocate the required memory and retry. Since output is only written when
 * the output buffer has sufficient room, you can safely pass `NULL` to
 * determine the length of the decoded body.
 */
extern XML_EXPORT(int) xml_decode_body(/* Body and its length in bytes. */
                                       const char *in, xml_size_t in_len,
                                       /* Output buffer and its size in bytes. */
                                       char *out, xml_size_t out_len);

/* Encodes (escapes) a string.
 *
 * Returns the length of output or a negative value on error.
 *
 * If the returned length is larger than the output buffer provided, you can
 * allocate the required memory and retry. Since output is only written when
 * the output buffer has sufficient room, you can safely pass `NULL` to
 * determine the length of the escaped string.
 */
extern XML_EXPORT(int) xml_encode_string(/* String and its length in bytes.  */
                                         const char *in, xml_size_t in_len,
                                         /* Output buffer and its size in bytes.  */
                                         char *out, xml_size_t out_len);

/* Encodes a string as CDATA.
 *
 * Returns the length of output or a negative value on error.
 *
 * If the returned length is larger than the output buffer provided, you can
 * allocate the required memory and retry. Since output is only written when
 * the output buffer has sufficient room, you can safely pass `NULL` to
 * determine the length of the escaped string.
 */
extern XML_EXPORT(int) xml_encode_cdata(/* String and its length in bytes.  */
                                        const char *in, xml_size_t in_len,
                                        /* Output buffer and its size in bytes. */
                                        char *out, xml_size_t out_len);

/* <xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema"> */
/* <tag type="xs:anyURI"></tag> */

/* Encodes a URI using percent encoding.
 *
 * Returns the length of output or a negative value on error.
 *
 * If the returned length is larger than the output buffer provided, you can
 * allocate the required memory and retry. Since output is only written when
 * the output buffer has sufficient room, you can safely pass `NULL` to
 * determine the length of the escaped string.
 */
extern XML_EXPORT(int) xml_encode_uri(/* URI and its length in bytes. */
                                      const char *in, xml_size_t in_len,
                                      /* Output buffer and its size in bytes. */
                                      char *out, xml_size_t out_len);

/* Decodes a percent encoded URI.
 *
 * Returns the length of output or a negative value on error.
 *
 * If the returned length is larger than the output buffer provided, you can
 * allocate the required memory and retry. Since output is only written when
 * the output buffer has sufficient room, you can safely pass `NULL` to
 * determine the length of the escaped string.
 */
extern XML_EXPORT(int) xml_decode_uri(/* Encoded URI and its length in bytes. */
                                      const char *in, xml_size_t in_len,
                                      /* Output buffer and its size in bytes. */
                                      char *out, xml_size_t out_len);

/* <tag type="xs:base64Binary"></tag> */

/* Estimates size of output buffer required for encoding. */
#define xml_encode_base64_est(Length) \
  ((((Length) + 2) / 3) * 4)

/* Encodes binary data into Base64.
 *
 * Returns the length of encoded data or a negative value on error.
 *
 * If the returned length is larger than the output buffer provided, you can
 * allocate the required memory and retry. Since output is only written when
 * the output buffer has sufficient room, you can safely pass `NULL` to
 * determine the length of the encoded data, but you should prefer estimating
 * the output buffer size directly by using `xml_encode_base64_est`.
 */
extern XML_EXPORT(int) xml_encode_base64(/* Raw data and its length in bytes. */
                                         const unsigned char *in, xml_size_t in_len,
                                         /* Output buffer and its size in bytes. */
                                         char *out, xml_size_t out_len);

/* Estimates size of output buffer required for decoding. */
#define xml_decode_base64_est(Length) \
  ((((Length) + 3) / 4) * 3)

/* Decodes from Base64 into binary data.
 *
 * Returns the length of decoded data or a negative value on error.
 *
 * If the returned length is larger than the output buffer provided, you can
 * allocate the required memory and retry. Since output is only written when
 * the output buffer has sufficient room, you can safely pass `NULL` to
 * determine the length of the decoded data, but you should prefer estimating
 * the output buffer size directly by using `xml_decode_base64_est`.
 */
extern XML_EXPORT(int) xml_decode_base64(/* Encoded data and its length in bytes. */
                                         const char *in, xml_size_t in_len,
                                         /* Output buffer and its size in bytes. */
                                         unsigned char *out, xml_size_t out_len);

/* <tag type="xs:hexBinary"></tag> */

/* Estimates size of output buffer required for encoding. */
#define xml_encode_hex_est(Length) \
  ((((Length) + 1) / 2) * 2)

/* Encodes binary data into hexadecimal.
 *
 * Returns the length of encoded data or a negative value on error.
 *
 * If the returned length is larger than the output buffer provided, you can
 * allocate the required memory and retry. Since output is only written when
 * the output buffer has sufficient room, you can safely pass `NULL` to
 * determine the length of the encoded data, but you should prefer estimating
 * the output buffer size directly by using `xml_encode_hex_est`.
 */

extern XML_EXPORT(int) xml_encode_hex(/* Raw data and its length in bytes. */
                                      const unsigned char *in, xml_size_t in_len,
                                      /* Output buffer and its size in bytes. */
                                      char *out, xml_size_t out_len);

/* Estimates size of output buffer required for decoding. */
#define xml_decode_hex_est(Length) \
  (((Length) + 1) / 2)

/* Decodes from hexadecimal into binary data.
 *
 * Returns the length of decoded data or a negative value on error.
 *
 * If the returned length is larger than the output buffer provided, you can
 * allocate the required memory and retry. Since output is only written when
 * the output buffer has sufficient room, you can safely pass `NULL` to
 * determine the length of the decoded data, but you should prefer estimating
 * the output buffer size directly by using `xml_decode_hex_est`.
 */

extern XML_EXPORT(int) xml_decode_hex(/* Encoded data and its length in bytes. */
                                      const char *in, xml_size_t in_len,
                                      /* Output buffer and its size in bytes. */
                                      unsigned char *out, xml_size_t out_len);

XML_END_EXTERN_C

#endif
