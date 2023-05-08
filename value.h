/**
  @file value.h
  @author Dr. Sturgill
  @author Sean Hinton (sahinto2)

  Support for representing different types of values that could be computed
  by our programming language.
*/

#ifndef _VALUE_H_
#define _VALUE_H_

/**Initial capacity for sequences and environment*/
#define INIT_CAP 5
/**Growth rate for reallocating memory*/
#define DOUBLE_CAP 2

#include <stdbool.h>

/** Representation for a seqeunce of integers.  One type of value supported
    by the language. */
typedef struct {
  int len;
  int cap;
  
  int *seq;
  int ref;
} Sequence;

/** Create an empty sequence.
    @return pointer to the new, dynamically allocated sequence.
*/
Sequence *makeSequence();

/** Free all the memory used to store the given sequence.
    @param seq sequence to free.
*/
void freeSequence( Sequence *seq );

/** Add one to the reference count for the given sequence.
    @param seq sequence in which to increate the reference count.
*/
void grabSequence( Sequence *seq );

/** Subtract one to the reference count for the given sequence.  If the
    reference count reaches zero, free the memory for the sequence.
    @param seq sequence in which to decrease the reference count.
*/
void releaseSequence( Sequence *seq );

//////////////////////////////////////////////////////////////////////
// Value Representat

/** Type of value in our langauge */
typedef enum { IntType, SeqType } ValType;

/** A short name to use for the Value interface. */
typedef struct ValueStruct Value;

/** Representation of a value in our programming language, either an int
    or a sequence of ints.
*/
struct ValueStruct {
  ValType vtype;
  
  union {
    /** If this value is just one int, this is its value. */
    int ival;

    /** If this value is a sequence, this is its value. */
    Sequence *sval;
  };
};

//////////////////////////////////////////////////////////////////////
// Environment, a mapping from variables names to their value.

// Maximum length of an identifier (variable) name.
#define MAX_VAR_NAME 20

/**
   Short typename for the Environment structure.  Its definition is an
   implementation detail of the language, not visible to client code.
*/
typedef struct EnvironmentStruct Environment;


/** Create and return a new, empty environment object.
    @return new, dynamically allocated environment object.  The caller
    must eventually free this with freeEnvironemtn().
*/
Environment *makeEnvironment();


/** Lookup the variable with the given name in the environment and
    return its value.  If the variable doesn't have a value, this
    function returns null.
    @param env Environment object in which to lookup the variable.
    @param name requested variable name.
    @return the variable's value.  This is a pointer into the
    environment's representation and should not be directly freed or
    modified by the caller.
*/
Value lookupVariable( Environment *env, char const *name );

/** In the given environment, set the named variable to store the given
    value.
    @param env Environment in which to store the variable name / value.
    @param name of the variable to set the value for.
    @param value new value for this variable.
*/
void setVariable( Environment *env, char const *name, Value value );

/** Free all the memory associated with this environment.
    @param env environment to free memory for.
*/
void freeEnvironment( Environment *env );

#endif
