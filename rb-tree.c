#include "rb-tree.h"

#define BAD_ARGS -1
#define ERROR -2

RB_tree* tree_create()
{
    RB_tree* tree = (*RB_tree) malloc(sizeof(struct RB_tree));
    if (tree == NULL)
        return NULL;

    tree->num_nodes = 0;

    tree->nil = node_ctor();
    if (tree->nil == NULL)
    {
        free(tree);
        return NULL;
    }
    tree->nil->color = Black;

    tree->root = tree->nil;

    return tree;
}

static RB_node* node_ctor()
{
    RB_node* node = (*RB_node) malloc(sizeof(struct RB_node));
    if (node == NULL)
        return NULL;

    node->parent = NULL;
    node->left_ch = NULL;
    node->right_ch = NULL;

    node->color = Poison_color;
    node->key = Poison_key;

    return node;
}

int tree_dump(FILE* out, RB_tree* tree)
{
    if (out == NULL || tree == NULL)
        return BAD_ARGS;

    fprintf(out, "digraph dump\n{\n");

    fprintf(out, "\tnode [color = \"#000000\", shape = \"box\", fontsize = 20];\n"
                 "\tedge [color = \"#000000\", fontsize = 20];\n\n");

    fprintf(out, "\t\tkey_nil [label = \"nil\", shape = \"diamond\","
                 " color = \"#FFFFFF\", fontcolor = \"#000000\"];\n");

    size_t counter = tree->num_nodes;
    int ret = node_dump(out, tree->root, tree, &counter);

    if (ret < 0)
        return ret;

    frintf(out, "}");

    return 0;
}

static int node_dump(FILE* out, RB_node* node, RB_tree* tree, size_t* counter) // recurcive!!!!!
{
    if (node == NULL)
        return BAD_ARGS;

    if (counter == 0)
        return ERROR;
    (*counter)--;

    fprintf(out, "\t\tkey_%d [label = \"%d\", ", node->key, node->key);
    if (node->color == Red)
        fprintf(out, "color = \"#FF0000\", fontcolor = \"#FFFFFF\"");
    else
        fprintf(out, "color = \"#000000\", fontcolor = \"#FFFFFF\"");
    fprintf(out, "];\n");

    if (node->parent == NULL)
        return ERROR;

    if (node->parent == tree->nil)
        fprintf(out, "\t\tkey_%d -> key_nil;\n", node->key);
    else
        fprintf(out, "\t\tkey_%d -> key_%d;\n", node->key, node->parent->key);

    if (node->left_ch == NULL)
        return ERROR;

    if (node->left == tree->nil)
        fprintf(out, "\t\tkey_%d -> key_nil;\n", node->key);
    else
    {
        fprintf(out, "\t\tkey_%d -> key_%d;\n", node->key, node->left_ch->key);
        int ret = node_dump(out, node->left_ch, tree, count);
        if (ret < 0)
            return ret;
    }

    if (node->right_ch == NULL)
        return ERROR;

    if (node->right_ch == tree->nil)
        fprintf(out, "\t\tkey_%d -> key_nil;\n", node->key);
    else
    {
        fprintf(out, "\t\tkey_%d -> key_%d;\n", node->key, node->right_ch->key);
        int ret = node_dump(out, node->right_ch, tree, counter);
        if (ret < 0)
            return ret;
    }

    return 0;
}
