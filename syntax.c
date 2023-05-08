/**
 * @file syntax.c
 * @author Dr. Sturgill
 * @author Sean Hinton (sahinto2)
 * Component containing the behaviors and constructors of different expression and statements
*/
#include "syntax.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//////////////////////////////////////////////////////////////////////
// Error-reporting functions

/** Report an error for a program with bad types, then exit. */
static int reportTypeMismatch()
{
  fprintf( stderr, "Type mismatch\n" );
  exit( EXIT_FAILURE );
}

/**
 * Report that index is out of bounds, then exit
*/
static int reportIndexOutOfBounds()
{
  fprintf( stderr, "Index out of bounds\n" );
  exit( EXIT_FAILURE );
}

/** Require a given value to be an IntType value.  Exit with an error
    message if not.
    @param v value to check, passed by address.
 */
static void requireIntType( Value const *v)
{
  if ( v->vtype != IntType ){

    reportTypeMismatch();
  }
}

/**
 * Checks for a typeMisMatch by requiring a sequence is the value
 * Exits with an error if not
 * @param s value that is being check passed by address
*/
static void requireSeqType(Value const *s)
{
  if(s->vtype != SeqType){
    reportTypeMismatch();
  }
}



//////////////////////////////////////////////////////////////////////
// LiteralInt

/** Representation for a LiteralInt expression, a subclass of Expr that
    evaluates to a constant value. */
typedef struct {
  Value (*eval)( Expr *expr, Environment *env );
  void (*destroy)( Expr *expr );

  /** Integer value this expression evaluates to. */
  int val;
} LiteralInt;

/** Implementation of eval for LiteralInt expressions. */
static Value evalLiteralInt( Expr *expr, Environment *env )
{
  // If this function gets called, expr must really be a LiteralInt.
  LiteralInt *this = (LiteralInt *)expr;

  // Return an int value containing a copy of the value we represent.
  return (Value){ IntType, .ival = this->val };
}

/** Implementation of destroy for LiteralInt expressions. */
static void destroyLiteralInt( Expr *expr )
{
  // This object is just one block of memory.  We can free it without
  // even having to type-cast its pointer.
  free( expr );
}

Expr *makeLiteralInt( int val )
{
  // Allocate space for the LiteralInt object
  LiteralInt *this = (LiteralInt *) malloc( sizeof( LiteralInt ) );

  // Remember the pointers to functions for evaluating and destroying ourself.
  this->eval = evalLiteralInt;
  this->destroy = destroyLiteralInt;

  // Remember the integer value we contain.
  this->val = val;

  // Return the result, as an instance of the Expr superclass.
  return (Expr *) this;
}

//////////////////////////////////////////////////////////////////////
// SimpleExpr Struct

/** Representation for an expression with either one or two
    sub-expressionts.  With the right eval funciton, this struct should
    be able to help implement any expression with either one or two
    sub-expressiosn. */
typedef struct {
  Value (*eval)( Expr *expr, Environment *env );
  void (*destroy)( Expr *oper );

  /** The first sub-expression */
  Expr *expr1;
  
  /** The second sub-expression, or NULL if it's not needed. */
  Expr *expr2;
} SimpleExpr;

/** General-purpose function for freeing an expression represented by
    SimpleExpr.  It frees the two sub-expressions, then frees the strucct
    itself. */
static void destroySimpleExpr( Expr *expr )
{
  // If this function gets called, expr must really be a SimpleExpr.
  SimpleExpr *this = (SimpleExpr *)expr;

  // Free the first sub-expression.
  this->expr1->destroy( this->expr1 );

  // Free the second one, if it exists.
  if ( this->expr2 )
    this->expr2->destroy( this->expr2 );

  // Then the SimpleExpr struct itself.
  free( this );
}

/** Helper funciton to construct a SimpleExpr representation and fill
    in the fields.
    @param first sub-expression in the expression.
    @param second sub-expression in the expression, or null if it only
    has one sub-expression.
    @param eval function implementing the eval mehod for this expression.
    @return new expression, as a poiner to Expr.
*/
static Expr *buildSimpleExpr( Expr *expr1, Expr *expr2,
                              Value (*eval)( Expr *, Environment * ) )
{
  // Allocate space for a new SimpleExpr and fill in the pointer for
  // its destroy function.
  SimpleExpr *this = (SimpleExpr *) malloc( sizeof( SimpleExpr ) );
  this->destroy = destroySimpleExpr;

  // Fill in the two parameters and the eval funciton.
  this->eval = eval;
  this->expr1 = expr1;
  this->expr2 = expr2;

  return (Expr *) this;
}

//////////////////////////////////////////////////////////////////////
// Integer addition

/** Implementation of the eval function for integer addition. */
static Value evalAdd( Expr *expr, Environment *env )
{
  // If this function gets called, expr must really be a SimpleExpr.
  SimpleExpr *this = (SimpleExpr *)expr;

  // Evaluate our left and right operands. 
  Value v1 = this->expr1->eval( this->expr1, env );
  Value v2 = this->expr2->eval( this->expr2, env );

  // Make sure the operands are both integers.
  // requireIntType( &v1 );
  // requireIntType( &v2 );

  //Return an int if they are both ints
  if(v1.vtype == IntType && v2.vtype == IntType){
    return (Value){ IntType, .ival = v1.ival + v2.ival };
  }

  //Otherwise there is some sequence in this
  //Both sequence
  if(v1.vtype == v2.vtype){
    Sequence *seq1 = v1.sval;
    Sequence *seq2 = v2.sval;
    grabSequence(seq1);
    grabSequence(seq2);

    Sequence *ret = makeSequence();

    //grabSequence(ret);
    //Now copy over all the elements of both array to a new sequence
    int len1 = seq1->len;
    for(int i = 0; i < len1; i++){
      if(ret->len >= ret->cap){
        ret->cap *= DOUBLE_CAP;
        ret->seq = (int*) realloc (ret->seq, ret->cap * sizeof(int));
      }
      ret->seq[i] = seq1->seq[i];
      ret->len++;
    }


    releaseSequence(seq1);
    for(int i = 0; i < seq2->len; i++){
      if(ret->len >= ret->cap){
        ret->cap *= DOUBLE_CAP;
        ret->seq = (int*) realloc (ret->seq, ret->cap * sizeof(int));
      }
      ret->seq[i + len1] = seq2->seq[i];
      ret->len++;
    }


    releaseSequence(seq2);

    //Now return the value
    return (Value) {SeqType, .sval = ret};

  }
  //Only the first is a sequence
  else if(v1.vtype == SeqType){
    Sequence *seq1 = v1.sval;
    grabSequence(seq1);
    Sequence *ret = makeSequence();

    //grabSequence(ret);
    int len1 = seq1->len;
    for(int i = 0; i < len1; i++){
      if(ret->len >= ret->cap){
        ret->cap *= DOUBLE_CAP;
        ret->seq = (int*) realloc (ret->seq, ret->cap * sizeof(int));
      }
      ret->seq[i] = seq1->seq[i];
      ret->len++;
    }

    //
    if(ret->len >= ret->cap){
        ret->cap *= DOUBLE_CAP;
        ret->seq = (int*) realloc (ret->seq, ret->cap * sizeof(int));
    }
    ret->seq[ret->len] = v2.ival;
    ret->len++;
    releaseSequence(seq1);
    return (Value) {SeqType, .sval = ret};

  }
  else if(v2.vtype == SeqType){
    Sequence *seq2 = v2.sval;
    grabSequence(seq2);
    Sequence *ret = makeSequence();

    //grabSequence(ret);
    //Now copy over all the elements of both array to a new sequence
    ret->seq[0] = v1.ival;
    ret->len++;
    for(int i = 0; i < seq2->len; i++){
      if(ret->len >= ret->cap){
        ret->cap *= DOUBLE_CAP;
        ret->seq = (int*) realloc (ret->seq, ret->cap * sizeof(int));
      }
      ret->seq[ret->len] = seq2->seq[i];
      ret->len++;
    }

    releaseSequence(seq2);
    return (Value) {SeqType, .sval = ret};

  }

  //Never reached
  return (Value){ IntType, .ival = 0 };
  

  
}

Expr *makeAdd( Expr *left, Expr *right )
{
  // Use the convenience function to build a SimpleExpr for addition
  return buildSimpleExpr( left, right, evalAdd );
}

//////////////////////////////////////////////////////////////////////
// Integer subtracton

/** Implementation of the eval function for integer subtraction. */
static Value evalSub( Expr *expr, Environment *env )
{
  // If this function gets called, expr must really be a SimpleExpr.
  SimpleExpr *this = (SimpleExpr *)expr;

  // Evaluate our left and right operands. 
  Value v1 = this->expr1->eval( this->expr1, env );
  Value v2 = this->expr2->eval( this->expr2, env );

  // Make sure the operands are both integers.
  requireIntType( &v1 );
  requireIntType( &v2 );

  // Return the difference of the two expression values.
  return (Value){ IntType, .ival = v1.ival - v2.ival };
}

Expr *makeSub( Expr *left, Expr *right )
{
  // Use the convenience function to build a SimpleExpr for subtraction.
  return buildSimpleExpr( left, right, evalSub );
}

//////////////////////////////////////////////////////////////////////
// Integer multiplication

/** Implementation of the eval function for integer multiplication. */
static Value evalMul( Expr *expr, Environment *env )
{
  // If this function gets called, expr must really be a SimpleExpr.
  SimpleExpr *this = (SimpleExpr *)expr;

  // Evaluate our left and right operands. 
  Value v1 = this->expr1->eval( this->expr1, env );
  Value v2 = this->expr2->eval( this->expr2, env );

  //Do a multiplication of sequences if one value is a sequnece
  if(v1.vtype == SeqType){
    Sequence *seq = v1.sval;
    grabSequence(seq);
    Sequence *ret = makeSequence();
    int times = v2.ival;
    for(int i = 0; i < times; i++){
      for(int j = 0; j < seq->len; j++){
        if(ret->len >= ret->cap){
          ret->cap *= DOUBLE_CAP;
          ret->seq = (int*)realloc(ret->seq, ret->cap * sizeof(int) );
        }
        ret->seq[ret->len] = seq->seq[j];
        ret->len++;
      }
    }

    releaseSequence(seq);
    return (Value){SeqType, .sval = ret};
  }
  else if(v2.vtype == SeqType){
    Sequence *seq = v2.sval;
    grabSequence(seq);
    Sequence *ret = makeSequence();
    int times = v1.ival;
    for(int i = 0; i < times; i++){
      for(int j = 0; j < seq->len; j++){
        if(ret->len >= ret->cap){
          ret->cap *= DOUBLE_CAP;
          ret->seq = (int*)realloc(ret->seq, ret->cap * sizeof(int) );
        }
        ret->seq[ret->len] = seq->seq[j];
        ret->len++;
      }
      // if(ret->len + seq->len >= ret->cap){
      //     ret->cap *= DOUBLE_CAP;
      //     ret->cap += seq->len;
      //     ret->seq = (int*)realloc(ret->seq, ret->cap * sizeof(int));
      // }
      // ret->seq[i * seq->len] = seq->seq;
    }

    releaseSequence(seq);
    return (Value){SeqType, .sval = ret};
  }
  else if(v1.vtype == SeqType && v2.vtype == SeqType){
    reportTypeMismatch();
  }

  // Make sure the operands are both integers.
  requireIntType( &v1 );
  requireIntType( &v2 );

  // Return the product of the two expression.
  return (Value){ IntType, .ival = v1.ival * v2.ival };
}

Expr *makeMul( Expr *left, Expr *right )
{
  // Use the convenience function to build a SimpleExpr for multiplication.
  return buildSimpleExpr( left, right, evalMul );
}

//////////////////////////////////////////////////////////////////////
// Integer division

/** Implementation of the eval function for integer division. */
static Value evalDiv( Expr *expr, Environment *env )
{
  // If this function gets called, expr must really be a SimpleExpr.
  SimpleExpr *this = (SimpleExpr *)expr;

  // Evaluate our left and right operands. 
  Value v1 = this->expr1->eval( this->expr1, env );
  Value v2 = this->expr2->eval( this->expr2, env );

  // Make sure the operands are both integers.
  requireIntType( &v1 );
  requireIntType( &v2 );

  // Catch it if we try to divide by zero.
  if ( v2.ival == 0 ) {
    fprintf( stderr, "Divide by zero\n" );
    exit( EXIT_FAILURE );
  }

  // Return the quotient of the two expression.
  return (Value){ IntType, .ival = v1.ival / v2.ival };
}

Expr *makeDiv( Expr *left, Expr *right )
{
  // Use the convenience function to build a SimpleExpr for division.
  return buildSimpleExpr( left, right, evalDiv );
}

//////////////////////////////////////////////////////////////////////
// Logical and

static Value evalAnd( Expr *expr, Environment *env )
{
  // If this function gets called, expr must really be a SimpleExpr.
  SimpleExpr *this = (SimpleExpr *)expr;

  // Evaluate the left operand; return immediately if it's false.
  Value v1 = this->expr1->eval( this->expr1, env );
  requireIntType( &v1 );
  if ( v1.ival == 0 )
    return v1;
  
  // Evaluate the right operand.
  Value v2 = this->expr2->eval( this->expr2, env );
  requireIntType( &v2 );

  // Return true if the right-hand operand is true.
  return v2;
}

Expr *makeAnd( Expr *left, Expr *right )
{
  // Use the convenience function to build a SimpleExpr for the logical and.
  return buildSimpleExpr( left, right, evalAnd );
}

//////////////////////////////////////////////////////////////////////
// Logical or

static Value evalOr( Expr *expr, Environment *env )
{
  // If this function gets called, expr must really be a SimpleExpr.
  SimpleExpr *this = (SimpleExpr *)expr;

  // Evaluate the left operand; return immediately if it's true.
  Value v1 = this->expr1->eval( this->expr1, env );
  requireIntType( &v1 );
  if ( v1.ival )
    return v1;
  
  // Evaluate the right operand
  Value v2 = this->expr2->eval( this->expr2, env );
  requireIntType( &v2 );

  // Return true if the right-hand operand is true.
  return v2;
}

Expr *makeOr( Expr *left, Expr *right )
{
  // Use the convenience function to build a SimpleExpr for the logical or
  return buildSimpleExpr( left, right, evalOr );
}

//////////////////////////////////////////////////////////////////////
// Less-than comparison

/** Implementation of eval for the less than operator. */
static Value evalLess( Expr *expr, Environment *env )
{
  // If this function gets called, expr must really be a SimpleExpr.
  SimpleExpr *this = (SimpleExpr *)expr;

  // Evaluate our left and right operands. 
  Value v1 = this->expr1->eval( this->expr1, env );
  Value v2 = this->expr2->eval( this->expr2, env );

  // Make sure the operands are both the same type.
  if ( v1.vtype != v2.vtype )
    
    reportTypeMismatch();

  if ( v1.vtype == IntType ) {
    // Is v1 less than v2
    return (Value){ IntType, .ival = v1.ival < v2.ival ? true : false };
  } else {
    // Replace with code to compare sequences.
    // fprintf( stderr, "Sequence comparison not implemented\n" );
    // exit( 0 );

    //Pick the length of the smaller one
    Sequence *seq1 = v1.sval;
    Sequence *seq2 = v2.sval;

    int length = seq1->len < seq2->len ? seq1->len : seq2->len;

    for(int i = 0; i < length; i++){
      if(seq1->seq[i] > seq2->seq[i]){
        //False since 1 is bigger than 2
        return (Value) {IntType, .ival = 0};
      }
      else if(seq1->seq[i] < seq2->seq[i]){
        //True since 1 is smaller than 2 at this index
        return (Value) {IntType, .ival = 1};
      }

      //If they are equal we go to the next index
    }

    if(seq1->len < seq2->len){
      //seq1 is a prefix so it's less than
      return (Value) {IntType, .ival = 1};
    }
    //Otherwise it is equal or greater than
    return (Value) {IntType, .ival = 0};
  }
}

Expr *makeLess( Expr *left, Expr *right )
{
  // Use the convenience function to build a SimpleExpr for the less-than
  // comparison.
  return buildSimpleExpr( left, right, evalLess );
}




/**
 * Custom expression for a Sequence Initializer Expression
*/
typedef struct{
  Value (*eval)( Expr *expr, Environment *env );
  void (*destroy)( Expr *expr );

 
  int length;
  
  //The pointer to a list of expressions
  Expr **elist;
} SequenceInitializerExpr;


/**
 * Create Value that has a pointer to a new sequence containing the stuff we wanted
 * @param expr a sequence intializer expression
 * @param env the environment of variables
*/
static Value evalSequence( Expr *expr, Environment *env)
{
  
  // We know this is a sequence
  SequenceInitializerExpr *this = (SequenceInitializerExpr *)expr;

  
  Sequence *seq = makeSequence();
  
  //Now create the sequence
  for(int i = 0; i < this->length; i++){
    //We probably should make sure everything is an int first
    //Case for regrowing the array
    if(seq->len >= seq->cap){
      seq->cap *= DOUBLE_CAP;
      seq->seq = (int*)realloc(seq->seq, seq->cap * sizeof(int));
    }
    Value v = this->elist[i]->eval(this->elist[i], env);
    requireIntType(&v);
    //Want to make sure to require and int and then destroy
    int val = v.ival;
    //this->elist[i]->destroy(this->elist[i]);
    seq->seq[i] = val;
    seq->len++;
    
  }
  
  // Return an int value containing a copy of the value we represent.
  return (Value){ SeqType, .sval = seq };
}


/**
 * Destroys the SequenceInitializerExpression
*/
static void destroySequence(Expr *expr)
{
  //We know it's a SequenceInitializerExpression
  SequenceInitializerExpr *destroy = (SequenceInitializerExpr *) expr;

  //Destroy each individual expression
  for(int i = 0; i < destroy->length; i++){
    destroy->elist[i]->destroy(destroy->elist[i]);
  }

  //Free that whole expression list
  free(destroy->elist);
  
  //Now free the block
  free(destroy);
}

Expr *makeSequenceInitializer(int len, Expr *elist[])
{
  SequenceInitializerExpr *ret = (SequenceInitializerExpr*) malloc (sizeof(SequenceInitializerExpr));
  
  
  ret->elist = (Expr**) malloc(len * sizeof(Expr*));
  ret->length = len;
  ret->destroy = destroySequence;
  ret->eval = evalSequence;
  
  //Now copy over the expressions into the pointers
  
  for(int i = 0; i < len; i++){
    ret->elist[i] = elist[i];
  }
  return (Expr*) ret;
}


////////////////////////////////////////////
//Len expression
/**
 * Evaluates the length of the passed in sequence
 * @param expr a pointer to a lenExpr that is really a simpleExpr
 * @param env the environment of variables
*/
static Value evalLen(Expr *expr, Environment *env)
{
  //We know its actually a simpleExpr if this gets called
  SimpleExpr *this = (SimpleExpr *) expr;
  
  //We probably want to make sure it's a sequence
  Value s = this->expr1->eval(this->expr1, env); 

  requireSeqType(&s);

  //Now we know it is indeed a sequence
  Sequence *current = s.sval;


  grabSequence(current);
  //And we know the second things is probably a varaible or sequence
  int len = current->len;

  //We also need to release it since the len expression no longer needs it
  releaseSequence(current);
  return (Value){IntType, .ival = len};
  
}

Expr *makeLenExpr(Expr *expr)
{
  return buildSimpleExpr(expr, NULL, evalLen);
}


//////////////////////////////////////////////////////////////////////
// Equality comparison

/** Eval function for an equality test. */
static Value evalEquals( Expr *expr, Environment *env )
{
  // If this function gets called, expr must really be a SimpleExpr.
  SimpleExpr *this = (SimpleExpr *)expr;

  // Evaluate our left and right operands. 
  Value v1 = this->expr1->eval( this->expr1, env );
  Value v2 = this->expr2->eval( this->expr2, env );

  // Make sure the same type.
  if ( v1.vtype == IntType && v2.vtype == IntType ) {
    return (Value){ IntType, .ival = ( v1.ival == v2.ival ) };
  } else {
    // Replace with code to permit sequence-sequence comparison.
    // A sequence can also be compared to an int, but they should
    // never be considered equal.
    // fprintf( stderr, "Sequence comparison not implemented\n" );
    // exit( 0 );
    
    //Check for mismatching types
    if((v1.vtype == IntType && v2.vtype == SeqType) || (v1.vtype == SeqType && v2.vtype == IntType)){
      return (Value){IntType, .ival = 0};
    }

    //If their lengths are not equal return false
    Sequence *seq1 = v1.sval;
    Sequence *seq2 = v2.sval;
    if(seq1->len != seq2->len){
      return (Value) {IntType, .ival = 0};
    }

    //Now compare elements for equality
    for(int i = 0; i < seq1->len; i++){
      if(seq1->seq[i] != seq2->seq[i]){
        return (Value) {IntType, .ival = 0};
      }
    }

    //Everything matches so return true
    return (Value) {IntType, .ival = 1};
  }

  // Never reached.
  return (Value){ IntType, .ival = 0 };
}

Expr *makeEquals( Expr *left, Expr *right )
{
  // Use the convenience function to build a SimpleExpr for the equals test.
  return buildSimpleExpr( left, right, evalEquals );
}

//////////////////////////////////////////////////////////////////////
// Variable in an expression

/** Representation for an expression representing an occurrence of a
    variable, subclass of Expr. */
typedef struct {
  Value (*eval)( Expr *expr, Environment *env );
  void (*destroy)( Expr *expr );

  /** Name of the variable. */
  char name[ MAX_VAR_NAME + 1 ];
} VariableExpr;

static Value evalVariable( Expr *expr, Environment *env )
{
  // If this function gets called, expr must really be a VariableExpr
  VariableExpr *this = (VariableExpr *) expr;

  // Get the value of this variable.
  Value val = lookupVariable( env, this->name );

  return val;
}

/** Implementation of destroy for Variable. */
static void destroyVariable( Expr *expr )
{
  free( expr );
}

Expr *makeVariable( char const *name )
{
  // Allocate space for the Variable statement, and fill in its function
  // pointers and a copy of the variable name.
  VariableExpr *this = (VariableExpr *) malloc( sizeof( VariableExpr ) );
  this->eval = evalVariable;
  this->destroy = destroyVariable;
  strcpy( this->name, name );

  return (Expr *) this;
}


/**
 * Returns the value of the integer at the given index in a sequence.
 * Makes sure that the sequence is within the bounds of the array
 * @param expr the SequenceIndexExpr
 * @param env the environment of variables
*/
static Value evalSequenceIndex(Expr *expr, Environment *env)
{
  //We know it's actually a simpleExpr
  SimpleExpr *this = (SimpleExpr *) expr;
  //Check if the integer comes withing the index and that it is an integer
  //Also check that it is a sequences
  Value seq = this->expr1->eval(this->expr1, env);
  Value index = this->expr2->eval(this->expr2, env);
  requireSeqType(&seq);
  requireIntType(&index);



  Sequence *sequence = seq.sval;
  grabSequence(sequence);
  int vindex = index.ival;
  if(vindex < 0 || vindex >= sequence->len){
    reportIndexOutOfBounds();
  }
  int val = sequence->seq[vindex];
  releaseSequence(sequence);

  
  return (Value){IntType, .ival = val};
}



Expr *makeSequenceIndex( Expr *aexpr, Expr *iexpr )
{
  //Allocate and create this using a SimpleExpr
  SimpleExpr *this = (SimpleExpr *) malloc(sizeof(SimpleExpr));
  this->eval = evalSequenceIndex;
  this->destroy = destroySimpleExpr;
  this->expr1 = aexpr;
  this->expr2 = iexpr;
  return (Expr *)this;
  
}


//////////////////////////////////////////////////////////////////////
// SimpleStmt Struct

/** Generic representation for a statement that contains one or two
    expressions.  With different execute methods, this same struct
    can be used to represent print and push statements. */
typedef struct {
  void (*execute)( Stmt *stmt, Environment *env );
  void (*destroy)( Stmt *stmt );

  /** First (or only) expression used by this statement. */
  Expr *expr1;
  /** Second expression used by this statement, or null */
  Expr *expr2;
} SimpleStmt;

/** Generic destroy function for SimpleStmt, with either one
    or two sub-expressions. */
static void destroySimpleStmt( Stmt *stmt )
{
  // If this function gets called, stmt must really be a SimpleStmt.
  SimpleStmt *this = (SimpleStmt *)stmt;

  // Free our subexpression then the SimpleStmt object itself.
  this->expr1->destroy( this->expr1 );
  if ( this->expr2 )
    this->expr2->destroy( this->expr2 );
  free( this );
}

//////////////////////////////////////////////////////////////////////
// Print Statement

/** Implementation of execute for a print statement */
static void executePrint( Stmt *stmt, Environment *env )
{
  // If this function gets called, stmt must really be a SimpleStmt.
  SimpleStmt *this = (SimpleStmt *)stmt;

  // Evaluate our argument.
  Value v = this->expr1->eval( this->expr1, env );

  // Print the value of our expression appropriately, based on its type.
  if ( v.vtype == IntType ) {
    printf( "%d", v.ival );
  } else {


    // Replace with code to permit print a sequence as a string of
    // ASCII character codes.
    //We are printing a sequence
    Sequence *seq = v.sval;
    grabSequence(seq);
    for(int i = 0; i < seq->len; i++){
      printf("%c", seq->seq[i]);
    }
    releaseSequence(seq);
  }
}

Stmt *makePrint( Expr *expr )
{
  // Allocate space for the SimpleStmt object
  SimpleStmt *this = (SimpleStmt *) malloc( sizeof( SimpleStmt ) );

  // Remember the pointers to execute and destroy this statement.
  this->execute = executePrint;
  this->destroy = destroySimpleStmt;

  // Remember the expression for the thing we're supposed to print.
  this->expr1 = expr;
  this->expr2 = NULL;

  // Return the SimpleStmt object, as an instance of the Stmt interface.
  return (Stmt *) this;
}

//////////////////////////////////////////////////////////////////////
// Compound Statement

/** Representation for a compound statement, derived from Stmt. */
typedef struct {
  void (*execute)( Stmt *stmt, Environment *env );
  void (*destroy)( Stmt *stmt );

  /** Number of statements in the compound. */
  int len;
  
  /** List of statements in the compound. */
  Stmt **stmtList;
} CompoundStmt;

/** Implementation of execute for CompountStmt */
static void executeCompound( Stmt *stmt, Environment *env )
{
  // If this function gets called, stmt must really be a CompoundStmt.
  CompoundStmt *this = (CompoundStmt *)stmt;

  // Execute the sequence of statements in this compound
  for ( int i = 0; i < this->len; i++ )
    this->stmtList[ i ]->execute( this->stmtList[ i ], env );
}

/** Implementation of destroy for CompountStmt */
static void destroyCompound( Stmt *stmt )
{
  // If this function gets called, stmt must really be a CompoundStmt.
  CompoundStmt *this = (CompoundStmt *)stmt;

  // Free the list of statements inside this compond.
  for ( int i = 0; i < this->len; i++ )
    this->stmtList[ i ]->destroy( this->stmtList[ i ] );

  // Then, free the array of pointers and the compund statement itself.
  free( this->stmtList );
  free( this );
}

Stmt *makeCompound( int len, Stmt **stmtList )
{
  // Allocate space for the CompoundStmt object
  CompoundStmt *this = (CompoundStmt *) malloc( sizeof( CompoundStmt ) );

  // Remember the pointers to execute and destroy this statement.
  this->execute = executeCompound;
  this->destroy = destroyCompound;

  // Remember the list of statements in the compound.
  this->len = len;
  this->stmtList = stmtList;

  // Return the result, as an instance of the Stmt interface.
  return (Stmt *) this;
}

///////////////////////////////////////////////////////////////////////
// ConditioanlStatement (for while/if)

/** Representation for either a while or if statement, subclass of Stmt. */
typedef struct {
  void (*execute)( Stmt *stmt, Environment *env );
  void (*destroy)( Stmt *stmt );

  // Condition to be checked before running the body.
  Expr *cond;

  // Body to execute if / while cond is true.
  Stmt *body;
} ConditionalStmt;

/** Implementation of destroy for either while of if statements. */
static void destroyConditional( Stmt *stmt )
{
  // If this function gets called, stmt must really be a ConditionalStmt.
  ConditionalStmt *this = (ConditionalStmt *)stmt;

  // Destroy the condition expression and the statement in the body.
  this->cond->destroy( this->cond );
  this->body->destroy( this->body );

  // Then, free the ConditionalStmt struct.
  free( this );
}

///////////////////////////////////////////////////////////////////////
// if statement

/** Implementation of th execute function for an if statement. */
static void executeIf( Stmt *stmt, Environment *env )
{
  // If this function gets called, stmt must really be a ConditionalStmt.
  ConditionalStmt *this = (ConditionalStmt *)stmt;

  // Evaluate our operand and see if it's true.
  Value result = this->cond->eval( this->cond, env );
  requireIntType( &result );

  // Execute the body if the condition evaluated to true.
  if ( result.ival )
    this->body->execute( this->body, env );
}

Stmt *makeIf( Expr *cond, Stmt *body )
{
  // Allocate an instance of ConditionalStmt
  ConditionalStmt *this =
    (ConditionalStmt *) malloc( sizeof( ConditionalStmt ) );

  // Functions to execute and destroy an if statement.
  this->execute = executeIf;
  this->destroy = destroyConditional;

  // Fill in the condition and the body of the if.
  this->cond = cond;
  this->body = body;

  // Return the result, as an instance of the Stmt interface.
  return (Stmt *) this;
}

///////////////////////////////////////////////////////////////////////
// while statement

/** Implementation of th execute function for a while statement. */
static void executeWhile( Stmt *stmt, Environment *env )
{
  // If this function gets called, stmt must really be a ConditionalStmt.
  ConditionalStmt *this = (ConditionalStmt *)stmt;

  // Evaluate our condition and see if it's true.
  Value result = this->cond->eval( this->cond, env );
  requireIntType( &result );
  
  // Execute the body while the condition evaluates to true.
  while ( result.ival ) {
    this->body->execute( this->body, env );
    
    // Get the value of the condition for the next iteration.
    result = this->cond->eval( this->cond, env );
    requireIntType( &result );
  }
}

Stmt *makeWhile( Expr *cond, Stmt *body )
{
  // Allocate an instance of ConditionalStmt
  ConditionalStmt *this =
    (ConditionalStmt *) malloc( sizeof( ConditionalStmt ) );

  // Functions to execute and destroy a while statement.
  this->execute = executeWhile;
  this->destroy = destroyConditional;

  // Fill in the condition and the body of the while.
  this->cond = cond;
  this->body = body;

  // Return the result, as an instance of the Stmt interface.
  return (Stmt *) this;
}

/*************************Push statement*/




/**
 * Pushes the integer to the end of the sequence and makes sure to grow if needed
 * Also checks for type mismatches between the first and second expression
 * @param stmt the statement to execute
 * @param env the environment that holds all the variables
*/
static void executePush(Stmt *stmt, Environment *env)
{
  //We know this is actually a SimpleStatement
  SimpleStmt *this = (SimpleStmt*) stmt;


  //Need to check for type mismatches
  Value seq = this->expr1->eval(this->expr1, env);
  Value val = this->expr2->eval(this->expr2, env);
  //Now assert their types
 
  requireSeqType(&seq);
  requireIntType(&val);
  

  Sequence *realSeq = seq.sval;

  //We must grab the sequence too
  grabSequence(realSeq);

  //Not enough room so realloc some space
  if(realSeq->len >= realSeq->cap){
    realSeq->cap *= DOUBLE_CAP;
    realSeq->seq = (int*)realloc(realSeq->seq, realSeq->cap * sizeof(int));
  }

  //Now assign to the end and increase the length by 1
  realSeq->seq[realSeq->len] = val.ival;
  realSeq->len++;


  //Then release the sequence
  releaseSequence(realSeq);

}

Stmt *makePush(Expr *sexpr, Expr *vexpr)
{
  SimpleStmt *this = (SimpleStmt*)malloc(sizeof(SimpleStmt));
  this->expr1 = sexpr;
  this->expr2 = vexpr;
  this->execute = executePush;
  this->destroy = destroySimpleStmt;
  return (Stmt*) this;
}


///////////////////////////////////////////////////////////////////////
// assignment statement

/** Representation of an assignment statement, a subclass of
    Stmt. This representation should be suitable for assigning to a
    variable or an element of a sequence.  */
typedef struct {
  void (*execute)( Stmt *stmt, Environment *env );
  void (*destroy)( Stmt *stmt );

  /** Name of the variable we're assigning to. */
  char name[ MAX_VAR_NAME + 1 ];
  
  /** If we're assigning to an element of a sequence, this is the index
      expression. Otherwise, it's zero. */
  Expr *iexpr;

  /** Expression for the right-hand side of the assignment (the source). */
  Expr *expr;
} AssignmentStmt;

/** Implementation of destroy for assignment Statements. */
static void destroyAssignment( Stmt *stmt )
{
  AssignmentStmt *this = (AssignmentStmt *)stmt;

  // Destroy the source expression and the index (if there is one)
  this->expr->destroy( this->expr );
  if ( this->iexpr )
    this->iexpr->destroy( this->iexpr );
  free( this );
}

/** Implementation of execute for assignment Statements. */
static void executeAssignment( Stmt *stmt, Environment *env )
{
  // If we get to this function, stmt must be an AssignmentStmt.
  AssignmentStmt *this = (AssignmentStmt *) stmt;

  // Evaluate the right-hand side of the equals.
  Value result = this->expr->eval( this->expr, env );
 
  if ( this->iexpr ) {
    // Replace with code to permit assigning to a sequence element.
    // fprintf( stderr, "Assignment to sequence elements not implemented\n" );
    // exit( 0 );
    
    //Go to the index of the sequence then assign the variable
    //Also make sure it is in bounds
    Value i = this->iexpr->eval(this->iexpr, env);
    requireIntType(&i);
    int index = i.ival;
    //We have to go find our variable now 
    //If it does not exist syntax error
    Value s = lookupVariable(env, this->name);
    requireSeqType(&s);

    Sequence *seq = s.sval;
    //Check out of bounds errors
    if(index < 0 || index >= seq->len){
      reportIndexOutOfBounds();
    }

    //Now do the assignment
    seq->seq[index] = result.ival;

  } else {
    // It's a variable, change its value

    //If it's a sequence we need to increase the reference count and decrease the reference count

    
    //We also might need to report a type mismatch
    if(result.vtype == SeqType){
      grabSequence(result.sval);
    }
    


    setVariable( env, this->name, result );
  }
}

Stmt *makeAssignment( char const *name, Expr *iexpr, Expr *expr )
{
  // Allocate the AssignmentStmt representations.
  AssignmentStmt *this =
    (AssignmentStmt *) malloc( sizeof( AssignmentStmt ) );

  // Fill in functions to execute or destory this statement.
  this->execute = executeAssignment;
  this->destroy = destroyAssignment;

  // Get a copy of the destination variable name, the source
  // expression and the sequence index (if it's non-null).
  strcpy( this->name, name );
  this->iexpr = iexpr;
  this->expr = expr;

  // Return this object, as an instance of Stmt.
  return (Stmt *) this;
}
