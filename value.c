/**
 * @file value.c
 * @author Dr. Sturgill
 * @author Sean Hinton (sahinto2)
 * part of the Value component containing the behaviors and constructors for 
 * Sequences of ints and the Environment of variables.
*/
#include "value.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>


//////////////////////////////////////////////////////////////////////
// Sequence.


Sequence *makeSequence()
{
  Sequence *seq = (Sequence *) malloc(sizeof(Sequence)); 
  seq->cap = INIT_CAP;
  seq->len = 0;
  seq->ref = 0;
  seq->seq = (int *) malloc (seq->cap * sizeof(int));
  return seq; 
}

void freeSequence( Sequence *seq )
{

  //Free all the expression
  if(seq->seq != NULL){
  free(seq->seq);
  }
  
  free(seq);
}

void grabSequence( Sequence *seq )
{
  seq->ref += 1;
}

void releaseSequence( Sequence *seq )
{
  seq->ref -= 1;

  if ( seq->ref <= 0 ) {
    assert( seq->ref == 0 );

    // Once the reference count hits zero, we can free the sequence memory.
    
    freeSequence( seq );
  }
}

//////////////////////////////////////////////////////////////////////
// Environment.

typedef struct {
  char name[ MAX_VAR_NAME + 1 ];
  Value val;
} VarRec;

// Hidden implementation of the environment.
struct EnvironmentStruct {
  VarRec *vlist;

  // Number of name/value pairs.
  int len;

  // Capacity of the name/value list.
  int capacity;
};

Environment *makeEnvironment()
{
  Environment *env = (Environment *) malloc( sizeof( Environment ) );
  env->capacity = INIT_CAP;
  env->len = 0;
  env->vlist = (VarRec *) malloc( sizeof( VarRec ) * env->capacity );
  return env;
}

Value lookupVariable( Environment *env, char const *name )
{
  // Linear search for a variable name, not too efficient.
  for ( int i = 0; i < env->len; i++ )
    if ( strcmp( env->vlist[ i ].name, name ) == 0 )
      return env->vlist[ i ].val;

  // Return zero for uninitialized variables.
  return (Value){ IntType, .ival = 0 };
}

void setVariable( Environment *env, char const *name, Value value )
{
  int pos = 0;
  while ( pos < env->len && strcmp( env->vlist[ pos ].name, name ) != 0 )
    pos++;

  if ( pos >= env->len ) {
    if ( env->len >= env->capacity ) {
      env->capacity *= DOUBLE_CAP;
      env->vlist = (VarRec *) realloc( env->vlist, sizeof( VarRec ) * env->capacity );
    }
  }

  // Is this a new variable, or one that already existed?
  if ( pos == env->len ) {
    pos = env->len++;
    strcpy( env->vlist[ pos ].name, name );
  }
  else{
    //If this variable already exists we need to release the sequence if needed
    if(env->vlist[pos].val.vtype == SeqType){
      releaseSequence(env->vlist[pos].val.sval);
    }
  }
  
  

  env->vlist[ pos ].val = value;
}

void freeEnvironment( Environment *env )
{
  //Need a case for if the variable is actually a sequence
  for(int i = 0; i < env->len; i++){
    if(env->vlist[i].val.vtype == SeqType){
      releaseSequence(env->vlist[i].val.sval);
    }
    
  }
  free( env->vlist );

  free( env );
}

