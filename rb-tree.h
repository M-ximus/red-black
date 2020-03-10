#ifndef RB_TREE_LIBRARY_H
#define RB_TREE_LIBRARY_H

#include "stdio.h"
#include "stdlib.h"

enum {
    Red,
    Black,
    Poison_color
};

enum Poisons{
    Poison_key
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
int RB_insert(RB_tree* tree, RB_node* new_node);

#endif // RB_TREE_LIBRARY_H
