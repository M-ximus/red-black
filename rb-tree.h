#ifndef RB_TREE_LIBRARY_H
#define RB_TREE_LIBRARY_H


enum {
    Red,
    Black,
    Poison_color
};

enum Poisons{
    Poison_key
};

struct RB_node{
    struct RB_node* parent;
    struct RB_node* left_ch;
    struct RB_node* right_ch;
    int color;
    int key;
};


// TODO: make cache friendly by reserving memory for nil in the structure

struct RB_tree{
    struct RB_node* nil;
    struct RB_node* root;
    size_t num_nodes;
};

int tree_dump(FILE* out, RB_tree* tree);
RB_tree* tree_create();

#endif // RB_TREE_LIBRARY_H
