%{
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "Logger.h"

int yylex();
void yyerror(const char *s);

extern "C" FILE *yyin;
int curr_line = 0;

auto logger = logging::Logger("logs.log");
%}

%define api.value.type {std::string}

%token SEMICOLON
%token EQ
%token ASSIGN
%token COMMA
%token PLUS
%token MIN
%token MUL
%token DIV
%token MOD
%token LBR
%token RBR
%token LSQBR
%token RSQBR
%token NEQ
%token LLESS
%token LMORE
%token LLEQ
%token LMEQ

%token IF
%token THEN
%token ELSE
%token ENDIF
%token WHILE
%token DO
%token ENDWHILE
%token REPEAT
%token UNTIL
%token READ
%token WRITE
%token PROCEDURE
%token IS
%token IN
%token BEGI
%token END
%token PROGRAM
%token T

%token PIDENTIFIER
%token NUM

%%

program_all:
    procedures main {}
;
procedures:
    procedures PROCEDURE proc_head IS declarations IN commands END {

    }
    | procedures PROCEDURE proc_head IS IN commands END {

    }
    | {}
;
main:
    PROGRAM IS declarations IN commands END {

    }
    | PROGRAM IS IN commands END {

    }
;
commands:
    commands command {

    }
    | command {

    }
;
command:
    identifier ASSIGN expression SEMICOLON {

    }
    | IF condition THEN commands ELSE commands ENDIF {

    }
    | IF condition THEN commands ENDIF {

    }
    | WHILE condition DO commands ENDWHILE {

    }
    | REPEAT commands UNTIL condition SEMICOLON {

    }
    | proc_call SEMICOLON {

    }
    | READ identifier SEMICOLON {

    }
    | WRITE value SEMICOLON {

    }
;
proc_head:
    PIDENTIFIER LBR args_decl RBR {

    }
;
proc_call:
    PIDENTIFIER LBR args RBR {

    }
;
declarations:
    declarations COMMA PIDENTIFIER {

    }
    | declarations COMMA PIDENTIFIER LSQBR NUM RSQBR {

    }
    | PIDENTIFIER {

    }
    | PIDENTIFIER LSQBR NUM RSQBR {

    }
;
args_decl:
    args_decl COMMA PIDENTIFIER {

    }
    | args_decl COMMA T PIDENTIFIER {

    }
    | PIDENTIFIER {

    }
    | T PIDENTIFIER {

    }
;
args:
    args COMMA PIDENTIFIER {

    }
    | PIDENTIFIER {

    }
;
expression:
    value {

    }
    | value PLUS value {

    }
    | value MIN value {
        
    }
    | value MUL value {
        
    }
    | value DIV value {
        
    }
    | value MOD value {
        
    }
;
condition:
    value EQ value {

    }
    | value NEQ value {
        
    }
    | value LMORE value {
        
    }
    | value LLESS value {
        
    }
    | value LMEQ value {
        
    }
    | value LLEQ value {

    }
;
value:
    NUM {

    }
    | identifier {
        //logger.log("ident:" + $1);
    }
;
identifier:
    PIDENTIFIER {

    }
    | PIDENTIFIER LSQBR NUM RSQBR {

    }
    | PIDENTIFIER LSQBR PIDENTIFIER RSQBR {

    }
;

%%

void yyerror(const char* msg) {
    std::cerr << "error: " << msg << std::endl;
    exit(1);   
}

int handle(const char* input_file) {
    yyin = fopen(input_file, "r");
    int parsed = yyparse();

    return parsed;
}

int main(int argc, const char** argv) {
    return handle(argv[1]);    
}