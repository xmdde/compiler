%option noyywrap
%{
#include <string>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include "parsery.hpp"

int yylex();
int lex(int token);
int lnum(int token);

int current_line = 1;
%}

%x COMMENT
%%

\t              {}
\;              { yylval = std::string(yytext); return SEMICOLON; }
=               { yylval = std::string(yytext); return EQ; }
:=              { yylval = std::string(yytext); return ASSIGN; }
,               { yylval = std::string(yytext); return COMMA; }
\+              { yylval = std::string(yytext); return PLUS; }
-               { yylval = std::string(yytext); return MIN; }
\*              { yylval = std::string(yytext); return MUL; }
\/              { yylval = std::string(yytext); return DIV; }
"%"             { yylval = std::string(yytext); return MOD; }
\(              { yylval = std::string(yytext); return LBR; }
\)              { yylval = std::string(yytext); return RBR; }
\[              { yylval = std::string(yytext); return LSQBR; }
\]              { yylval = std::string(yytext); return RSQBR; }
!=              { yylval = std::string(yytext); return NEQ; }
"<"             { yylval = std::string(yytext); return LLESS; }
">"             { yylval = std::string(yytext); return LMORE; }
"<="            { yylval = std::string(yytext); return LLEQ; }
">="            { yylval = std::string(yytext); return LMEQ; }
#               { BEGIN(COMMENT); }

<COMMENT>{
    .           {}
    \n          { BEGIN(INITIAL); }
}

[A-Z]+          {
    std::string ID(yytext);
    yylval = ID;
    if (ID == "PROCEDURE") {
        return PROCEDURE;
    } else if (ID == "IS") {
        return IS;
    } else if (ID == "IN") {
        return IN;
    } else if (ID == "BEGIN") {
        return BEGI;
    } else if (ID == "END") {
        return END;
    } else if (ID == "IF") {
        return IF;
    } else if (ID == "THEN") {
        return THEN;
    } else if (ID == "ELSE") {
        return ELSE;
    } else if (ID == "ENDIF") {
        return ENDIF;
    } else if (ID == "WHILE") {
        return WHILE;
    } else if (ID == "DO") {
        return DO;
    } else if (ID == "ENDWHILE") {
        return ENDWHILE;
    } else if (ID == "REPEAT") {
        return REPEAT;
    } else if (ID == "UNTIL") {
        return UNTIL;
    } else if (ID == "READ") {
        return READ;
    } else if (ID == "WRITE") {
        return WRITE;
    } else if (ID == "PROGRAM") {
        return PROGRAM;
    } else if (ID == "T") {
        return T;
    } else ECHO;
}

[[:digit:]]+    {
    yylval = std::string(yytext);
    return NUM;
}

[_a-z]+         {
    yylval = std::string(yytext);
    return PIDENTIFIER;
}

\r|\r\n|\n      {  current_line++; }

[[:space:]]     {}

.               { std::cout << "invalid character in line: " << current_line; exit(1); }
%%