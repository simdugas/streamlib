/**
 * CRC library
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include "stream.h"

#define CRC32_GEN "00000100110000010001110110110111"
#define FRAME_SIZE 1520
#define NTEST 1000

/** Updates the feedback register for an incoming bit. */
void
update( stream_t *reg, stream_t *gen, char bit )
{
  //stream_print( reg ); printf( " - %d\n", bit );
  char feedback = getbit( reg, 0 ); // leftmost bit is the feedback

  for( int i = 0; i < reg->pos; i++ )
  {
    char cbit = i == reg->pos - 1? bit : getbit( reg, i + 1 );
    char choice = getbit( gen, i );
    /** The choice is wether or not to XOR. */
    if( 0 == choice )
    {
      setbit( reg, i, cbit );
    }
    else if( 1 == choice )
    {
      setbit( reg, i, cbit ^ feedback );
    }
    else
    {
      printf( "Error invalid bit for updating register: '%d'\n", choice );
    }
  }
}

/** Iterates over the stream as input to the LFSR. */
stream_t *
shift_stream( stream_t *in, stream_t *gen )
{
  stream_t *reg = stream_new();
  int len = gen->pos; // size of the generator
  
  // fill the register with a total of 'len' 0's
  for( int i = 0; i < len; i++ )
  {
    add( reg, 0 );
  }

  // for each bit in 'in'
  for( int i = 0; i < in->pos; i++ )
  {
    char c = getbit( in, i );
    update( reg, gen, c );
  }
  return( reg );
}

/** Prepares a stream for transmission by appending the checksum. */
stream_t *
transmit( stream_t *in, stream_t *gen )
{

  stream_t *out = stream_new();

  // Copy in to out
  for( int i = 0; i < in->pos; i++ )
  {
    add( out, getbit( in, i ) );
  }

  // Add 0's to the input
  for( int i = 0; i < gen->pos; i++ )
  {
    add( in, 0 );
  }

  // Store the remainder in reg
  stream_t *reg = shift_stream( in, gen );

  // add remainder to the output
  for( int i = 0; i < gen->pos; i++ )
  {
    add( out, getbit( reg, i ) );
  }

  return( out );
}

/** Check that a stream has no remainder (assuming CRC is appended at the end) */
int
check( stream_t *in, stream_t *gen )
{
  /** Calculate the remainder and store in reg */
  stream_t *reg = shift_stream( in, gen );
  int result = 0; // sum the bits 
  for( int i = 0; i < reg->pos; i++ )
  {
    result += getbit( reg, i );
  }
  return( result );  
}

/** Display usage information. */
void
usage()
{
    printf( "Usage: ./crc [option] [bitstream] [generator]\n" );
    printf( "  option: transmit - generate bitstream "
            "to transmit with CRC appended\n" );
    printf( "  option: check - check that a bitstream "
            "(with CRC) was not corrupted\n"  );
    printf( "  option: test - perform the test described in part d.\n" );
}

/** Introduce a random burst error of 'size' at a random location in the stream.*/
void
burst( stream_t *stream, int size )
{
  srand( time( NULL ) );
  int loc = rand() % ( FRAME_SIZE * 8 - size - 1 );
  
  for( int i = loc; i < loc + size; i++ )
  {
    setbit( stream, i, rand() % 2 );
  }
}

int
main( int argc, char *argv[] )
{
  if( argc == 4 )
  {
    stream_t *in = stream_string( argv[2] );
    stream_t *gen = stream_string( argv[3] );
    
    /** Process the transmit option */
    if( 0 == strncmp( argv[1], "transmit", 8 ) )
    {
      stream_t *out = transmit( in, gen );
      stream_print( out ); printf( "\n" );
    }

    /** Process the check option */
    else if( 0 == strncmp( argv[1], "check", 5 ) )
    {
      printf( "%s\n", check( in, gen )? "error detected" : "no error detected" );
    }
    else
    {
      usage();
    }
  }
  else if( argc == 2 )
  {
    /** Process the test option */
    if( 0 == strncmp( argv[1], "test", 4 ) )
    {
      stream_t *gen = stream_string( CRC32_GEN );
      srand( time( NULL ) );

      int errorslt = 0; int countlt = 0;
      int errorseq = 0; int counteq = 0;
      int errorsgt = 0; int countgt = 0;

      /** Generate test cases */
      for( int i = 0; i < NTEST; i++ )
      {
        stream_t *in = stream_random( FRAME_SIZE * 8 );
        stream_t *out = transmit( in, gen );
        
        /** mod by 3 to resolve the <, =, > 32 cases */
        int cat = i % 3;
        switch( cat )
        {
          case 0:
            countlt++;
            burst( out, 1 + ( rand() % 31 ) );
            errorslt += check( out, gen )? 1 : 0;
            break;
          case 1:
            counteq++;
            burst( out, 32 );
            errorseq += check( out, gen )? 1 : 0;
            break;
          case 2:
            countgt++;
            burst( out, 33 + ( rand() % ( FRAME_SIZE * 8 - 34 ) ) );
            errorsgt += check( out, gen )? 1 : 0;
            break;
        }
        free( in );
        free( out );
      }
      printf( "%d/%d burst errors < 32 were detected.\n", errorslt, countlt );
      printf( "%d/%d burst errors = 32 were detected.\n", errorseq, counteq );
      printf( "%d/%d burst errors > 32 were detected.\n", errorsgt, countgt );
    }
    else
    {
      usage();
    }
  }
  else
  {
    usage();
  }
}
