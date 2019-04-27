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

LexicalAnalyzer lexer;
//////////////////////////////////////////////////////
//// Tree Utilities
/////////////////////////////////////////////////////
deque<ValueNode *> global_LHS;
deque<ValueNode *> global_RHS;
bool while_scope = false;
int tracker = 0;
struct node {
    string name = "";
    int value = 0;
    Token token;
};
deque<node *> var_vals;
bool impactful = true;
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
struct if_branch;
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

};

struct var_section {
    id_list *ID_LIST = NULL;

};

struct id_list {
    node *ID = NULL;
    id_list *ID_LIST = NULL;

};

struct body {
    StatementNode *STMT_LIST = NULL;

};

struct stmt_list {
    int go = 0;
    StatementNode *STMT = NULL;
    StatementNode *next = NULL;

};

struct stmt {
    StatementNode *ASGMT = NULL;
    StatementNode *PRINT = NULL;
    StatementNode *WHILE_STMT = NULL;
    StatementNode *IF = NULL;
    StatementNode *SWITCH_STMT = NULL;
    StatementNode *FOR_STMT = NULL;
    string path = "";

};

struct assign_stmt {
    node *ID = NULL;
    primary *PRIM = NULL;
    expr *EXPR = NULL;

};

struct expr {
    primary *PRIM1 = NULL;
    op *OP = NULL;
    primary *PRIM2 = NULL;
    int newVal = 0;

};

struct primary {
    int value;
    string name;
    Token token;
    TokenType prim_type;

};

struct op {
    Token token;
    string name;
    ArithmeticOperatorType operatorType;

};

struct while_stmt {
    node *WHILE = NULL;
    condition *CON = NULL;
    body *BODY = NULL;

};

struct if_branch {
    node *IF = NULL;
    condition *CON = NULL;
    body *BODY = NULL;

};

struct condition {
    primary *PRIM1 = NULL;
    relop *RELOP = NULL;
    primary *PRIM2 = NULL;
    bool conditional_truth = false;

};

struct relop {
    Token token;
    ConditionalOperatorType REL_OP;

};

struct switch_stmt {
    node *SWITCH = NULL;
    node *ID = NULL;
    case_list *CASE_LIST = NULL;
    default_case *DEF_CASE = NULL;

};

struct for_stmt {
    node *FOR = NULL;
    StatementNode *ASGMT1 = NULL;
    condition *CON = NULL;
    StatementNode *ASGMT2 = NULL;
    body *BODY = NULL;


};

struct case_list {
    case_t *CASE = NULL;
    case_list *CASE_LIST = NULL;

};

struct case_t {
    node *CASE_T = NULL;
    node *NUM = NULL;
    body *BODY = NULL;

};

struct default_case {
    node *DEF = NULL;
    body *BODY = NULL;

};


//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
///////// Function Declarations
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
body *parse_body();

condition *parse_condition();

primary *parse_primary();

StatementNode *parse_assign_stmt();

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
default_case *parse_default_case() {
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
            default_case->BODY = parse_body();
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

case_t *parse_case() {
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
                case_t->BODY = parse_body();
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

case_list *parse_case_list() {
    ////////////// Initialization ///////////
    auto *case_list = new (struct case_list);
    /////////////////////////////////////////
    case_list->CASE = parse_case();
    Token t1 = lexer.GetToken();
    if (t1.token_type == CASE) {
        lexer.UngetToken(t1);
        case_list->CASE_LIST = parse_case_list();
        return case_list;
    }
    else {
        lexer.UngetToken(t1);
        return case_list;
    }
}

relop *parse_relop() {
    /////////// Initialization ///////
    auto *relop = new (struct relop);
    /////////////////////////////////
    Token t1 = lexer.GetToken();
    if (t1.token_type == GREATER) {
        //t1.Print();

        relop->token = t1;
        relop->REL_OP = CONDITION_GREATER;

        return relop;
    }
    else if (t1.token_type == LESS) {
        //t1.Print();
        relop->REL_OP = CONDITION_LESS;
        relop->token = t1;

        return relop;
    }
    else if (t1.token_type == NOTEQUAL) {
        //t1.Print();
        relop->REL_OP = CONDITION_NOTEQUAL;
        relop->token = t1;

        return relop;
    }
    else {
        syntax_error();
    }
}

condition *parse_condition() {
    ///////////// Initialization ////////////
    auto *condition = new (struct condition);
    ////////////////////////////////////////
    condition->PRIM1 = parse_primary();
    condition->RELOP = parse_relop();
    condition->PRIM2 = parse_primary();

    if (condition->RELOP->REL_OP == CONDITION_LESS) {
        if (condition->PRIM1->value < condition->PRIM2->value) {
            condition->conditional_truth = true;
        }
    }
    else if (condition->RELOP->REL_OP == CONDITION_NOTEQUAL) {
        if (condition->PRIM1->value != condition->PRIM2->value) {
            condition->conditional_truth = true;
        }
    }
    else if (condition->RELOP->REL_OP == CONDITION_GREATER) {
        if (condition->PRIM1->value > condition->PRIM2->value) {
            condition->conditional_truth = true;
        }
    }
    return condition;
}

op *parse_operator() {
    /////////// Initialization /////
    auto *op = new (struct op);
    ///////////////////////////////

    Token t1 = lexer.GetToken();
    if (t1.token_type == PLUS) {
        //t1.Print();

        op->token = t1;
        op->name = t1.lexeme;
        op->operatorType = OPERATOR_PLUS;

        return op;
    }
    else if (t1.token_type == MINUS) {
        //t1.Print();

        op->token = t1;
        op->name = t1.lexeme;
        op->operatorType = OPERATOR_MINUS;

        return op;
    }
    else if (t1.token_type == MULT) {
        //t1.Print();

        op->token = t1;
        op->name = t1.lexeme;
        op->operatorType = OPERATOR_MULT;

        return op;
    }
    else if (t1.token_type == DIV) {
        //t1.Print();

        op->token = t1;
        op->name = t1.lexeme;
        op->operatorType = OPERATOR_DIV;

        return op;
    }
    else syntax_error();
}

primary *parse_primary() {
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
        return primary;
    }
    else if (t1.token_type == NUM) {
        //t1.Print();
        primary->token = t1;
        primary->name = "";
        primary->value = stoi(t1.lexeme);
        primary->prim_type = NUM;
        return primary;
    }
    else {
        syntax_error();
    }
}

expr *parse_expr() {
    /////////initialization//////
    auto *expr = new (struct expr);
    ////////////////////////////////
    expr->PRIM1 = parse_primary();
    expr->OP = parse_operator();
    expr->PRIM2 = parse_primary();
    if (expr->OP->operatorType == OPERATOR_MINUS) {
        expr->newVal = expr->PRIM1->value - expr->PRIM2->value;
    }
    else if (expr->OP->operatorType == OPERATOR_DIV) {
        expr->newVal = expr->PRIM1->value / expr->PRIM2->value;
    }
    else if (expr->OP->operatorType == OPERATOR_MULT) {
        expr->newVal = expr->PRIM1->value * expr->PRIM2->value;
    }
    else if (expr->OP->operatorType == OPERATOR_PLUS) {
        expr->newVal = expr->PRIM1->value + expr->PRIM2->value;
    }
    return expr;
}

StatementNode *parse_for_stmt() {
    ////////////// Initialization /////////
    auto *for_stmt = new(struct for_stmt);
    auto *stmt_node = new StatementNode;
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
            for_stmt->ASGMT1 = parse_assign_stmt();
            for_stmt->CON = parse_condition();
            Token t3 = lexer.GetToken();
            if (t3.token_type == SEMICOLON) {
                //t3.Print();
                for_stmt->ASGMT2 = parse_assign_stmt();
                Token t4 = lexer.GetToken();
                if (t4.token_type == RPAREN) {
                    //t4.Print();
                    for_stmt->BODY = parse_body();
                    return stmt_node;
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

StatementNode *parse_switch_stmt() {
    ////////////// Initialization ////////////////
    auto *switch_stmt = new (struct switch_stmt);
    auto *stmt_node = new StatementNode;
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
                switch_stmt->CASE_LIST = parse_case_list();
                Token t4 = lexer.GetToken();
                if (t4.token_type == RBRACE) {
                    //t4.Print();
                    return stmt_node;
                }
                else {
                    lexer.UngetToken(t4);
                    switch_stmt->DEF_CASE = parse_default_case();
                    Token t5 = lexer.GetToken();
                    if (t5.token_type == SEMICOLON) {
                        //t5.Print();
                        return stmt_node;
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

StatementNode *parse_if_branch() {
    /////////////////////////////////////
    auto *if_branch = new (struct if_branch);
    auto *stmt_node = new (StatementNode);
    auto *noop_node = new StatementNode;
    noop_node->type = NOOP_STMT;
    noop_node->next = NULL;
    stmt_node->type = IF_STMT;
    stmt_node->if_stmt = new IfStatement;
    if_branch->IF = new node;
    /////////////////////////////////////
    Token t1 = lexer.GetToken();
    if (t1.token_type == IF) {
        //t1.Print();

        if_branch->IF->name = t1.lexeme;
        if_branch->IF->token = t1;
        if_branch->CON = parse_condition();
        ValueNode *OP1 = new ValueNode;
        ValueNode *OP2 = new ValueNode;
        OP1->value = if_branch->CON->PRIM1->value;
        OP1->name = if_branch->CON->PRIM1->name;
        OP2->value = if_branch->CON->PRIM2->value;
        OP2->name = if_branch->CON->PRIM2->name;
        cout << "if :" << OP1->name << " :" << OP1->value << "  OP  " << OP2->name << " :" << OP2->value << "\n";
        stmt_node->if_stmt->condition_operand1 = OP1;
        stmt_node->if_stmt->condition_op = if_branch->CON->RELOP->REL_OP;
        stmt_node->if_stmt->condition_operand2 = OP2;
        if (!if_branch->CON->conditional_truth) {
            impactful = false;
        }
        if_branch->BODY = parse_body();
        stmt_node->if_stmt->true_branch = if_branch->BODY->STMT_LIST;
        StatementNode *curr = if_branch->BODY->STMT_LIST;
        while (curr->next != NULL) {
            curr = curr->next;
        }
        curr->next = noop_node;
        stmt_node->if_stmt->false_branch = noop_node;
        stmt_node->next = noop_node;

        impactful = true;
        return stmt_node;
    }
    else {
        syntax_error();
    }
}

StatementNode *parse_while_stmt() {
    ////////////////////////////////////////////
    auto *while_stmt = new (struct while_stmt);
    auto *stmt_node = new StatementNode;
    while_stmt->WHILE = new node;
    auto *noop_node = new StatementNode;
    noop_node->type = NOOP_STMT;
    noop_node->next = NULL;
    auto *goto_node = new StatementNode;
    goto_node->type = GOTO_STMT;
    goto_node->goto_stmt = new GotoStatement;
    ////////////////////////////////////////////
    Token t1 = lexer.GetToken();
    if (t1.token_type == WHILE) {
        //t1.Print();

        while_stmt->WHILE->name = t1.lexeme;
        while_stmt->WHILE->token = t1;
        while_stmt->CON = parse_condition();
        /////////////////////////////////////////
//        ValueNode* OP1 = new ValueNode;
//        OP1->value = while_stmt->CON->PRIM1->value;
//        OP1->name = while_stmt->CON->PRIM1->name;
//        ValueNode* OP2 = new ValueNode;
//        OP2->value = while_stmt->CON->PRIM2->value;
//        OP2->name = while_stmt->CON->PRIM2->name;
//        global_LHS.push_back(OP1);
//        global_RHS.push_back(OP2);
        /////////////////////////////////////////////
        while_stmt->BODY = parse_body();
        return stmt_node;
    }
    else {
        syntax_error();
    }
}

StatementNode *parse_print_stmt() {
    ///////////////// Initialization///////////
    auto *stmt_node = new (StatementNode);
    stmt_node->type = PRINT_STMT;
    stmt_node->print_stmt = new PrintStatement;
    stmt_node->next = NULL;
    ///////////////////////////////////////////
    Token t1 = lexer.GetToken();
    if (t1.token_type == PRINT) {
        //t1.Print();

        Token t2 = lexer.GetToken();
        if (t2.token_type == ID) {
            //t2.Print();

            Token t3 = lexer.GetToken();
            if (t3.token_type == SEMICOLON) {
                //t3.Print();
                ValueNode *OP1 = new ValueNode;
                OP1->value = var_value(t2);
                OP1->name = t2.lexeme;
                stmt_node->print_stmt->id = OP1;
                return stmt_node;
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

StatementNode *parse_assign_stmt() {
    ///////////// Initialize/////////////////////
    auto *assign_stmt = new (struct assign_stmt);
    assign_stmt->ID = new (node);
    auto *stmt_node = new StatementNode;
    stmt_node->assign_stmt = new AssignmentStatement;
    stmt_node->type = ASSIGN_STMT;
    stmt_node->next = NULL;
    auto *lhs = new ValueNode;
    ////////////////////////////////////////////
    Token t1 = lexer.GetToken();
    if (t1.token_type == ID) {
        //t1.Print();

        assign_stmt->ID->name = t1.lexeme;
        assign_stmt->ID->token = t1;
        lhs->name = t1.lexeme;

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
                    assign_stmt->PRIM = parse_primary();
                    int i = 0;
                    assign_stmt->ID->value = assign_stmt->PRIM->value;
                    lhs->value = assign_stmt->PRIM->value;
                    for (node *node: var_vals) {
                        if (node->name == t1.lexeme && impactful) {
                            var_vals.at(i)->value = assign_stmt->PRIM->value;
                        }
                        i++;
                    }
                    Token t5 = lexer.GetToken();
                    //t5.Print();

                    stmt_node->assign_stmt->left_hand_side = lhs;
                    ValueNode *OP1 = new ValueNode;
                    OP1->value = assign_stmt->PRIM->value;
                    OP1->name = assign_stmt->PRIM->name;
                    stmt_node->assign_stmt->operand1 = OP1;
                    stmt_node->assign_stmt->op = OPERATOR_NONE;
                    stmt_node->assign_stmt->operand2 = NULL;

                    return stmt_node;
                }
                    //// EXPR
                else {
                    lexer.UngetToken(t4);
                    lexer.UngetToken(t3);
                    assign_stmt->EXPR = parse_expr();

                    ValueNode *OP1 = new ValueNode;
                    ValueNode *OP2 = new ValueNode;
                    OP1->value = assign_stmt->EXPR->PRIM1->value;
                    OP1->name = assign_stmt->EXPR->PRIM1->name;
                    OP2->value = assign_stmt->EXPR->PRIM2->value;
                    OP2->name = assign_stmt->EXPR->PRIM2->name;
                    stmt_node->assign_stmt->operand1 = OP1;
                    stmt_node->assign_stmt->operand2 = OP2;
                    stmt_node->assign_stmt->op = assign_stmt->EXPR->OP->operatorType;


                    Token t5 = lexer.GetToken();
                    if (t5.token_type == SEMICOLON) {
                        //t5.Print();
                        assign_stmt->ID->value = assign_stmt->EXPR->newVal;
                        lhs->value = assign_stmt->EXPR->newVal;
                        int i = 0;
                        for (node *node : var_vals) {
                            if (node->name == t1.lexeme && impactful) {
                                //cout << "var val: " << var_vals.at(i)->name << " " << to_string(var_vals.at(i)->value);
                                var_vals.at(i)->value = assign_stmt->EXPR->newVal;
                            }
                            i++;
                        }

                        stmt_node->assign_stmt->left_hand_side = lhs;

                        return stmt_node;
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

StatementNode *parse_stmt() {
    ////////////// Initialize //////////
    StatementNode *stmt_node;
////////////////////////////////////

    Token t1 = lexer.GetToken();
    if (t1.token_type == ID) {
        lexer.UngetToken(t1);
        stmt_node = parse_assign_stmt();
        return stmt_node;
    }
    else if (t1.token_type == PRINT) {
        lexer.UngetToken(t1);
        stmt_node = parse_print_stmt();
        return stmt_node;
    }
    else if (t1.token_type == WHILE) {
        lexer.UngetToken(t1);
        stmt_node = parse_while_stmt();
        return stmt_node;
    }
    else if (t1.token_type == IF) {
        lexer.UngetToken(t1);
        stmt_node = parse_if_branch();
        return stmt_node;
    }
    else if (t1.token_type == SWITCH) {
        lexer.UngetToken(t1);
        stmt_node = parse_switch_stmt();
        return stmt_node;
    }
    else if (t1.token_type == FOR) {
        lexer.UngetToken(t1);
        stmt_node = parse_for_stmt();
        return stmt_node;
    }
    else {
        syntax_error();
    }
}

StatementNode *parse_stmt_list() {
    ////////////// Initialize ///////////////
    auto *stmt_list = new (struct stmt_list);
    auto *statement_node = new StatementNode;
    ////////////////////////////////////////
    statement_node = parse_stmt();
    Token t1 = lexer.GetToken();
    if (t1.token_type == RBRACE) {
        lexer.UngetToken(t1);
        return statement_node;
    }
    else if (t1.token_type != END_OF_FILE) {
        lexer.UngetToken(t1);
        stmt_list->go = 1;
        statement_node->next = parse_stmt_list();
        return statement_node;
    }
    else {
        syntax_error();
    }
}

body *parse_body() {
    //////////// Initialization //////
    auto *body = new (struct body);
    //////////////////////////////////
    Token t1 = lexer.GetToken();
    if (t1.token_type == LBRACE) {
        //t1.Print();
        body->STMT_LIST = parse_stmt_list();
        Token t2 = lexer.GetToken();
        if (t2.token_type == RBRACE) {
            //t2.Print();
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

id_list *parse_id_list() {
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
                id_list->ID_LIST = parse_id_list();
                return id_list;
            }
        } ///////////////////////// Base Case ///////
        else if (t2.token_type != END_OF_FILE) {
            lexer.UngetToken(t2);
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

var_section *parse_var_section() {
    ////////////////////// Initialization //////
    auto *var_section = new(struct var_section);
    ///////////////////////////////////////////
    var_section->ID_LIST = parse_id_list();
    Token t1 = lexer.GetToken();
    if (t1.token_type == SEMICOLON) {
        //t1.Print();
        return var_section;
    }
    else {
        syntax_error();
    }
}

program *parse_program() {
    auto *program = new(struct program);
    program->VS = parse_var_section();
    program->BODY = parse_body();
    Token t1 = lexer.GetToken();
    if (t1.token_type == END_OF_FILE) {
        //t1.Print();
        return program;
    }
    else {
        syntax_error();
    }
}

struct StatementNode *parse_generate_intermediate_representation() {
    auto *valN = new ValueNode;
    valN->name = "Push Material";
    valN->value = 999;
    auto *program = parse_program();
    return program->BODY->STMT_LIST;
}