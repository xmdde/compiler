%{
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "Logger.h"
#include "CodeBlock.h"
#include "MemoryManager.h"

int yylex();
void yyerror(const char *s);
void decode_val(std::string v1, std::string v2);

extern "C" FILE *yyin;
int curr_line = 0;

std::vector<std::string> decoded_values(4, "");
auto bison_logger = logging::Logger("bison_logs.log");
MemoryManager manager;
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
        decode_val($1, "");
        int a = manager.add_assign_block(decoded_values[0], decoded_values[1]);
        $$ = std::to_string(a); // config index
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
        decode_val($2, "");
        int k = manager.add_keyword_block(Keyword::_READ, decoded_values[0], decoded_values[2]);
        $$ = std::to_string(k);
    }
    | WRITE value SEMICOLON {
        decode_val($2, "");
        int k = manager.add_keyword_block(Keyword::_WRITE, decoded_values[0], decoded_values[2]);
        $$ = std::to_string(k);
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
        decode_val($1, "");
        int e = manager.add_expr_to_buffor(ExprOperatorType::_MUL, decoded_values[0], decoded_values[2], "", "");
        $$ = std::to_string(e);
    }
    | value PLUS value {
        decode_val($1, $3);
        int e = manager.add_expr_to_buffor(ExprOperatorType::_PLUS, decoded_values[0], decoded_values[2], decoded_values[1], decoded_values[3]);
        $$ = std::to_string(e);
    }
    | value MIN value {
        decode_val($1, $3);
        int e = manager.add_expr_to_buffor(ExprOperatorType::_MIN, decoded_values[0], decoded_values[2], decoded_values[1], decoded_values[3]);
        $$ = std::to_string(e);
    }
    | value MUL value {
        decode_val($1, $3);
        int e = manager.add_expr_to_buffor(ExprOperatorType::_MUL, decoded_values[0], decoded_values[2], decoded_values[1], decoded_values[3]);
        $$ = std::to_string(e);
    }
    | value DIV value {
        decode_val($1, $3);
        int e = manager.add_expr_to_buffor(ExprOperatorType::_DIV, decoded_values[0], decoded_values[2], decoded_values[1], decoded_values[3]);
        $$ = std::to_string(e);
    }
    | value MOD value {
        decode_val($1, $3);
        int e = manager.add_expr_to_buffor(ExprOperatorType::_MOD, decoded_values[0], decoded_values[2], decoded_values[1], decoded_values[3]);
        $$ = std::to_string(e);
    }
;
condition:
    value EQ value {
        decode_val($1, $3);
        int c = manager.add_cond_block(CondOperatorType::_EQ, decoded_values[0], decoded_values[2], decoded_values[1], decoded_values[3]);
        $$ = std::to_string(c);
    }
    | value NEQ value {
        decode_val($1, $3);
        int c = manager.add_cond_block(CondOperatorType::_NEQ, decoded_values[0], decoded_values[2], decoded_values[1], decoded_values[3]);
        $$ = std::to_string(c);
    }
    | value LMORE value {
        decode_val($1, $3);
        int c = manager.add_cond_block(CondOperatorType::_LMORE, decoded_values[0], decoded_values[2], decoded_values[1], decoded_values[3]);
        $$ = std::to_string(c);
    }
    | value LLESS value {
        decode_val($1, $3);
        int c = manager.add_cond_block(CondOperatorType::_LLESS, decoded_values[0], decoded_values[2], decoded_values[1], decoded_values[3]);
        $$ = std::to_string(c);
    }
    | value LMEQ value {
        decode_val($1, $3);
        int c = manager.add_cond_block(CondOperatorType::_LMEQ, decoded_values[0], decoded_values[2], decoded_values[1], decoded_values[3]);
        $$ = std::to_string(c);
    }
    | value LLEQ value {
        decode_val($1, $3);
        int c = manager.add_cond_block(CondOperatorType::_LLEQ, decoded_values[0], decoded_values[2], decoded_values[1], decoded_values[3]);
        $$ = std::to_string(c);
    }
;
value:
    NUM {
        manager.add_const($1);
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
        manager.add_const($3);
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