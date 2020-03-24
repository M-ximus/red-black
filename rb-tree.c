#include "rb-tree.h"

#ifdef DEVCHECKS
#define CHECK_RET(x) do {if ((x) < 0) return (x);} while(0)
#else
#define CHECK_RET(x)
#endif

#ifdef  DEVCHECKS
#define NOT_NULL(x) do {assert((x) != NULL);} while(0)
#else
#define NOT_NULL(x)
#endif

#ifdef TEST
#define Malloc(x) mymalloc(x)
#else
#define Malloc(x) malloc(x)
#endif

static RB_node* node_create();
static int insert_fixup(RB_tree* tree, RB_node* new_node);
static void left_rotate(RB_tree* tree, RB_node* node);
static void right_rotate(RB_tree* tree, RB_node* node);
static int node_dump(FILE* out, RB_node* node, RB_tree* tree, size_t* counter);
static int delete_fixup();
static int node_transplant(RB_tree* tree, RB_node* to, RB_node* who);
static int subtree_distruct(RB_node* root, RB_node* nil, size_t* counter);
static int call(RB_node* node, RB_tree* tree,
    int (*func)(RB_tree*, RB_node*, void*), void* data, size_t counter);


static void* mymalloc(size_t size)
{
    static int num_iter = 0;
    if (num_iter < 4)
    {
        num_iter++;
        if (num_iter != 2)
        {
            errno = ENOMEM;
            return NULL;
        }
    }
    return malloc(size);
 }

static RB_node* node_create()
{
    RB_node* node = (RB_node*) Malloc(sizeof(RB_node));
    if (node == NULL)
        return NULL;

    node->parent = NULL;
    node->left_ch = NULL;
    node->right_ch = NULL;

    node->color = Poison_color;
    node->key = Poison_key;

    return node;
}

RB_tree* tree_ctor()
{
    RB_tree* tree = (RB_tree*) Malloc(sizeof(RB_tree));
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

    if (ret_val != 0)
        return BAD_TREE_CONDITION;

    return 0;
}

static int insert_fixup(RB_tree* tree, RB_node* new_node)
{
// Develop time checks
    NOT_NULL(tree);
    NOT_NULL(new_node);

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
                    left_rotate(tree, cur_node);
                }
                cur_node->parent->color = Black;
                cur_node->parent->parent->color = Red;

                right_rotate(tree, cur_node->parent->parent);
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
                    right_rotate(tree, cur_node);
                }
                cur_node->parent->parent->color = Red;
                cur_node->parent->color = Black;
                left_rotate(tree, cur_node->parent->parent);
            }
        }
    }

    tree->root->color = Black;

    return 0;
}

static void left_rotate(RB_tree* tree, RB_node* node)
{
    RB_node* child = node->right_ch;

    child->parent = node->parent;

    if (child->parent->left_ch == node)
        child->parent->left_ch = child;
    else if (child->parent->right_ch == node)
        child->parent->right_ch = child;

    node->parent = child;
    if (child->parent == tree->nil)
        tree->root = child;

    node->right_ch = child->left_ch;

    if (node->right_ch != tree->nil)
        node->right_ch->parent = node;

    child->left_ch = node;

    return;
}

static void right_rotate(RB_tree* tree, RB_node* node)
{
    RB_node* child = node->left_ch;

    child->parent = node->parent;

    if (child->parent->left_ch == node)
        child->parent->left_ch = child;
    else if (child->parent->right_ch == node)
        child->parent->right_ch = child;

    node->parent = child;
    if (child->parent == tree->nil)
        tree->root = child;

    node->left_ch = child->right_ch;

    if (node->left_ch != tree->nil)
        node->left_ch->parent = node;

    child->right_ch = node;

    return;
}

int RB_delete(RB_tree* tree, RB_node* node)
{
    if (tree == NULL || node == NULL)
        return BAD_ARGS;

    RB_node* nil = tree->nil;
    RB_node* old = node;
    int old_orig_color = old->color;
    RB_node* replaced = NULL;

    if (node->right_ch == nil)
    {
        replaced = node->left_ch;
        int ret = node_transplant(tree, node, replaced);
        if (ret < 0)
            return ret;
    }
    else if (node->left_ch == nil)
    {
        replaced = node->right_ch;
        int ret = node_transplant(tree, node, replaced);
        if (ret < 0)
            return ret;
    }
    else
    {
        old = min_node(tree, node->right_ch);
        if (old == NULL)
            return ERROR;
        //Develop time checks
        //if (old->left_ch != nil)
            //return BAD_TREE_CONDITION;

        old_orig_color =  old->color;
        replaced = old->right_ch;
        if (old->parent == node)
            replaced->parent = old; // it heps us if replaced == nil
        else
        {
            int ret = node_transplant(tree, old, old->right_ch);
            if (ret < 0)
                return ret;
            old->right_ch = node->right_ch;
            old->right_ch->parent = old;
        }
        node_transplant(tree, node, old);
        old->left_ch = node->left_ch;
        old->left_ch->parent = old;
        old->color = node->color;
    }

    node_dtor(node);

    (tree->num_nodes)--;

    int ret = 0;
    if (old_orig_color == Black)
        ret = delete_fixup(tree, replaced);

    return ret;
}

static int node_transplant(RB_tree* tree, RB_node* to, RB_node* who)
{
    if (tree == NULL || to == NULL || who == NULL)
        return BAD_ARGS;

    RB_node* nil = tree->nil;
// Develop time checks
//    if (to == nil)
//        return WRONG_OPERATION;

    if (to->parent == nil)
        tree->root = who;
    else if (to == to->parent->left_ch)
        to->parent->left_ch = who;
    else
        to->parent->right_ch = who;

    who->parent = to->parent;

    return 0;
}

int node_dtor(RB_node* node)
{
    if (node == NULL)
        return BAD_ARGS;

    node->parent = NULL;
    node->left_ch = NULL;
    node->right_ch = NULL;
    node->color = Poison_color;
    node->key = Poison_key;

    free(node);

    return 0;
}

int tree_dtor(RB_tree* tree)
{
    if (tree == NULL)
        return BAD_ARGS;

    size_t counter = tree->num_nodes;

    int ret = subtree_distruct(tree->root, tree->nil, &counter);
    if (ret < 0)
        return ret;

    if (counter != 0)
    {
        tree->num_nodes = counter;
        return counter;
    }

    ret = node_dtor(tree->nil);

    tree->root = NULL;
    tree->nil = NULL;
    tree->num_nodes = 0;

    free(tree);

    return ret;
}

static int subtree_distruct(RB_node* root, RB_node* nil, size_t* counter)
{
    if (root == NULL || nil == NULL || counter == NULL)
        return BAD_ARGS;

    if (root == nil)
        return 0;

    int ret = 0;
    if (root->left_ch != nil || root->left_ch != NULL)
    {
        ret = subtree_distruct(root->left_ch, nil, counter);
        if (ret != 0)
            return ret;
    }
    if (root->right_ch != nil || root->right_ch != NULL)
    {
        ret = subtree_distruct(root->right_ch, nil, counter);
        if (ret != 0)
            return ret;
    }

    if (*counter == 0)
        return E_TOO_MUCH_ELEM;

    (*counter)--;

    ret = node_dtor(root);

    return ret;
}

static int delete_fixup(RB_tree* tree, RB_node* extra_black)
{
    if (tree == NULL || extra_black == NULL)
        return BAD_ARGS;

    RB_node* root = tree->root;
    while(extra_black->color == Black && extra_black != root)
    {
        if (extra_black->parent->left_ch == extra_black)
        {
            RB_node* bro = extra_black->parent->right_ch;
            if (bro->color == Red)
            {
                extra_black->parent->color = Red;
                bro->color = Black;

                left_rotate(tree, extra_black->parent);

                bro = extra_black->parent->right_ch;
            }
            if (bro->left_ch->color == Black && bro->right_ch->color == Black)
            {
                bro->color = Red;
                extra_black = extra_black->parent;
            }
            else
            {
                if (bro->right_ch->color == Black)
                {
                    bro->color = Red;
                    bro->left_ch->color = Black;

                    right_rotate(tree, bro);

                    bro = extra_black->parent->right_ch;
                }

                bro->color = extra_black->parent->color;
                extra_black->parent->color = Black;
                bro->right_ch->color = Black;

                left_rotate(tree, extra_black->parent);

                extra_black = root;
            }
        }
        else
        {
            RB_node* bro = extra_black->parent->left_ch;

            if (bro->color == Red)
            {
                bro->color = Black;
                extra_black->parent->color = Red;

                right_rotate(tree, extra_black->parent);

                bro = extra_black->parent->left_ch;
            }
            if (bro->left_ch->color == Black && bro->right_ch->color == Black)
            {
                bro->color = Red;
                extra_black = extra_black->parent;
            }
            else
            {
                if (bro->left_ch->color == Black)
                {
                    bro->right_ch->color = Black;
                    bro->color = Red;

                    left_rotate(tree, bro);

                    bro = extra_black->parent->left_ch;
                }

                bro->color = extra_black->parent->color;
                extra_black->parent->color = Black;
                bro->left_ch->color = Black;

                right_rotate(tree, extra_black->parent);

                extra_black = root;
            }
        }
    }
    extra_black->color = Black;

    return 0;
}

int foreach(RB_tree* tree, int (*func)(RB_tree*, RB_node*, void*), void* data)
{
    if (tree == NULL || func == NULL)
        return BAD_ARGS;

    int ret = call(tree->root, tree, func, data, tree->num_nodes);

    return ret;
}

static int call(RB_node* node, RB_tree* tree,
    int (*func)(RB_tree*, RB_node*, void*), void* data, size_t counter)
{
    if (node == NULL || tree == NULL || func == NULL)
        return BAD_ARGS;

    RB_node* nil = tree->nil;
    if (node == nil)
        return EMPTY_TREE;

    int ret = 0;

    if (counter <= 0)
        return E_TOO_MUCH_ELEM;

    if (node->left_ch != nil)
    {
        ret = call(node->left_ch, tree, func, data, counter - 1);
        if (ret < 0)
            return ret;
    }

    ret = func(tree, node, data);
    if (ret < 0)
        return ret;

    if (node->right_ch != nil)
    {
        ret = call(node->right_ch, tree, func, data, counter - 1);
        if (ret < 0)
            return ret;
    }

    return 0;
}

RB_node* min_node(RB_tree* tree, RB_node* node)
{
    if (tree == NULL || node == NULL)
        return NULL;

    size_t counter = tree->num_nodes;
    RB_node* nil = tree->nil;
    RB_node* min = node;

    for(; counter > 0; counter--)
    {
        if (min->left_ch == nil)
            return min;
        min = min->left_ch;
    }

    return NULL;
}

RB_node* max_node(RB_tree* tree, RB_node* node)
{
    if (tree == NULL || node == NULL)
        return NULL;

    size_t counter = tree->num_nodes;
    RB_node* nil = tree->nil;
    RB_node* max = tree->root;

    for (; counter > 0; counter--)
    {
        if (max->right_ch == nil)
            return max;
        max = max->right_ch;
    }

    return NULL;
}

RB_node* RB_search(RB_tree* tree, int key)
{
    if (tree == NULL)
        return NULL;

    RB_node* cur_node = tree->root;
    size_t counter = tree->num_nodes;

    for(; counter > 0; counter--)
    {
        if (cur_node == tree->nil)
            return NULL;
        if (key > cur_node->key)
            cur_node = cur_node->right_ch;
        else if (key < cur_node->key)
            cur_node = cur_node->left_ch;
        else
            return cur_node;
    }

    return NULL;
}

int tree_dump(FILE* out, RB_tree* tree)
{
    if (out == NULL || tree == NULL)
        return BAD_ARGS;

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
