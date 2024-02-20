/* File generated by the BNF Converter (bnfc 2.9.5). */

/*** Visitor Traversal Skeleton. ***/
/* This traverses the abstract syntax tree.
   To use, copy Skeleton.h and Skeleton.c to
   new files. */

#include <stdlib.h>
#include <stdio.h>

#include "Skeleton.h"

void visitExpression(Expression p)
{
  switch(p->kind)
  {
  case is_ExpressionList:
    /* Code for ExpressionList Goes Here */
    visitList(p->u.expressionlist_.list_);
    break;

  default:
    fprintf(stderr, "Error: bad kind field when printing Expression!\n");
    exit(1);
  }
}

void visitList(List p)
{
  switch(p->kind)
  {
  case is_List1:
    /* Code for List1 Goes Here */
    visitBang(p->u.list1_.bang_);
    visitList(p->u.list1_.list_);
    break;
  case is_List2:
    /* Code for List2 Goes Here */
    visitBang(p->u.list2_.bang_);
    visitList(p->u.list2_.list_);
    break;
  case is_List3:
    /* Code for List3 Goes Here */
    visitBang(p->u.list3_.bang_);
    visitList(p->u.list3_.list_);
    break;

  default:
    fprintf(stderr, "Error: bad kind field when printing List!\n");
    exit(1);
  }
}

void visitBang(Bang p)
{
  switch(p->kind)
  {
  case is_Bang1:
    /* Code for Bang1 Goes Here */
    visitSubshell(p->u.bang1_.subshell_);
    break;
  case is_BangSubshell:
    /* Code for BangSubshell Goes Here */
    visitSubshell(p->u.bangsubshell_.subshell_);
    break;

  default:
    fprintf(stderr, "Error: bad kind field when printing Bang!\n");
    exit(1);
  }
}

void visitSubshell(Subshell p)
{
  switch(p->kind)
  {
  case is_Subshell1:
    /* Code for Subshell1 Goes Here */
    visitList(p->u.subshell1_.list_);
    break;
  case is_Subshell2:
    /* Code for Subshell2 Goes Here */
    visitIdent(p->u.subshell2_.ident_);
    visitArgs(p->u.subshell2_.args_);
    break;

  default:
    fprintf(stderr, "Error: bad kind field when printing Subshell!\n");
    exit(1);
  }
}

void visitArgs(Args p)
{
  switch(p->kind)
  {
  case is_Args1:
    /* Code for Args1 Goes Here */
    visitExpression(p->u.args1_.expression_);
    break;
  case is_Args2:
    /* Code for Args2 Goes Here */
    visitListIdent(p->u.args2_.listident_);
    break;

  default:
    fprintf(stderr, "Error: bad kind field when printing Args!\n");
    exit(1);
  }
}

void visitListIdent(ListIdent listident)
{
  while(listident  != 0)
  {
    /* Code For ListIdent Goes Here */
    visitIdent(listident->ident_);
    listident = listident->listident_;
  }
}

void visitIdent(Ident i)
{
  /* Code for Ident Goes Here */
}
void visitInteger(Integer i)
{
  /* Code for Integer Goes Here */
}
void visitDouble(Double d)
{
  /* Code for Double Goes Here */
}
void visitChar(Char c)
{
  /* Code for Char Goes Here */
}
void visitString(String s)
{
  /* Code for String Goes Here */
}

