#ifndef RB_TREE_LIBRARY_H
#define RB_TREE_LIBRARY_H


enum {
    Red,
    Black
};

struct RB_node{
    struct RB_node* parent;
    struct RB_node* left_ch;
    struct RB_node* right_ch;
    int color;
    int key;
    size_t num_nodes;
};


// TODO: make cache friendly by reserving memory for nil in the structure

struct RB_tree{
    struct RB_node* nil;
    struct RB_node* root;
};



#endif // RB_TREE_LIBRARY_H
