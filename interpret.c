/**
 * @file interpret.c
 * @author Dr. Sturgill
 * @author Sean Hinton (sahinto2)
 * This is the top level component of the parser program that reads in lines of input.
 * It reads in lines of input until no more lines are found.
 * It is also responsible for controlling the lifecycle of the environment of variables.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "value.h"
#include "syntax.h"
#include "parse.h"


#define ARGS 2
/** Print a usage message then exit unsuccessfully. */
void usage()
{
  fprintf( stderr, "usage: interpret <program-file>\n" );
  exit( EXIT_FAILURE );
}

int main( int argc, char *argv[] )
{
  // Open the program's source.
  if ( argc != ARGS )
  
    usage();
  
  FILE *fp = fopen( argv[ 1 ], "r" );
  if ( !fp ) {
    perror( argv[ 1 ] );
    exit( EXIT_FAILURE );
  }

  // Environment, for storing variable values.
  Environment *env = makeEnvironment();
  
  // Parse one statement at a time, then run each statement
  // using the same Environment.
  char tok[ MAX_TOKEN + 1 ];
  while ( parseToken( tok, fp ) ) {
    // Parse the next input statement.
    Stmt *stmt = parseStmt( tok, fp );

    // Run the statement.
    stmt->execute( stmt, env );

    // Delete the statement.
    stmt->destroy( stmt );
  }
  
  // We're done, close the input file and free the environment.
  fclose( fp );
  freeEnvironment( env );

  return EXIT_SUCCESS;
}
