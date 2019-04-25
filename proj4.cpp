//
// Created by Chase Overcash on 4/11/2019.
//

#include <iostream>
#include "lexer.h"
#include "compiler.h"
#include "ir_debug.h"
#include "inputbuf.h"
#include "deque"

using namespace std;

//////////////////////////////////////////////////////
//// Tree Utilities
//////////////////////////////////////////////////////
struct node {
    string name = "";
    int value = 0;
    Token token;
};
deque<node *> var_vals;

//////////////////////////////////////////////////////
//// Parse Trees
//////////////////////////////////////////////////////
//////////////// Declarations
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

//////////////////////////////////////////////////////
/////////// STRUCTS
//////////////////////////////////////////////////////
struct program {
    var_section *VS = NULL;
    body *BODY = NULL;
    LexicalAnalyzer lexer;
};

struct var_section {
    id_list *ID_LIST = NULL;
    LexicalAnalyzer lexer;
};

struct id_list {
    node *ID = NULL;
    id_list *ID_LIST = NULL;
    LexicalAnalyzer lexer;
};

struct body {
    stmt_list *STMT_LIST = NULL;
    LexicalAnalyzer lexer;
};

struct stmt_list {
    int go = 0;
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
    node *ID = NULL;
    primary *PRIM = NULL;
    expr *EXPR = NULL;
    LexicalAnalyzer lexer;
};

struct expr {
    primary *PRIM1 = NULL;
    op *OP = NULL;
    primary *PRIM2 = NULL;
    int newVal = 0;
    LexicalAnalyzer lexer;
};

struct primary {
    int value;
    string name;
    Token token;
    TokenType prim_type;
    LexicalAnalyzer lexer;
};

struct op {
    Token token;
    string name;
    ArithmeticOperatorType operatorType;
    LexicalAnalyzer lexer;
};

struct print_stmt {
    node *ID = NULL;
    LexicalAnalyzer lexer;
};

struct while_stmt {
    node *WHILE = NULL;
    condition *CON = NULL;
    body *BODY = NULL;
    LexicalAnalyzer lexer;
};

struct if_stmt {
    node *IF = NULL;
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
    Token token;
    ConditionalOperatorType REL_OP;
    LexicalAnalyzer lexer;
};

struct switch_stmt {
    node *SWITCH = NULL;
    node *ID = NULL;
    case_list *CASE_LIST = NULL;
    default_case *DEF_CASE = NULL;
    LexicalAnalyzer lexer;
};

struct for_stmt {
    node *FOR = NULL;
    assign_stmt *ASGMT1 = NULL;
    condition *CON = NULL;
    assign_stmt *ASGMT2 = NULL;
    body *BODY = NULL;
    LexicalAnalyzer lexer;

};

struct case_list {
    case_t *CASE = NULL;
    case_list *CASE_LIST = NULL;
    LexicalAnalyzer lexer;
};

struct case_t {
    node *CASE_T = NULL;
    node *NUM = NULL;
    body *BODY = NULL;
    LexicalAnalyzer lexer;
};

struct default_case {
    node *DEF = NULL;
    body *BODY = NULL;
    LexicalAnalyzer lexer;
};


//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
///////// Function Declarations  
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
body *parse_body(LexicalAnalyzer lexer);

condition *parse_condition(LexicalAnalyzer lexer);

primary *parse_primary(LexicalAnalyzer lexer);

assign_stmt *parse_assign_stmt(LexicalAnalyzer lexer);

//////////////////////////////////////////////////////
//////////////// HELPFUL STUFF ///////////////////////
//////////////////////////////////////////////////////
void syntax_error() {
    cout << "Syntax Error";
    exit(EXIT_FAILURE);
}

int var_vals_indexOf(Token token) {
    int index = 0;
    for (node *n : var_vals) {
        if (n->name == token.lexeme)
            return index;
        index++;
    }
    return -1;
}

int var_value(Token token) {
    for (node *n : var_vals) {
        if (n->name == token.lexeme)
            return n->value;
    }
    return 0;
}

//////////////////////////////////////////////////////
//////////////// PARSE FUNCTIONS /////////////////////
//////////////////////////////////////////////////////
default_case *parse_default_case(LexicalAnalyzer lexer) {
    /////////////// Initialization ///////////////
    auto *default_case = new(struct default_case);
    default_case->DEF = new node;
    //////////////////////////////////////////////

    Token t1 = lexer.GetToken();
    if (t1.token_type == DEFAULT) {
        //t1.Print();

        default_case->DEF->token = t1;
        default_case->DEF->name = t1.lexeme;

        Token t2 = lexer.GetToken();
        if (t2.token_type == COLON) {
            //t2.Print();
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
    ///////////// Initialization ///////////
    auto *case_t = new (struct case_t);
    case_t->CASE_T = new node;
    case_t->NUM = new node;
    ////////////////////////////////////////
    Token t1 = lexer.GetToken();
    if (t1.token_type == CASE) {
        //t1.Print();

        case_t->CASE_T->token = t1;
        case_t->CASE_T->name = t1.lexeme;

        Token t2 = lexer.GetToken();
        if (t2.token_type == NUM) {
            //t2.Print();

            case_t->NUM->token = t2;
            case_t->NUM->value = stoi(t2.lexeme);
            case_t->NUM->name = "";

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
    ////////////// Initialization ///////////
    auto *case_list = new (struct case_list);
    /////////////////////////////////////////
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
    /////////// Initialization ///////
    auto *relop = new (struct relop);
    /////////////////////////////////
    Token t1 = lexer.GetToken();
    if (t1.token_type == GREATER) {
        //t1.Print();

        relop->token = t1;
        relop->lexer = lexer;
        relop->REL_OP = CONDITION_GREATER;

        return relop;
    }
    else if (t1.token_type == LESS) {
        //t1.Print();
        relop->REL_OP = CONDITION_LESS;
        relop->token = t1;
        relop->lexer = lexer;

        return relop;
    }
    else if (t1.token_type == NOTEQUAL) {
        //t1.Print();
        relop->REL_OP = CONDITION_NOTEQUAL;
        relop->token = t1;
        relop->lexer = lexer;

        return relop;
    }
    else {
        syntax_error();
    }
}

condition *parse_condition(LexicalAnalyzer lexer) {
    ///////////// Initialization ////////////
    auto *condition = new (struct condition);
    ////////////////////////////////////////
    condition->PRIM1 = parse_primary(lexer);
    lexer = condition->PRIM1->lexer;
    condition->RELOP = parse_relop(lexer);
    lexer = condition->RELOP->lexer;
    condition->PRIM2 = parse_primary(lexer);
    condition->lexer = condition->PRIM2->lexer;
    return condition;
}

op *parse_operator(LexicalAnalyzer lexer) {
    /////////// Initialization /////
    auto *op = new (struct op);
    ///////////////////////////////

    Token t1 = lexer.GetToken();
    if (t1.token_type == PLUS) {
        //t1.Print();

        op->token = t1;
        op->name = t1.lexeme;
        op->operatorType = OPERATOR_PLUS;
        op->lexer = lexer;

        return op;
    }
    else if (t1.token_type == MINUS) {
        //t1.Print();

        op->token = t1;
        op->name = t1.lexeme;
        op->operatorType = OPERATOR_MINUS;
        op->lexer = lexer;

        return op;
    }
    else if (t1.token_type == MULT) {
        //t1.Print();

        op->token = t1;
        op->name = t1.lexeme;
        op->operatorType = OPERATOR_MULT;
        op->lexer = lexer;

        return op;
    }
    else if (t1.token_type == DIV) {
        //t1.Print();

        op->token = t1;
        op->name = t1.lexeme;
        op->operatorType = OPERATOR_DIV;
        op->lexer = lexer;

        return op;
    }
    else syntax_error();
}

primary *parse_primary(LexicalAnalyzer lexer) {
    /////////// Initialization /////////////
    auto *primary = new (struct primary);
    ///////////////////////////////////////
    Token t1 = lexer.GetToken();
    if (t1.token_type == ID) {
        //t1.Print();
        primary->token = t1;
        primary->name = t1.lexeme;
        primary->value = var_value(t1);
        primary->prim_type = ID;
        primary->lexer = lexer;
        return primary;
    }
    else if (t1.token_type == NUM) {
        //t1.Print();
        primary->token = t1;
        primary->name = "";
        primary->value = stoi(t1.lexeme);
        primary->prim_type = NUM;
        primary->lexer = lexer;
        return primary;
    }
    else {
        syntax_error();
    }
}

expr *parse_expr(LexicalAnalyzer lexer) {
    /////////initialization//////
    auto *expr = new (struct expr);
    ////////////////////////////////
    expr->PRIM1 = parse_primary(lexer);
    lexer = expr->PRIM1->lexer;
    expr->OP = parse_operator(lexer);
    lexer = expr->OP->lexer;
    expr->PRIM2 = parse_primary(lexer);
    expr->lexer = expr->PRIM2->lexer;
    if (expr->OP->operatorType == OPERATOR_MINUS)
    {
        expr->newVal = expr->PRIM1->value - expr->PRIM2->value;
    }
    else if (expr->OP->operatorType == OPERATOR_DIV)
    {
        expr->newVal = expr->PRIM1->value / expr->PRIM2->value;
    }
    else if (expr->OP->operatorType == OPERATOR_MULT)
    {
        expr->newVal = expr->PRIM1->value * expr->PRIM2->value;
    }
    else if (expr->OP->operatorType == OPERATOR_PLUS)
    {
        expr->newVal = expr->PRIM1->value + expr->PRIM2->value;
    }
    return expr;
}

for_stmt *parse_for_stmt(LexicalAnalyzer lexer) {
    ////////////// Initialization /////////
    auto *for_stmt = new(struct for_stmt);
    for_stmt->FOR = new node;
    ///////////////////////////////////////
    Token t1 = lexer.GetToken();
    if (t1.token_type == FOR) {
        //t1.Print();

        for_stmt->FOR->token = t1;
        for_stmt->FOR->name = t1.lexeme;

        Token t2 = lexer.GetToken();
        if (t2.token_type == LPAREN) {
            //t2.Print();
            for_stmt->ASGMT1 = parse_assign_stmt(lexer);
            lexer = for_stmt->ASGMT1->lexer;
            for_stmt->CON = parse_condition(lexer);
            lexer = for_stmt->CON->lexer;
            Token t3 = lexer.GetToken();
            if (t3.token_type == SEMICOLON) {
                //t3.Print();
                for_stmt->ASGMT2 = parse_assign_stmt(lexer);
                lexer = for_stmt->ASGMT2->lexer;
                Token t4 = lexer.GetToken();
                if (t4.token_type == RPAREN) {
                    //t4.Print();
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
    ////////////// Initialization ////////////////
    auto *switch_stmt = new (struct switch_stmt);
    switch_stmt->SWITCH = new node;
    switch_stmt->ID = new node;
    /////////////////////////////////////////////

    Token t1 = lexer.GetToken();
    if (t1.token_type == SWITCH) {
        //t1.Print();

        switch_stmt->SWITCH->token = t1;
        switch_stmt->SWITCH->name = t1.lexeme;

        Token t2 = lexer.GetToken();
        if (t2.token_type == ID) {
            // //t2.Print();

            switch_stmt->ID->name = t2.lexeme;
            switch_stmt->ID->token = t2;

            Token t3 = lexer.GetToken();
            if (t3.token_type == LBRACE) {
                //t3.Print();
                switch_stmt->CASE_LIST = parse_case_list(lexer);
                Token t4 = lexer.GetToken();
                if (t4.token_type == RBRACE) {
                    //t4.Print();
                    switch_stmt->lexer = lexer;
                    return switch_stmt;
                }
                else {
                    lexer.UngetToken(t4);
                    switch_stmt->DEF_CASE = parse_default_case(lexer);
                    lexer = switch_stmt->DEF_CASE->lexer;
                    Token t5 = lexer.GetToken();
                    if (t5.token_type == SEMICOLON) {
                        //t5.Print();
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
    /////////////////////////////////////
    auto *if_stmt = new (struct if_stmt);
    if_stmt->IF = new node;
    /////////////////////////////////////
    Token t1 = lexer.GetToken();
    if (t1.token_type == IF) {
        //t1.Print();

        if_stmt->IF->name = t1.lexeme;
        if_stmt->IF->token = t1;

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
    ////////////////////////////////////////////
    auto *while_stmt = new (struct while_stmt);
    while_stmt->WHILE = new node;
    ////////////////////////////////////////////
    Token t1 = lexer.GetToken();
    if (t1.token_type == WHILE) {
        //t1.Print();

        while_stmt->WHILE->name = t1.lexeme;
        while_stmt->WHILE->token = t1;

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
    ///////////////// Initialization///////////
    auto *print_stmt = new (struct print_stmt);
    print_stmt->ID = new node;
    ///////////////////////////////////////////
    Token t1 = lexer.GetToken();
    if (t1.token_type == PRINT) {
        //t1.Print();

        Token t2 = lexer.GetToken();
        if (t2.token_type == ID) {
            //t2.Print();
            print_stmt->ID->name = t2.lexeme;
            print_stmt->ID->value = var_value(t2);
            print_stmt->ID->token = t2;

            Token t3 = lexer.GetToken();
            if (t3.token_type == SEMICOLON) {
                //t3.Print();
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
    ///////////// Initialize/////////////////////
    auto *assign_stmt = new (struct assign_stmt);
    assign_stmt->ID = new (node);
    ////////////////////////////////////////////
    Token t1 = lexer.GetToken();
    if (t1.token_type == ID) {
        //t1.Print();

        assign_stmt->ID->name = t1.lexeme;
        assign_stmt->ID->token = t1;
        if (var_vals_indexOf(t1) == -1) {
            var_vals.push_back(assign_stmt->ID);
        }
        Token t2 = lexer.GetToken();
        if (t2.token_type == EQUAL) {
            //t2.Print();
            Token t3 = lexer.GetToken();
            if (t3.token_type == ID || t3.token_type == NUM) {
                Token t4 = lexer.GetToken();

                //// PRIM only
                if (t4.token_type == SEMICOLON) {
                    lexer.UngetToken(t4);
                    lexer.UngetToken(t3);
                    assign_stmt->PRIM = parse_primary(lexer);
                    int i = 0;
                    assign_stmt->ID->value = assign_stmt->PRIM->value;
                    for (node * node: var_vals)
                    {
                        if (node->name == t1.lexeme)
                        {
                            var_vals.at(i)->value = assign_stmt->PRIM->value;
                        }
                         i++;
                    }
                    lexer = assign_stmt->PRIM->lexer;
                    Token t5 = lexer.GetToken();
                    //t5.Print();
                    assign_stmt->lexer = lexer;
                    return assign_stmt;
                }
                    //// EXPR
                else {
                    lexer.UngetToken(t4);
                    lexer.UngetToken(t3);
                    assign_stmt->EXPR = parse_expr(lexer);
                    lexer = assign_stmt->EXPR->lexer;
                    Token t5 = lexer.GetToken();
                    if (t5.token_type == SEMICOLON) {
                        //t5.Print();
                        assign_stmt->ID->value = assign_stmt->EXPR->newVal;
                        int i = 0;
                        for (node *node : var_vals)
                        {
                            if (node->name == t1.lexeme)
                            {
                                //cout << "var val: " << var_vals.at(i)->name << " " << to_string(var_vals.at(i)->value);
                                var_vals.at(i)->value = assign_stmt->EXPR->newVal;
                            }
                            i++;
                        }
                        assign_stmt->lexer = lexer;
                        return assign_stmt;
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

stmt *parse_stmt(LexicalAnalyzer lexer) {
    ////////////// Initialize //////////
    auto *stmt = new (struct stmt);
    ////////////////////////////////////

    Token t1 = lexer.GetToken();
    if (t1.token_type == ID) {
        lexer.UngetToken(t1);
        stmt->path = "ASGMT";
        stmt->ASGMT = parse_assign_stmt(lexer);
        stmt->lexer = stmt->ASGMT->lexer;
        return stmt;
    }
    else if (t1.token_type == PRINT) {
        lexer.UngetToken(t1);
        stmt->path = "PRINT";
        stmt->PRINT = parse_print_stmt(lexer);
        lexer = stmt->PRINT->lexer;
        Token token = lexer.GetToken();
//        token.Print();
        lexer.UngetToken(token);
        stmt->lexer = lexer;
        return stmt;
    }
    else if (t1.token_type == WHILE) {
        lexer.UngetToken(t1);
        stmt->path = "WHILE";
        stmt->WHILE_STMT = parse_while_stmt(lexer);
        stmt->lexer = stmt->WHILE_STMT->lexer;
        return stmt;
    }
    else if (t1.token_type == IF) {
        lexer.UngetToken(t1);
        stmt->path = "IF";
        stmt->IF = parse_if_stmt(lexer);
        stmt->lexer = stmt->IF->lexer;
        return stmt;
    }
    else if (t1.token_type == SWITCH) {
        lexer.UngetToken(t1);
        stmt->path = "SWITCH";
        stmt->SWITCH_STMT = parse_switch_stmt(lexer);
        stmt->lexer = stmt->SWITCH_STMT->lexer;
        return stmt;
    }
    else if (t1.token_type == FOR) {
        lexer.UngetToken(t1);
        stmt->path = "FOR";
        stmt->FOR_STMT = parse_for_stmt(lexer);
        stmt->lexer = stmt->FOR_STMT->lexer;
        return stmt;
    }
    else {
        syntax_error();
    }
}

stmt_list *parse_stmt_list(LexicalAnalyzer lexer) {
    ////////////// Initialize ///////////////
    auto *stmt_list = new (struct stmt_list);
    ////////////////////////////////////////
    stmt_list->STMT = parse_stmt(lexer);
    lexer = stmt_list->STMT->lexer;
    Token t1 = lexer.GetToken();
    if (t1.token_type == RBRACE)
    {
        lexer.UngetToken(t1);
        stmt_list->lexer = lexer;
        return stmt_list;
    }
    else if (t1.token_type != END_OF_FILE) {
        lexer.UngetToken(t1);
        stmt_list->go = 1;
        stmt_list->STMT_LIST = parse_stmt_list(lexer);
        stmt_list->lexer = stmt_list->STMT_LIST->lexer;
        return stmt_list;
    }
    else {
        syntax_error();
    }
}

body *parse_body(LexicalAnalyzer lexer) {
    //////////// Initialization //////
    auto *body = new (struct body);
    //////////////////////////////////
    Token t1 = lexer.GetToken();
    if (t1.token_type == LBRACE) {
        //t1.Print();
        body->STMT_LIST = parse_stmt_list(lexer);
        lexer = body->STMT_LIST->lexer;
        Token t2 = lexer.GetToken();
        if (t2.token_type == RBRACE) {
            //t2.Print();
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
    /////////////// Initialize //////////

    auto *id_list = new (struct id_list);
    id_list->ID = new node;
    /////////////////////////////////////

    Token t1 = lexer.GetToken();
    if (t1.token_type == ID) {
        //t1.Print();

        id_list->ID->name = t1.lexeme;
        id_list->ID->value = 0;
        id_list->ID->token = t1;
        var_vals.push_back(id_list->ID);

        Token t2 = lexer.GetToken();
        ///////////////////////// Var_list /////////////
        if (t2.token_type == COMMA) {
            //t2.Print();
            Token t3 = lexer.GetToken();
            if (t3.token_type == ID) {
                lexer.UngetToken(t3);
                id_list->ID_LIST = parse_id_list(lexer);
                id_list->lexer = id_list->ID_LIST->lexer;
                return id_list;
            }
        } ///////////////////////// Base Case ///////
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
    ////////////////////// Initialization //////
    auto *var_section = new(struct var_section);
    ///////////////////////////////////////////
    var_section->ID_LIST = parse_id_list(lexer);
    lexer = var_section->ID_LIST->lexer;
    Token t1 = lexer.GetToken();
    if (t1.token_type == SEMICOLON) {
        //t1.Print();
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

//////////////////////////////////////////////////////
////////// return to compiler ////////////////////////
//////////////////////////////////////////////////////
struct StatementNode *parse_stmt_node(stmt_list* stmtList) {

//    for (node* n: var_vals)
//    {
//        cout << "var val: " << n->name << "has value of: " << to_string(n->value);
//    }
    auto *stmt_list_temp = stmtList;
    auto *stmt_node = new (struct StatementNode);
    auto *value_node = new (struct ValueNode);
    if (stmt_list_temp->STMT == NULL)
    {
        stmt_node = NULL;
        return stmt_node;
    }
    else if (stmt_list_temp->STMT->path == "ASGMT") {

        auto *temp_assign_stmt = stmt_list_temp->STMT->ASGMT;
        stmt_node->assign_stmt = new (AssignmentStatement);
        stmt_node->type = ASSIGN_STMT;
        value_node->name = temp_assign_stmt->ID->name;
        value_node->value = temp_assign_stmt->ID->value;
        stmt_node->assign_stmt->left_hand_side = value_node;

        ///////////// if assign_stmt only has PRIM //////////////
        if (temp_assign_stmt->PRIM != NULL) {
            if (temp_assign_stmt->PRIM->prim_type == NUM) {
                value_node->name = "";
                value_node->value = temp_assign_stmt->PRIM->value;
                stmt_node->assign_stmt->operand1 = value_node;
                stmt_node->assign_stmt->op = OPERATOR_NONE;
                stmt_node->assign_stmt->operand2 = NULL;
                if (stmt_list_temp->go == 1)
                {
                    stmt_node->next = parse_stmt_node(stmt_list_temp->STMT_LIST);
                }
                return stmt_node;
            }
            else {
                value_node->name = temp_assign_stmt->PRIM->name;
                value_node->value = temp_assign_stmt->PRIM->value;
                stmt_node->assign_stmt->operand1 = value_node;
                stmt_node->assign_stmt->op = OPERATOR_NONE;
                stmt_node->assign_stmt->operand2 = NULL;
                if (stmt_list_temp->go == 1) {
                    stmt_node->next = parse_stmt_node(stmt_list_temp->STMT_LIST);
                }
                return stmt_node;
            }
        }
            //////////// IF Assignstatment has an Expr //////////////
        else {

            //// OP1
            auto *temp_expr = temp_assign_stmt->EXPR;
            if (temp_expr->PRIM1->prim_type == NUM) {
                value_node->name = "";
                value_node->value = temp_expr->PRIM1->value;
                stmt_node->assign_stmt->operand1 = value_node;
            }
            else {

                value_node->name = temp_expr->PRIM1->name;
                value_node->value = temp_expr->PRIM1->value;
                stmt_node->assign_stmt->operand1 = value_node;
            }

            // OP TYPE
            ArithmeticOperatorType op_type = temp_expr->OP->operatorType;
            if (op_type == OPERATOR_MULT) {
                stmt_node->assign_stmt->op = OPERATOR_MULT;
            }
            else if (op_type == OPERATOR_DIV) {
                stmt_node->assign_stmt->op = OPERATOR_DIV;
            }
            else if (op_type == OPERATOR_MINUS) {
                stmt_node->assign_stmt->op = OPERATOR_MINUS;
            }
            else {
                stmt_node->assign_stmt->op = OPERATOR_PLUS;
            }
            //// OP2
            if (temp_expr->PRIM2->prim_type == NUM) {
                value_node->name = "";
                value_node->value = temp_expr->PRIM2->value;
                stmt_node->assign_stmt->operand2 = value_node;
                if (stmt_list_temp->go == 1) {
                    stmt_node->next = parse_stmt_node(stmt_list_temp->STMT_LIST);
                }
                return stmt_node;
            }
            else {
                value_node->name = temp_expr->PRIM2->name;
                value_node->value = temp_expr->PRIM2->value;
                stmt_node->assign_stmt->operand2 = value_node;
                if (stmt_list_temp->go == 1) {
                    stmt_node->next = parse_stmt_node(stmt_list_temp->STMT_LIST);
                }
                return stmt_node;
            }
        }
    }
    else if (stmt_list_temp->STMT->path == "PRINT") {
        value_node->name = stmt_list_temp->STMT->PRINT->ID->name;
        value_node->value = stmt_list_temp->STMT->PRINT->ID->value;
        auto *temp_print_stmt = new PrintStatement;
        temp_print_stmt->id = value_node;
        stmt_node->print_stmt = temp_print_stmt;
        stmt_node->type = PRINT_STMT;
        if (stmt_list_temp->go == 1)
        {
            stmt_node->next = parse_stmt_node(stmt_list_temp->STMT_LIST);
        }
        return stmt_node;
    }
////    else if (stmt_list_temp->STMT->path == "WHILE") {
//////                    auto* value_node = new ValueNode;
//////                    value_node->name = stmt_list_temp->STMT->PRINT->ID->name;
////
////    }
////    else if (stmt_list_temp->STMT->path == "IF") {
////
////    }
////    else if (stmt_list_temp->STMT->path == "FOR") {
////
////    }
////    else if (stmt_list_temp->STMT->path == "SWITCH") {
////
////    }
    return stmt_node;
}

struct StatementNode *parse_generate_intermediate_representation() {
    auto* valN = new ValueNode;
    valN->name = "Push Material";
    valN->value = 999;
    auto *program = parse_program();
    if (program->BODY != NULL) {
        if (program->BODY->STMT_LIST != NULL) {
            auto *stmtList = program->BODY->STMT_LIST;
            auto* StatementNode = parse_stmt_node(stmtList);
            return StatementNode;
        }
    }
}