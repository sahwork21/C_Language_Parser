/**
  @file syntax.h
  @author Dr. Sturgill
  @author Sean Hinton (sahinto2)

  Abstract representation for expressions and statements in our
  language.
*/

#ifndef _SYNTAX_H_
#define _SYNTAX_H_

#include "value.h"



//////////////////////////////////////////////////////////////////////
// Expr, an interface for an expression in the input program.

/** A short name to use for the expression interface. */
typedef struct ExprStruct Expr;

/** Representation for an Expr interface.  Classes implementing this
    have these two fields as their first members.  They will set eval
    to point to appropriate functions to evaluate the expression, based on
    what kind of expression it is.  They will set destroy to
    point to a function that frees memory for their type of expresson.
*/
struct ExprStruct {
  /** Pointer to a function to evaluate the given expression and
      return the result as an instance of Value.
      @param expr expression to be evaluated.
      @param env current values of all variables.
      @return value this expression evaluates to.
   */
  Value (*eval)( Expr *expr, Environment *env );

  /** Free memory for this expression, including any subexpressions
      it contains.
      @param expr expression to free.
  */
  void (*destroy)( Expr *expr );
};

/** Make a representation of a literal int value, a value that gives
    back a Value containing a particular int whever it is evaluated.
    @param val value this expression evaluates to.
    @return a new, dynamically allocated expression that evaluates to
    a Value contianing the given integer.
 */
Expr *makeLiteralInt( int val );

/** Make an expression that adds up the values its two parameter
    expressions evaluate to.  This new object will take ownership of
    the memory pointed to by its two sub-expressions, and it will be
    responsible for freeing that memory when it is destroyed.
    @param left first sub-expression we're adding.
    @param right second sub-expression we're adding.
    @return pointer to a new, dynamically allocated subclass of Expr.
 */
Expr *makeAdd( Expr *left, Expr *right );

/** Make an expression that subtracts its second operand from the
    first. This new object will take ownership of the memory pointed
    to by its two sub-expressions, freeing them when it is destroyed.
    @param left first sub-expression we're subtracting
    @param right second sub-expression we're subtracting
    @return pointer to a new, dynamically allocated subclass of Expr.
 */
Expr *makeSub( Expr *left, Expr *right );

/** Make an expression that multiplies its two operands. This new
    object will take ownership of the memory pointed to by its two
    sub-expressions, freeing them when it is destroyed.
    @param left first sub-expression we're multiplying
    @param right second sub-expression we're multiplying
    @return pointer to a new, dynamically allocated subclass of Expr.
 */
Expr *makeMul( Expr *left, Expr *right );

/** Make an expression that divides its first operand by the
    second. This new object will take ownership of the memory pointed
    to by its two sub-expressions, freeing them when it is destroyed.
    @param left first sub-expression we're dividing
    @param right second sub-expression we're dividing
    @return pointer to a new, dynamically allocated subclass of Expr.
 */
Expr *makeDiv( Expr *left, Expr *right );

/** Make an expression that compares its two operands. This new object
    will take ownership of the memory pointed to by its two
    sub-expressions, freeing them when it is destroyed.
    @param left first sub-expression we're dividing
    @param right second sub-expression we're dividing
    @return pointer to a new, dynamically allocated subclass of Expr.
 */
Expr *makeEquals( Expr *left, Expr *right );

/** Make an expression that compares its two operands as integers.  It
    returns true if the first one is less than the second.  This new
    object will take ownership of the memory pointed to by its two
    sub-expressions, freeing them when it is destroyed.
    @param left first sub-expression we're dividing
    @param right second sub-expression we're dividing
    @return pointer to a new, dynamically allocated subclass of Expr.
 */
Expr *makeLess( Expr *left, Expr *right );

/** Make an expression that evaluates to true if both of its two
    sub-expressions evaluate to true.
    @param left left-hand operand for the and.
    @param right right-hand operand for the and.
    @return pointer to a new, dynamically allocated subclass of Expr.
 */
Expr *makeAnd( Expr *left, Expr *right );

/** Make an expression that evaluates to true if either of its two
    sub-expressions evaluate to true.
    @param left left-hand operand for the or.
    @param right right-hand operand for the or.
    @return pointer to a new, dynamically allocated subclass of Expr.
 */
Expr *makeOr( Expr *left, Expr *right );

/** Make an expression that evaluates to a copy of the value of the
    variable with the given name.  The variable's value will depend on
    the Environment.
    @param name Name of the variable 
    @return pointer to a new, dynamically allocated subclass of Expr.
 */
Expr *makeVariable( char const *name );


/**********************Sequence Initializer operations*/
/**
 * Make an expression that initializes a sequence.
 * Will evaluate every single one of the input expressions and then increase
 * length and capacity as is fit.
 * @param len the intial length of the list
 * @param eList the list of elements to add into the sequence
 * @return a pointer to a new sequence expression.
*/
Expr *makeSequenceInitializer( int len, Expr *elist[] );




/***********************Len operations*/

/**
 * Makes an Expression from a SimpleExpression that evaluates the length a sequence or variable sequence
 * Will Evaluate using the len field for the associated sequence
 * @param expr a pointer to a sequence or variable expression
 * @return a pointer to a new Len expression
*/
Expr *makeLenExpr(Expr *expr);

/**
 * Makes an expression that represents the value of a sequence's index.
 * Will evaluate to an integer value at the index
 * Can check and make an index out of bounds exception
 * @param aexpr the sequence expression
 * @param iexpr the index of the sequence to get
*/
Expr *makeSequenceIndex( Expr *aexpr, Expr *iexpr );

//////////////////////////////////////////////////////////////////////
// Stmt, an interface for a statement in the input program.

/** A short name to use for the statement interface. */
typedef struct StmtStruct Stmt;

/** Representation for the Stmt interface, a superclass for all types
    of statements.  Classes implementing this have these two fields as
    their first members.  They will set execute to point to an
    appropriate functions to execute the type of statement their
    class represents, and they will set destroy to point to a function
    that frees memory for their type of statement.
*/
struct StmtStruct {
  /** Pointer to a function to execute the given staement.
      @param stmt staement to be evaluated.
      @param env current values of all variables.
   */
  void (*execute)( Stmt *stmt, Environment *env );

  /** Free memory for this statement, including any other statements and
      expressions it contains.
      @param stmt statement to free.
  */
  void (*destroy)( Stmt *stmt );
};

/** Make a statement that evaluates the given argument and prints it
    to the terminal.
    @param arg expression to evaluate and print. 
    @return a new print statement.
 */
Stmt *makePrint( Expr *arg );

/** Make a compound statement, representing the sequence of statements
    @param len number of statements in stmtList.
    @param stmtList list of statements making up this compound. The
    compound statement will take ownership of this list of statements
    and will be responsible for freeing all these statements when it
    is destroyed.
    @return a new statement that executes all the statements in
    stmtList, in order.
 */
Stmt *makeCompound( int len, Stmt **stmtList );

/** Make a representation of an if statement.  This new object
    will take ownership of the memory pointed to by cond and body
    and will be responsible for freeing that memory when it is
    destroyed.
    @param cond Expression for the condition on this if statement.
    @param body Statement in the body of this if.
    @return A new statement object that can perform the if statement.
 */
Stmt *makeIf( Expr *cond, Stmt *body );

/** Make a representation of a while statement.  This new object
    will take ownership of the memory pointed to by cond and body
    and will be responsible for freeing that memory when it is
    destroyed.
    @param cond Expression for the condition on this if statement.
    @param body Statement in the body of this while.
    @return A new statement object that can perform the while statement.
 */
Stmt *makeWhile( Expr *cond, Stmt *body );


/**
 * Make a push statements that will add the int value to the end of a sequence.
 * This will grab the sequence briefly then release it.
 * Needs to make sure that the first expression evaluate to a sequence and the second
 * expression evaluate to an int or a typeMismatch occurs.
 * Uses a simpleStatement for the interfave
 * @param sexpr the expression that evaluates to a sequence
 * @param vexpr the expression that evaluates to an integer
*/
Stmt *makePush( Expr *sexpr, Expr *vexpr );


/** Make a representation of an assignment statement.  It is intended to
    work for assigning to a variable (if idx is null), or changing just
    one element in an array (if idx is non-null).
    @param vname Name of the variable we're assigning to.
    @param iexpr If this is an assignment to an array element, this is the
    index for the target element, or null if not.
    @param expr Expression on the right-hand side of the assignemnt.
    @return A new statement object that can perform the assignment.
 */
Stmt *makeAssignment( char const *name, Expr *iexpr, Expr *expr );

//////////////////////////////////////
//Push statement
/**
 * Make a statement that pushes an element to the end of a sequence
 * @param name a variable name that has an element pushed to
 * @return 
*/

#endif
