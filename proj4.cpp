//
// Created by Chase Overcash on 4/11/2019.
//

#include <iostream>
#include "lexer.h"
#include "compiler.h"
#include "ir_debug.h"
#include "inputbuf.h"

using namespace std;

//// Tree Utilities
struct node;
struct index_node;
struct indexDeques;
//// Parse Trees
struct default_case;
struct case_t;
struct case_list;
struct for_stmt;
struct switch_stmt;
struct relop;
struct condition;
struct if_stmt;
struct while_stmt;
struct print_stmt;
struct op;
struct primary;
struct expr;
struct assign_stmt;
struct stmt;
struct stmt_list;
struct body;
struct id_list;
struct var_section;

struct program {
    var_section *VS = NULL;
    body *BODY = NULL;
    LexicalAnalyzer lexer;
};

struct var_section {
    id_list *ID_LIST = NULL;
    Token SC;
    LexicalAnalyzer lexer;
};

struct id_list {
    Token ID;
    Token COMMA;
    id_list *ID_LIST = NULL;
    LexicalAnalyzer lexer;
};

struct body {
    Token LBRACE;
    stmt_list *STMT_LIST = NULL;
    LexicalAnalyzer lexer;
};

struct stmt_list {
    stmt *STMT = NULL;
    stmt_list *STMT_LIST = NULL;
    LexicalAnalyzer lexer;
};

struct stmt {
    assign_stmt *ASGMT = NULL;
    print_stmt *PRINT = NULL;
    while_stmt *WHILE_STMT = NULL;
    if_stmt *IF = NULL;
    switch_stmt *SWITCH_STMT = NULL;
    for_stmt *FOR_STMT = NULL;
    string path = "";
    LexicalAnalyzer lexer;
};

struct assign_stmt {
    Token ID;
    Token EQUAL;
    primary *PRIM = NULL;
    expr *EXPR = NULL;
    Token SEMICOLON;
    LexicalAnalyzer lexer;
};

struct expr {
    primary *PRIM1 = NULL;
    op *OP = NULL;
    primary *PRIM2 = NULL;
    LexicalAnalyzer lexer;
};

struct primary {
    Token PRIM_NODE;
    LexicalAnalyzer lexer;
};

struct op {
    Token OPER;
    LexicalAnalyzer lexer;
};

struct print_stmt {
    Token ID;
    Token SC;
    LexicalAnalyzer lexer;
};

struct while_stmt {
    Token WHILE;
    condition *CON = NULL;
    body *BODY = NULL;
    LexicalAnalyzer lexer;
};

struct if_stmt {
    Token IF;
    condition *CON = NULL;
    body *BODY = NULL;
    LexicalAnalyzer lexer;
};

struct condition {
    primary *PRIM1 = NULL;
    relop *RELOP = NULL;
    primary *PRIM2 = NULL;
    LexicalAnalyzer lexer;
};

struct relop {
    Token REL_OP;
    LexicalAnalyzer lexer;
};

struct switch_stmt {
    Token SWITCH;
    Token ID;
    Token LB;
    case_list *CASE_LIST = NULL;
    default_case *DEF_CASE = NULL;
    Token RB;
    LexicalAnalyzer lexer;
};

struct for_stmt {
    Token FOR;
    Token LP;
    assign_stmt *ASGMT1 = NULL;
    condition *CON = NULL;
    Token SC;
    assign_stmt *ASGMT2 = NULL;
    Token RP;
    body *BODY = NULL;
    LexicalAnalyzer lexer;

};

struct case_list {
    case_t *CASE = NULL;
    case_list *CASE_LIST = NULL;
    LexicalAnalyzer lexer;
};

struct case_t {
    Token CASE_T;
    Token NUM;
    Token COL;
    body *BODY = NULL;
    LexicalAnalyzer lexer;
};

struct default_case {
    Token DEF;
    Token COL;
    body *BODY = NULL;
    LexicalAnalyzer lexer;
};

body *parse_body(LexicalAnalyzer lexer);

condition *parse_condition(LexicalAnalyzer lexer);

primary *parse_primary(LexicalAnalyzer lexer);

assign_stmt *parse_assign_stmt(LexicalAnalyzer lexer);

void syntax_error() {
    cout << "Syntax Error";
    exit(EXIT_FAILURE);
}

default_case *parse_default_case(LexicalAnalyzer lexer) {
    auto *default_case = new(struct default_case);
    Token t1 = lexer.GetToken();
    if (t1.token_type == DEFAULT) {
        t1.Print();
        Token t2 = lexer.GetToken();
        if (t2.token_type == COLON) {
            t2.Print();
            default_case->BODY = parse_body(lexer);
            default_case->lexer = default_case->BODY->lexer;
            return default_case;
        }
        else {
            syntax_error();
        }
    }
    else {
        syntax_error();
    }
}

case_t *parse_case(LexicalAnalyzer lexer) {
    auto *case_t = new (struct case_t);
    Token t1 = lexer.GetToken();
    if (t1.token_type == CASE) {
        t1.Print();
        Token t2 = lexer.GetToken();
        if (t2.token_type == NUM) {
            t2.Print();
            Token t3 = lexer.GetToken();
            if (t3.token_type == COLON) {
                case_t->BODY = parse_body(lexer);
                case_t->lexer = case_t->BODY->lexer;
                return case_t;
            }
            else {
                syntax_error();
            }
        }
        else {
            syntax_error();
        }
    }
    else {
        syntax_error();
    }
}

case_list *parse_case_list(LexicalAnalyzer lexer) {
    auto *case_list = new (struct case_list);
    case_list->CASE = parse_case(lexer);
    lexer = case_list->CASE->lexer;
    Token t1 = lexer.GetToken();
    if (t1.token_type == CASE) {
        lexer.UngetToken(t1);
        case_list->CASE_LIST = parse_case_list(lexer);
        case_list->lexer = case_list->CASE_LIST->lexer;
        return case_list;
    }
    else {
        lexer.UngetToken(t1);
        case_list->lexer = lexer;
        return case_list;
    }
}

relop *parse_relop(LexicalAnalyzer lexer) {
    auto *relop = new (struct relop);
    Token t1 = lexer.GetToken();
    if (t1.token_type == GREATER) {
        t1.Print();
        relop->REL_OP = t1;
        relop->lexer = lexer;
        return relop;
    }
    else if (t1.token_type == LESS) {
        t1.Print();
        relop->REL_OP = t1;
        relop->lexer = lexer;
        return relop;
    }
    else if (t1.token_type == NOTEQUAL) {
        t1.Print();
        relop->REL_OP = t1;
        relop->lexer = lexer;
        return relop;
    }
    else {
        syntax_error();
    }
}

condition *parse_condition(LexicalAnalyzer lexer) {
    auto *condition = new (struct condition);
    condition->PRIM1 = parse_primary(lexer);
    lexer = condition->PRIM1->lexer;
    condition->RELOP = parse_relop(lexer);
    lexer = condition->RELOP->lexer;
    condition->PRIM2 = parse_primary(lexer);
    condition->lexer = condition->PRIM2->lexer;
    return condition;
}

op *parse_operator(LexicalAnalyzer lexer) {
    auto *op = new (struct op);
    Token t1 = lexer.GetToken();
    if (t1.token_type == PLUS) {
        t1.Print();
        op->OPER = t1;
        op->lexer = lexer;
        return op;
    }
    else if (t1.token_type == MINUS) {
        t1.Print();
        op->OPER = t1;
        op->lexer = lexer;
        return op;
    }
    else if (t1.token_type == MULT) {
        t1.Print();
        op->OPER = t1;
        op->lexer = lexer;
        return op;
    }
    else if (t1.token_type == DIV) {
        t1.Print();
        op->OPER = t1;
        op->lexer = lexer;
        return op;
    }
    else syntax_error();
}

primary *parse_primary(LexicalAnalyzer lexer) {
    auto *primary = new (struct primary);
    Token t1 = lexer.GetToken();
    if (t1.token_type == ID) {
        t1.Print();
        primary->PRIM_NODE = t1;
        primary->lexer = lexer;
        return primary;
    }
    else if (t1.token_type == NUM) {
        t1.Print();
        primary->PRIM_NODE = t1;
        primary->lexer = lexer;
        return primary;
    }
    else {
        syntax_error();
    }
}

expr *parse_expr(LexicalAnalyzer lexer) {
    auto *expr = new (struct expr);
    expr->PRIM1 = parse_primary(lexer);
    lexer = expr->PRIM1->lexer;
    expr->OP = parse_operator(lexer);
    lexer = expr->OP->lexer;
    expr->PRIM2 = parse_primary(lexer);
    expr->lexer = expr->PRIM2->lexer;
    return expr;
}

for_stmt *parse_for_stmt(LexicalAnalyzer lexer) {
    auto *for_stmt = new(struct for_stmt);
    Token t1 = lexer.GetToken();
    if (t1.token_type == FOR) {
        t1.Print();
        Token t2 = lexer.GetToken();
        if (t2.token_type == LPAREN) {
            t2.Print();
            for_stmt->ASGMT1 = parse_assign_stmt(lexer);
            lexer = for_stmt->ASGMT1->lexer;
            for_stmt->CON = parse_condition(lexer);
            lexer = for_stmt->CON->lexer;
            Token t3 = lexer.GetToken();
            if (t3.token_type == SEMICOLON) {
                t3.Print();
                for_stmt->ASGMT2 = parse_assign_stmt(lexer);
                lexer = for_stmt->ASGMT2->lexer;
                Token t4 = lexer.GetToken();
                if (t4.token_type == RPAREN) {
                    t4.Print();
                    for_stmt->BODY = parse_body(lexer);
                    for_stmt->lexer = for_stmt->BODY->lexer;
                    return for_stmt;
                }
                else {
                    syntax_error();
                }
            }
            else {
                syntax_error();
            }
        }
        else {
            syntax_error();
        }
    }
    else {
        syntax_error();
    }
}

switch_stmt *parse_switch_stmt(LexicalAnalyzer lexer) {
    auto *switch_stmt = new (struct switch_stmt);
    Token t1 = lexer.GetToken();
    if (t1.token_type == SWITCH) {
        t1.Print();
        Token t2 = lexer.GetToken();
        if (t2.token_type == ID) {
            Token t3 = lexer.GetToken();
            if (t3.token_type == LBRACE) {
                t3.Print();
                switch_stmt->CASE_LIST = parse_case_list(lexer);
                Token t4 = lexer.GetToken();
                if (t4.token_type == RBRACE) {
                    t4.Print();
                    switch_stmt->lexer = lexer;
                    return switch_stmt;
                }
                else {
                    lexer.UngetToken(t4);
                    switch_stmt->DEF_CASE = parse_default_case(lexer);
                    lexer = switch_stmt->DEF_CASE->lexer;
                    Token t5 = lexer.GetToken();
                    if (t5.token_type == SEMICOLON) {
                        t5.Print();
                        switch_stmt->lexer = lexer;
                        return switch_stmt;
                    }
                    else {
                        syntax_error();
                    }
                }
            }
            else {
                syntax_error();
            }
        }
        else {
            syntax_error();
        }
    }
    else {
        syntax_error();
    }
}

if_stmt *parse_if_stmt(LexicalAnalyzer lexer) {
    auto *if_stmt = new (struct if_stmt);
    Token t1 = lexer.GetToken();
    if (t1.token_type == IF) {
        t1.Print();
        if_stmt->CON = parse_condition(lexer);
        lexer = if_stmt->CON->lexer;
        if_stmt->BODY = parse_body(lexer);
        if_stmt->lexer = if_stmt->BODY->lexer;
        return if_stmt;
    }
    else {
        syntax_error();
    }
}

while_stmt *parse_while_stmt(LexicalAnalyzer lexer) {
    auto *while_stmt = new (struct while_stmt);
    Token t1 = lexer.GetToken();
    if (t1.token_type == WHILE) {
        t1.Print();
        while_stmt->CON = parse_condition(lexer);
        lexer = while_stmt->CON->lexer;
        while_stmt->BODY = parse_body(lexer);
        while_stmt->lexer = while_stmt->BODY->lexer;
        return while_stmt;
    }
    else {
        syntax_error();
    }
}

print_stmt *parse_print_stmt(LexicalAnalyzer lexer) {
    auto *print_stmt = new (struct print_stmt);
    Token t1 = lexer.GetToken();
    if (t1.token_type == PRINT) {
        t1.Print();
        Token t2 = lexer.GetToken();
        if (t2.token_type == ID) {
            t2.Print();
            Token t3 = lexer.GetToken();
            if (t3.token_type == SEMICOLON) {
                t3.Print();
                print_stmt->lexer = lexer;
                return print_stmt;
            }
            else {
                syntax_error();
            }
        }
        else {
            syntax_error();
        }
    }
    else {
        syntax_error();
    }
}

assign_stmt *parse_assign_stmt(LexicalAnalyzer lexer) {
    auto *assign_stmt = new (struct assign_stmt);
    Token t1 = lexer.GetToken();
    if (t1.token_type == ID) {
        t1.Print();
        Token t2 = lexer.GetToken();
        if (t2.token_type == EQUAL) {
            t2.Print();
            Token t3 = lexer.GetToken();
            if (t3.token_type == ID || t3.token_type == NUM) {
                Token t4 = lexer.GetToken();
                if (t4.token_type == SEMICOLON) {
                    lexer.UngetToken(t4);
                    lexer.UngetToken(t3);
                    assign_stmt->PRIM = parse_primary(lexer);
                    lexer = assign_stmt->PRIM->lexer;
                    Token t5 = lexer.GetToken();
                    t5.Print();
                    assign_stmt->lexer = lexer;
                    return assign_stmt;
                }
                else {
                    lexer.UngetToken(t4);
                    lexer.UngetToken(t3);
                    assign_stmt->EXPR = parse_expr(lexer);
                    lexer = assign_stmt->EXPR->lexer;
                    Token t5 = lexer.GetToken();
                    if (t5.token_type == SEMICOLON) {
                        t5.Print();
                        assign_stmt->lexer = lexer;
                        return assign_stmt;
                    }
                    else syntax_error();
                }
            }
            else {
                syntax_error();
            }
        }
        else {
            syntax_error();
        }
    }
    else {
        syntax_error();
    }
}

stmt *parse_stmt(LexicalAnalyzer lexer) {
    auto *stmt = new (struct stmt);
    Token t1 = lexer.GetToken();
    if (t1.token_type == ID) {
        lexer.UngetToken(t1);
        stmt->ASGMT = parse_assign_stmt(lexer);
        stmt->lexer = stmt->ASGMT->lexer;
        return stmt;
    }
    else if (t1.token_type == PRINT) {
        lexer.UngetToken(t1);
        stmt->PRINT = parse_print_stmt(lexer);
        stmt->lexer = stmt->PRINT->lexer;
        return stmt;
    }
    else if (t1.token_type == WHILE) {
        lexer.UngetToken(t1);
        stmt->WHILE_STMT = parse_while_stmt(lexer);
        stmt->lexer = stmt->WHILE_STMT->lexer;
        return stmt;
    }
    else if (t1.token_type == IF) {
        lexer.UngetToken(t1);
        stmt->IF = parse_if_stmt(lexer);
        stmt->lexer = stmt->IF->lexer;
        return stmt;
    }
    else if (t1.token_type == SWITCH) {
        lexer.UngetToken(t1);
        stmt->SWITCH_STMT = parse_switch_stmt(lexer);
        stmt->lexer = stmt->SWITCH_STMT->lexer;
        return stmt;
    }
    else if (t1.token_type == FOR) {
        lexer.UngetToken(t1);
        stmt->FOR_STMT = parse_for_stmt(lexer);
        stmt->lexer = stmt->FOR_STMT->lexer;
        return stmt;
    }
    else {
        syntax_error();
    }
}

stmt_list *parse_stmt_list(LexicalAnalyzer lexer) {
    auto *stmt_list = new (struct stmt_list);
    stmt_list->STMT = parse_stmt(lexer);
    lexer = stmt_list->STMT->lexer;
    Token t1 = lexer.GetToken();
    if (t1.token_type == RBRACE) {
        lexer.UngetToken(t1);
        stmt_list->lexer = lexer;
        return stmt_list;
    }
    else if (t1.token_type != END_OF_FILE) {
        lexer.UngetToken(t1);
        stmt_list->STMT_LIST = parse_stmt_list(lexer);
        stmt_list->lexer = stmt_list->STMT_LIST->lexer;
        return stmt_list;
    }
    else {
        syntax_error();
    }
}

body *parse_body(LexicalAnalyzer lexer) {
    auto *body = new (struct body);
    Token t1 = lexer.GetToken();
    if (t1.token_type == LBRACE) {
        t1.Print();
        body->STMT_LIST = parse_stmt_list(lexer);
        lexer = body->STMT_LIST->lexer;
        Token t2 = lexer.GetToken();
        if (t2.token_type == RBRACE) {
            t2.Print();
            body->lexer = lexer;
            return body;
        }
        else {
            syntax_error();
        }
    }
    else {
        syntax_error();
    }
}

id_list *parse_id_list(LexicalAnalyzer lexer) {
    auto *id_list = new (struct id_list);
    Token t1 = lexer.GetToken();
    if (t1.token_type == ID) {
        t1.Print();
        id_list->ID = t1;
        Token t2 = lexer.GetToken();
        if (t2.token_type == COMMA) {
            t2.Print();
            Token t3 = lexer.GetToken();
            if (t3.token_type == ID) {
                lexer.UngetToken(t3);
                id_list->ID_LIST = parse_id_list(lexer);
                id_list->lexer = id_list->ID_LIST->lexer;
                return id_list;
            }
        }
        else if (t2.token_type != END_OF_FILE) {
            lexer.UngetToken(t2);
            id_list->lexer = lexer;
            return id_list;
        }
        else {
            syntax_error();
        }
    }
    else {
        syntax_error();
    }
}

var_section *parse_var_section(LexicalAnalyzer lexer) {
    auto *var_section = new(struct var_section);
    var_section->ID_LIST = parse_id_list(lexer);
    lexer = var_section->ID_LIST->lexer;
    Token t1 = lexer.GetToken();
    if (t1.token_type == SEMICOLON) {
        t1.Print();
        var_section->lexer = lexer;
        return var_section;
    }
    else {
        syntax_error();
    }
}

program *parse_program() {
    auto *program = new(struct program);
    LexicalAnalyzer lexer;
    program->VS = parse_var_section(lexer);
    lexer = program->VS->lexer;
    program->BODY = parse_body(lexer);
    program->lexer = program->BODY->lexer;
    return program;
}
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include "compiler.h"

struct StatementNode * parse_generate_intermediate_representation()
{
    parse_program();
    auto* statementNode = new StatementNode;
    return statementNode;
}
