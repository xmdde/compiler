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
std::vector<std::string> args_buff;
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
    procedures main {
        manager.log_procedures_info();
    }
;
procedures:
    procedures PROCEDURE proc_head IS declarations IN commands END {
        manager.set_procedure_name($5, $3);
        manager.set_procedure_head($5, $7);
    }
    | procedures PROCEDURE proc_head IS IN commands END {
        std::string p = std::to_string(manager.create_procedure());
        manager.set_procedure_name(p, $3);
        manager.set_procedure_head(p, $6);
    }
    | {}
;
main:
    PROGRAM IS declarations IN commands END {
        manager.set_procedure_name($3, "main");
        manager.set_procedure_head($3, $5);
    }
    | PROGRAM IS IN commands END { // tworz procedure main
        std::string p = std::to_string(manager.create_procedure());
        manager.set_procedure_name(p, "main");
        manager.set_procedure_head(p, $4);
    }
;
commands:
    commands command {
        int c = manager.connect_blocks($1, $2);
        $$ = std::to_string(c);
    }
    | command {
        $$ = $1;
    }
;
command:
    identifier ASSIGN expression SEMICOLON {
        decode_val($1, "");
        int a = manager.add_assign_block(decoded_values[0], decoded_values[1]);
        $$ = std::to_string(a);  // config index
    }
    | IF condition THEN commands ELSE commands ENDIF {
        int i = manager.connect_if_else($2, $4, $6);
        $$ = std::to_string(i);
    }
    | IF condition THEN commands ENDIF {
        int i = manager.connect_if($2, $4);
        $$ = std::to_string(i);
    }
    | WHILE condition DO commands ENDWHILE {
        int w = manager.connect_while($2, $4);
        $$ = std::to_string(w);
    }
    | REPEAT commands UNTIL condition SEMICOLON {
        int r = manager.connect_repeat_until($2, $4);
        $$ = std::to_string(r);
    }
    | proc_call SEMICOLON {
        $$ = $1;
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
proc_head: //nazwe w buff
    PIDENTIFIER LBR args_decl RBR {
        $$ = $1;
    }
;
proc_call:
    PIDENTIFIER LBR args RBR {
        int p = manager.add_proc_call($1, args_buff);
        $$ = std::to_string(p);
    }
;
declarations:
    declarations COMMA PIDENTIFIER {
        manager.add_val_to_procedure($1, ValType::_ID, $3, "");
        $$ = $1;
    }
    | declarations COMMA PIDENTIFIER LSQBR NUM RSQBR {
        manager.add_val_to_procedure($1, ValType::_ARR, $3, $5);
        $$ = $1;
    }
    | PIDENTIFIER {
        int p = manager.create_procedure();
        manager.add_val_to_procedure(std::to_string(p), ValType::_ID, $1, "");
        $$ = std::to_string(p);
    }
    | PIDENTIFIER LSQBR NUM RSQBR {
        int p = manager.create_procedure();
        manager.add_val_to_procedure(std::to_string(p), ValType::_ARR, $1, $3);
        $$ = std::to_string(p);
    }
;
args_decl:
    args_decl COMMA PIDENTIFIER {
        manager.add_val_to_buffor(ValType::_ID, $3);
    }
    | args_decl COMMA T PIDENTIFIER {
        manager.add_val_to_buffor(ValType::_ARR, $4);
    }
    | PIDENTIFIER {
        //manager.clear_args_decl_buffor();
        manager.add_val_to_buffor(ValType::_ID, $1);
    }
    | T PIDENTIFIER {
        //manager.clear_args_decl_buffor();
        manager.add_val_to_buffor(ValType::_ARR, $2);
    }
;
args:
    args COMMA PIDENTIFIER {
        args_buff.push_back($3);
    }
    | PIDENTIFIER {
        args_buff.clear();
        args_buff.push_back($1);
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
    manager.export_ast();
    return parsed;
}

int main(int argc, const char** argv) {
    return handle(argv[1]);
}
