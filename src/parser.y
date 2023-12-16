%{
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "Logger.h"
#include "CodeBlock.h"

int yylex();
void yyerror(const char *s);
void decode_val(std::string v1, std::string v2);

extern "C" FILE *yyin;
int curr_line = 0;

std::vector<std::string> decoded_values(4, "");
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
        decode_val($1, $3);
        CondBlock c(0, CondOperatorType::_EQ, decoded_values[0], decoded_values[2], decoded_values[1], decoded_values[3]);
        logger.log(&c);
    }
    | value NEQ value {
        decode_val($1, $3);
        CondBlock c(0, CondOperatorType::_NEQ, decoded_values[0], decoded_values[2], decoded_values[1], decoded_values[3]);
        logger.log(&c);
    }
    | value LMORE value {
        decode_val($1, $3);
        CondBlock c(0, CondOperatorType::_LMORE, decoded_values[0], decoded_values[2], decoded_values[1], decoded_values[3]);
        logger.log(&c);
    }
    | value LLESS value {
        decode_val($1, $3);
        CondBlock c(0, CondOperatorType::_LLESS, decoded_values[0], decoded_values[2], decoded_values[1], decoded_values[3]);
        logger.log(&c);
    }
    | value LMEQ value {
        decode_val($1, $3);
        CondBlock c(0, CondOperatorType::_LMEQ, decoded_values[0], decoded_values[2], decoded_values[1], decoded_values[3]);
        logger.log(&c);
    }
    | value LLEQ value {
        decode_val($1, $3);
        CondBlock c(0, CondOperatorType::_LLEQ, decoded_values[0], decoded_values[2], decoded_values[1], decoded_values[3]);
        logger.log(&c);
    }
;
value:
    NUM {
        // add num to consts
        $$ = $1;
    }
    | identifier {
        $$ = $1;
    }
;
identifier:
    PIDENTIFIER {
        $$ = $1;
    }
    | PIDENTIFIER LSQBR NUM RSQBR {
        // add num to consts
        $$ = $1 + "[" + $3 + "]";
    }
    | PIDENTIFIER LSQBR PIDENTIFIER RSQBR {
        $$ = $1 + "[" + $3 + "]";
    }
;

%%

void yyerror(const char* msg) {
    std::cerr << "error: " << msg << std::endl;
    exit(1);   
}

void decode_val(std::string v1, std::string v2) {
    size_t openingBracketPos = v1.find('[');
    std::string arr_substring1 = (openingBracketPos != std::string::npos) ? v1.substr(0, openingBracketPos) : v1;
    size_t closingBracketPos = v1.find(']');
    std::string index_substring1 = (closingBracketPos != std::string::npos) ? v1.substr(openingBracketPos + 1, closingBracketPos - openingBracketPos - 1) : "";
    std::string arr_substring2 = "";
    std::string index_substring2 = "";
    if (v2 != "") {
        size_t openingBracketPos = v2.find('[');
        arr_substring2 = (openingBracketPos != std::string::npos) ? v2.substr(0, openingBracketPos) : v2;
        size_t closingBracketPos = v2.find(']');
        index_substring2 = (closingBracketPos != std::string::npos) ? v2.substr(openingBracketPos + 1, closingBracketPos - openingBracketPos - 1) : "";
    }
    decoded_values[0] = arr_substring1;
    decoded_values[1] = index_substring1;
    decoded_values[2] = arr_substring2;
    decoded_values[3] = index_substring2;
}

int handle(const char* input_file) {
    yyin = fopen(input_file, "r");
    int parsed = yyparse();

    return parsed;
}

int main(int argc, const char** argv) {
    return handle(argv[1]);    
}