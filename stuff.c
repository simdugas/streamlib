/**
 * Bit stuffing and unstuffing library.
 */

#include <stdio.h>
#include <stdlib.h>

#include "stream.h"

stream_t *stuff( FILE *in );

/** Return a stuffed stream for a given file input. */
stream_t *stuff( FILE *in )
{
char c;
stream_t *stream;

  stream = stream_new();
  int count = 0;
  // read char from file
  while( stream && in && EOF != ( c = fgetc( in ) ) )
  {
  int i, n;
    // for each bit in the byte
    for( i = NBITS - 1; i >= 0; i-- )
    {
      char bit = get( c, i );
      // stuff the bit
      if( 5 == count )
      {
        add( stream, 0 );
        count = 0;
      }
      if( 1 == bit )
      {
        count++;
      }
      else
      {
        count = 0;
      }
      // add to stream
      add( stream, bit );
    }
  }
  return( stream );
}

/** Print unstuffed output for file for a given stuffed stream. */
void unstuff( stream_t *stream, FILE *out )
{
int count = 0;
stream_t *sout = stream_new();
  if( sout )
  {
    for( int i = 0; i < stream->pos; i++ )
    {
        if( count == 5 )
        {
          // skip this bit
          count = 0;
        }
        else
        {
          char bit = get( stream->s[i/NBITS], ( NBITS - 1 ) - ( i % NBITS ) );
          add( sout, bit );
          if( bit == 1 )
          {
            count++;
          }
          else
          {
            count = 0;
          }
        }
    }
    for( int i = 0; i < sout->pos / NBITS; i++ )
    {
      fprintf( out, "%c", sout->s[i] );
    }
  }
  free( sout );
}

void
usage()
{
  printf( "Usage: ./stuff [infile]\n" );
  printf( "  [infile] input file to stuff\n" );
  printf( "  the output file will be saved to 'data.out'\n" );
}

int main( int argc, char** argv )
{
  if( argc == 2 )
  {
    FILE *in = fopen( argv[1], "r" );
    FILE *out = fopen( "data.out", "w" );
    stream_t *stream = stuff( in );
    unstuff( stream, out );
    fclose( in );
    fclose( out );
    free( stream );
  }
  else
  {
    usage();
  }
}
