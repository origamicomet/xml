<h1 align="center">
  XML
</h1>

This is a small and fast evented (also known as callback) parser for XML, based on [G. David Butler's work](https://github.com/gdavidbutler/xmlTrivialCallbackParser) that can be easily embedded into and used by your project.

After embedding, you parse markup by calling `xml_parse` and passing a callback to parse markup in one pass with a constant memory footprint (i.e. only using the stack or memory provided). During parsing, the parser will call into your code for the beginning and end of every element as well as every attribute allowing you to map markup to internal data structures.

The parser can also be used to to validate markup by passing a `NULL` callback and verifying the return value of `xml_parse`.

A tree parser built on top of the evented parser is also provided. You can parse markup into a tree structure by calling `xml_parse_into_memory` and providing additional memory for the tree. Afterwords, you can walk the returned tree by following the `children` and `sibling` pointers in `xml_element_t`.

Also included are encoding and decoding routines often used during markup parsing and generation. They are designed so that you can determine the amount of memory necessary required in a similar fashion to `snprintf` by not providing a buffer.

This library is distributed under the terms described in LICENSE.
