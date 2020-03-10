#include "rb-tree.h"

#define BAD_ARGS -1
#define ERROR -2
#define BAD_TREE_CONDITION -3
#define WRONG_OPERATION -4

static RB_node* node_create();
RB_tree* tree_ctor();
RB_node* node_ctor(int node_key);
int RB_insert(RB_tree* tree, RB_node* new_node);
static int insert_fixup(RB_tree* tree, RB_node* new_node);
static int left_rotate(RB_tree* tree, RB_node* node);
static int right_rotate(RB_tree* tree, RB_node* node);
int tree_dump(FILE* out, RB_tree* tree);
static int node_dump(FILE* out, RB_node* node, RB_tree* tree, size_t* counter);



static RB_node* node_create()
{
    struct RB_node* node = (RB_node*) malloc(sizeof(struct RB_node));
    if (node == NULL)
        return NULL;

    node->parent = NULL;
    node->left_ch = NULL;
    node->right_ch = NULL;

    node->color = Poison_color;
    node->key = Poison_key;

    return node;
}

struct RB_tree* tree_ctor()
{
    RB_tree* tree = (RB_tree*) calloc(1, sizeof(RB_tree));
    if (tree == NULL)
        return NULL;

    tree->num_nodes = 0;

    tree->nil = node_create();
    if (tree->nil == NULL)
    {
        free(tree);
        return NULL;
    }
    tree->nil->color = Black;

    tree->root = tree->nil;

    return tree;
}

RB_node* node_ctor(int node_key)
{
    RB_node* new_node = node_create();
    if (new_node != NULL)
        new_node->key = node_key;

    return new_node;
}

int RB_insert(RB_tree* tree, RB_node* new_node)
{
    if (tree == NULL || new_node == NULL)
        return BAD_ARGS;

    RB_node* Nil = tree->nil;
    int new_key = new_node->key;

    RB_node* cur_node = tree->root;
    RB_node* new_parent = Nil;

    while(cur_node != Nil)
    {
        new_parent = cur_node;

        if (new_key > cur_node->key)
            cur_node = cur_node->right_ch;
        else
            cur_node = cur_node->left_ch;
    }

    if (new_parent == Nil)
        tree->root = new_node;
    else if(new_key > new_parent->key)
        new_parent->right_ch = new_node;
    else
        new_parent->left_ch = new_node;

    new_node->parent = new_parent;
    new_node->color = Red;
    new_node->left_ch = Nil;
    new_node->right_ch = Nil;

    (tree->num_nodes)++;

    int ret_val = insert_fixup(tree, new_node);
    //printf("ret_val = %d\n", ret_val);

    if (ret_val != 0)
        return BAD_TREE_CONDITION;

    return 0;
}

static int insert_fixup(RB_tree* tree, RB_node* new_node)
{
    if (tree == NULL || new_node == NULL)
        return BAD_ARGS;

    RB_node* cur_node = new_node;
    int ret_val = 0;

    while (cur_node->parent->color == Red)
    {
        if (cur_node->parent->parent->left_ch == cur_node->parent) // left branch
        {
            RB_node* uncle = cur_node->parent->parent->right_ch;
            if (uncle->color == Red)
            {
                cur_node->parent->color = Black;
                uncle->color = Black;

                cur_node = cur_node->parent->parent;
                cur_node->color = Red;
            }
            else
            {
                if(cur_node->parent->right_ch == cur_node)
                {
                    cur_node = cur_node->parent;
                    ret_val = left_rotate(tree, cur_node);
                    if (ret_val != 0)
                        return ret_val;
                }
                cur_node->parent->color = Black;
                cur_node->parent->parent->color = Red;

                ret_val = right_rotate(tree, cur_node->parent->parent);
                if (ret_val != 0)
                    return ret_val;
            }
        }
        else // right branch
        {
            RB_node* uncle = cur_node->parent->parent->left_ch;
            if (uncle->color == Red)
            {
                cur_node->parent->color = Black;
                uncle->color = Black;

                cur_node = cur_node->parent->parent;
                cur_node->color = Red;
            }
            else
            {
                if (cur_node->parent->left_ch == cur_node)
                {
                    cur_node = cur_node->parent;
                    ret_val = right_rotate(tree, cur_node);
                    if (ret_val != 0)
                        return ret_val;
                }
                cur_node->parent->parent->color = Red;
                cur_node->parent->color = Black;
                ret_val = left_rotate(tree, cur_node->parent->parent);
                if (ret_val != 0)
                    return ret_val;
            }
        }
    }

    tree->root->color = Black;

    return 0;
}

static int left_rotate(RB_tree* tree, RB_node* node)
{
    if (tree == NULL || node == NULL)
        return BAD_ARGS;

    if (node == tree->nil || node->right_ch == NULL)
        return WRONG_OPERATION;

    RB_node* child = node->right_ch;

    child->parent = node->parent;

    if (child->parent->left_ch == node)
        child->parent->left_ch = child;
    else
        child->parent->right_ch = child;

    node->parent = child;
    if (child->parent == tree->nil)
        tree->root = child;

    node->right_ch = child->left_ch;
    node->right_ch->parent = node;
    child->left_ch = node;

    return 0;
}

static int right_rotate(RB_tree* tree, RB_node* node)
{
    if (tree == NULL || node == NULL)
        return BAD_ARGS;

    if (node == tree->nil || node->left_ch == NULL)
        return WRONG_OPERATION;

    struct RB_node* child = node->left_ch;

    child->parent = node->parent;

    if (child->parent->left_ch == node)
        child->parent->left_ch = child;
    else
        child->parent->right_ch = child;

    node->parent = child;
    if (child->parent == tree->nil)
        tree->root = child;

    node->left_ch = child->right_ch;
    node->left_ch->parent = node;
    child->right_ch = node;

    return 0;
}

int tree_dump(FILE* out, RB_tree* tree)
{
    if (out == NULL || tree == NULL)
        return BAD_ARGS;

    //printf("num nodes = %lu\n", tree->num_nodes);

    fprintf(out, "digraph dump\n{\n");

    fprintf(out, "\tnode [color = \"#000000\", shape = \"box\", fontsize = 20];\n"
                 "\tedge [color = \"#000000\", fontsize = 20];\n\n");

    fprintf(out, "\t\troot_nil [label = \"root_nil\", shape = \"diamond\","
                 " color = \"#FFFFFF\", fontcolor = \"#000000\"];\n");


    if (tree->num_nodes == 0)
    {
        fprintf(out, "}\n");
        return 0;
    }

    size_t counter = tree->num_nodes;
    int ret = node_dump(out, tree->root, tree, &counter);

    fprintf(out, "\t\tkey_nil [label = \"nil\", shape = \"diamond\","
                 " color = \"#FFFFFF\", fontcolor = \"#000000\"];\n");

    fprintf(out, "}\n");

    if (ret != 0)
        return BAD_TREE_CONDITION;

    return 0;
}

static int node_dump(FILE* out, RB_node* node, RB_tree* tree, size_t* counter) // recurcive!!!!!
{
    if (node == NULL || tree == NULL || counter == NULL)
        return BAD_ARGS;

    if (*counter == 0)
        return ERROR;
    (*counter)--;

    fprintf(out, "\t\tkey_%d [label = \"%d\", ", node->key, node->key);
    if (node->color == Red)
        fprintf(out, "color = \"#FF0000\", style = \"filled\", "
                     "fillcolor = \"#FF0000\", fontcolor = \"#FFFFFF\"");
    else
        fprintf(out, "color = \"#000000\", style = \"filled\", "
                     "fillcolor = \"#000000\", fontcolor = \"#FFFFFF\"");
    fprintf(out, "];\n");

    if (node->parent == NULL)
        return ERROR;

    if (node->parent == tree->nil)
        fprintf(out, "\t\tkey_%d -> root_nil", node->key);
    else
        fprintf(out, "\t\tkey_%d -> key_%d", node->key, node->parent->key);
    fprintf(out, "[label = \"parent\"];\n");

    if (node->left_ch == NULL)
        return ERROR;

    if (node->left_ch == tree->nil)
        fprintf(out, "\t\tkey_%d -> key_nil [label = \"left\"];\n", node->key);
    else
    {
        fprintf(out, "\t\tkey_%d -> key_%d [label = \"left\"];\n", node->key, node->left_ch->key);
        int ret = node_dump(out, node->left_ch, tree, counter);
        if (ret < 0)
            return ret;
    }

    if (node->right_ch == NULL)
        return ERROR;

    if (node->right_ch == tree->nil)
        fprintf(out, "\t\tkey_%d -> key_nil [label = \"right\"];\n", node->key);
    else
    {
        fprintf(out, "\t\tkey_%d -> key_%d [label = \"right\"];\n", node->key, node->right_ch->key);

        int ret = node_dump(out, node->right_ch, tree, counter);
        if (ret < 0)
            return ret;
    }

    return 0;
}
