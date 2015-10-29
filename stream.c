/**
 * Common stream library.
 */

#include "stream.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/** Allocates a new stream */
stream_t *stream_new()
{
  stream_t *stream = malloc( sizeof( stream[0] ) );
  if( stream )
  {
    stream->pos = 0;
    stream->size = 32;
    stream->s = calloc( stream->size, sizeof( stream->s[0] ) );
  }
  return( stream );
}

/** Creates a stream from a string of 0's and 1's. */
stream_t *stream_string( char *str )
{
  stream_t *stream = NULL;
  if( str )
  {
    stream = stream_new();
    if( stream )
    {
      while( *str != '\0' )
      {
        char c = *str - '0';
        if( c != 0 && c != 1 )
        {
          printf( "Invalid character '%c'.", *str );
        }
        else
        {
          add( stream, c );
        }
        str++;
      }
    }
  }
  return( stream );
}

/** Generates a random stream of 'size' bits. */
stream_t *
stream_random( int size )
{
  stream_t *stream = stream_new();
  srand( time( NULL ) );
  for( int i = 0; i < size; i++ )
  {
    char r = (char) rand() % 2;
    add( stream, r );
  }
  return( stream );
}

/** Prints a bit stream to the screen for debugging. */
void stream_print( stream_t *stream )
{
  //printf( "pos=%d, size=%d, s: ", stream->pos, stream->size );
  for( int i = 0; i < stream->pos; i++ )
  {
    char c = get( stream->s[i/NBITS], ( NBITS - 1 ) - ( i % NBITS ) );
    printf( "%d ", c );
  }
}

/** Gets a bit at a position in the stream. (Leftmost bit is pos = 0). */
char
getbit( stream_t *stream, int pos )
{
    return( get( stream->s[ pos / NBITS ], ( NBITS - 1 ) - ( pos % NBITS ) ) );
}

/** Sets the bit at a position in the stream. */
char
setbit( stream_t *stream, int pos, char bit )
{
  if( 0 == bit )
  {
    stream->s[ pos / NBITS ] &= ~( 1 << ( ( NBITS - 1 ) - ( pos % NBITS ) ) );
  }
  else
  {
    stream->s[ pos / NBITS ] |= 1 << ( ( NBITS - 1 ) - ( pos % NBITS ) );
  }
  return getbit( stream, pos );
}

/** Adds a bit to the stream. */
void add( stream_t *stream, char bit )
{
  // Mem location
  int i = stream->pos / NBITS;
  // Bit location
  int ibit = 
      ( NBITS - 1 ) 
      - ( stream->pos % NBITS );

  // Double the size if we reached the end
  if( i == stream->size )
  {
    stream->size *= 2;
    stream->s = (char *) reallocf( 
        stream->s,
        sizeof( stream->s[0] ) * stream->size
        );
    // Zero the allocated space
    for( int j = i; j < stream->size; j++ )
    {
      stream->s[j] = 0;
    }
  }
  
  // Add bit
  stream->s[i] |= bit << ibit;

  // Inc position
  stream->pos++;
}

/** Get a bit at position i in c. */
char get( char c, int i )
{
  return ( c & ( 1 << i ) ) >> i;
}

/** Print a bit from left (MSB or index 7) to right (LSB or index 0) */
int print( char c )
{
  int i;
  for( i = NBITS - 1; i >= 0; i-- )
  {
    printf( "%d ", get( c, i ) );
  }
  return( 0 );
}

