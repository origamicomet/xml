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

/* callback event types */
typedef enum {
  xmlTp_Eb /* element begin */
 ,xmlTp_Ea /* element attribute */
 ,xmlTp_Ee /* element end */
 ,xmlTp_Er /* syntax error */
} xmlTp_t;

/* pointers referencing fragments of constant buffer must include length */
typedef struct {
  const unsigned char *s;
  unsigned int l;
} xmlSt_t;

/* prototype of a callback function */
/* returns 0 on success else aborts parse */
typedef int (*xmlCb_t)(
  xmlTp_t
 ,unsigned int numberOfElementTagNames
 ,const xmlSt_t *elementTagName
 ,const xmlSt_t *elementAttributeName
 ,const xmlSt_t *EeBodyOrEaValue
 ,void *userContext
);

/* return -1 on error else offset of last char parsed */
/* buf must be null terminated */
int xmlParse(xmlCb_t, const unsigned char *buf, void *userContext);

/* return -1 on error else the length of out */
/* if length returned is more than length provided, allocate needed memory and retry */
int xmlDecodeBody(unsigned char *out, int olen, const unsigned char *in, int ilen);

/* Encodes (escapes) a string.
 *
 * Returns the length of output or a negative value on error.
 *
 * If the returned length is larger than the output buffer provided, you can
 * allocate the required memory and retry. Since output is only written when
 * the output buffer has sufficient room, you can safely pass `NULL` to
 * determine the length of the escaped string.
 */
extern XML_EXPORT(int) xml_encode_string(const char *in, xml_size_t in_len,
                                         char *out, xml_size_t out_len);

/* Encodes as CDATA.
 *
 * Returns the length of output or a negative value on error.
 *
 * If the returned length is larger than the output buffer provided, you can
 * allocate the required memory and retry. Since output is only written when
 * the output buffer has sufficient room, you can safely pass `NULL` to
 * determine the length of the escaped string.
 */
extern XML_EXPORT(int) xml_encode_cdata(const unsigned char *in, xml_size_t in_len,
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
extern XML_EXPORT(int) xml_encode_uri(const char *in, xml_size_t in_len,
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
extern XML_EXPORT(int) xml_decode_uri(const char *in, xml_size_t in_len,
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
extern XML_EXPORT(int) xml_encode_base64(const unsigned char *in, xml_size_t in_len,
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
extern XML_EXPORT(int) xml_decode_base64(const char *in, xml_size_t in_len,
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

extern XML_EXPORT(int) xml_encode_hex(const unsigned char *in, xml_size_t in_len,
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

extern XML_EXPORT(int) xml_decode_hex(const char *in, xml_size_t in_len,
                                       unsigned char *out, xml_size_t out_len);

XML_END_EXTERN_C

#endif
