#ifndef RB_TREE_LIBRARY_H
#define RB_TREE_LIBRARY_H

#include "stdio.h"
#include "stdlib.h"
#include "errno.h"

enum {
    Red = 1, // not 0 for simpler search via warning
    Black = 2,
    Poison_color = 3
};

enum Poisons{
    Poison_key
};

enum errors{
    BAD_ARGS = -1,
    ERROR = -2,
    BAD_TREE_CONDITION = -3,
    WRONG_OPERATION = -4,
    E_TOO_MUCH_ELEM = -5,
    EMPTY_TREE = -6
};

typedef struct RB_node{
    struct RB_node* parent;
    struct RB_node* left_ch;
    struct RB_node* right_ch;
    int color;
    int key;
} RB_node;


// TODO: make cache friendly by reserving memory for nil in the structure

typedef struct RB_tree{
    struct RB_node* nil;
    struct RB_node* root;
    size_t num_nodes;
} RB_tree;

int tree_dump(FILE* out, RB_tree* tree);
RB_tree* tree_ctor();
RB_node* node_ctor(int node_key);
int tree_dtor(RB_tree* tree);
int node_dtor(RB_node* node);
int RB_insert(RB_tree* tree, RB_node* new_node);
int RB_delete(RB_tree* tree, RB_node* node);
int foreach(RB_tree* tree, int (*func)(RB_tree*, RB_node*, void*), void*);
RB_node* min_node(RB_tree* tree, RB_node* root);
RB_node* max_node(RB_tree* tree, RB_node* root);
RB_node* RB_search(RB_tree* tree, int key);

#endif // RB_TREE_LIBRARY_H
