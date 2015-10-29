/**
 * Common stream header file.
 */

#ifndef _STREAM_H_
#define _STREAM_H_

/** Public interface to the stream ADT. */

/** Defines the number of bits in a "word" */
#define NBITS ( sizeof( char ) * 8 )

/** A bit stream to hold the stuffed values */
typedef struct stream
{
  int pos; /** bit position of the next spot to fill */
  int size; /** max size, in bytes, of the buffer. */
  char *s; /** pointer to the buffer. */
} stream_t;

/* function prototypes */
stream_t *stream_new();
stream_t *stream_string( char *str );
stream_t *stream_random( int size );
void stream_print( stream_t *stream );
char getbit( stream_t *stream, int pos );
char setbit( stream_t *stream, int pos, char bit );
void add( stream_t *stream, char bit );
char get( char c, int i );
int print( char c );

#endif

