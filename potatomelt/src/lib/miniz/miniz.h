#ifndef MINIZ_H
#define MINIZ_H

// Define MINIZ_NO_STDIO, MINIZ_NO_TIME, etc. based on Pngle's needs or project defaults.
// Pngle primarily needs tinfl_decompress. Default miniz.c provided by user has many disabled.
// For Pngle, we primarily need the decompression part (tinfl).
// The user-provided miniz.c already has many features like NO_STDIO, NO_TIME, NO_ARCHIVE_APIS, NO_ZLIB_APIS, NO_COMPRESSION defined.
// These seem fine for Pngle's tinfl usage.

#define MINIZ_HEADER_FILE_ONLY
#include "miniz.c"

#endif // MINIZ_H
