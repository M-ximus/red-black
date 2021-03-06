#include "rb-tree.h"
#include "stdio.h"

#define UNITTEST( what, opera, truth) \
{\
    if(!((what) opera (truth)))\
        printf("FAIL test!!! %s != %s\n", #what, #truth);\
    else\
        printf("Test passed\n");\
};

void Testcase_dump();
void Testcase_insert_right_fixup();
void Testcase_insert_left_fixup();
void Testcase_tree_ctor_and_dtor();
void Testcase_node_ctor_and_dtor();
void Testcase_delete_and_fixup();
void Testcase_search();
void Testcase_max();
void Testcase_min();
void Testcase_foreach();
void Testcase_memfail();

int main()
{
    Testcase_memfail();
    Testcase_node_ctor_and_dtor();
    Testcase_tree_ctor_and_dtor();
    Testcase_dump();
    Testcase_insert_left_fixup();
    Testcase_insert_right_fixup();
    Testcase_search();
    Testcase_max();
    Testcase_min();
    Testcase_foreach();
    Testcase_delete_and_fixup();

    return 0;
}

void Testcase_memfail()
{
    errno = 0;
    RB_node* fail_node = node_ctor(42);
    UNITTEST(errno, ==, ENOMEM);
    UNITTEST(fail_node, ==, NULL);

    errno = 0;
    RB_tree* fail_tree = tree_ctor();
    UNITTEST(errno, ==, ENOMEM);
    UNITTEST(fail_tree, ==, NULL);

    errno = 0;
    fail_tree = tree_ctor();
    UNITTEST(errno, ==, ENOMEM);
    UNITTEST(fail_tree, ==, NULL);
}

void Testcase_node_ctor_and_dtor()
{
    RB_node* node1 = node_ctor(25);
    UNITTEST(node1, !=, NULL);
    UNITTEST(node1->key, ==, 25);

    int ret_node_dtor = node_dtor(node1);
    UNITTEST(ret_node_dtor, ==, 0);

    ret_node_dtor = node_dtor(NULL);
    UNITTEST(ret_node_dtor, ==, BAD_ARGS);
}

void Testcase_tree_ctor_and_dtor()
{
    RB_tree* tree = tree_ctor();
    UNITTEST(tree, !=, NULL);

    for (int i = 0; i < 3; i++)
    {
        RB_node* node = node_ctor(i);
        UNITTEST(node, !=, NULL);
        UNITTEST(node->key, ==, i);

        int ret_insert = RB_insert(tree, node);
        UNITTEST(ret_insert, ==, 0);
    }

    int ret_tree_dtor = tree_dtor(tree);
    UNITTEST(ret_tree_dtor, ==, 0);
////////////////////////////////////////////////////////////////////////////////
    ret_tree_dtor = tree_dtor(NULL);
    UNITTEST(ret_tree_dtor, ==, BAD_ARGS);
////////////////////////////////////////////////////////////////////////////////
    tree = tree_ctor();
    UNITTEST(tree, !=, NULL);

    int ret_empty_tree_dtor = tree_dtor(tree);
    UNITTEST(ret_empty_tree_dtor, ==, 0);
////////////////////////////////////////////////////////////////////////////////
    tree = tree_ctor();
    UNITTEST(tree, !=, NULL);

    for (int i = 0; i < 3; i++)
    {
        RB_node* node = node_ctor(i);
        UNITTEST(node, !=, NULL);
        UNITTEST(node->key, ==, i);

        int ret_insert = RB_insert(tree, node);
        UNITTEST(ret_insert, ==, 0);
    }

    (tree->num_nodes)++;
    int ret_incr_tree_dtor = tree_dtor(tree);
    UNITTEST(ret_incr_tree_dtor, ==, 1);
    int ret_nil_dtor = node_dtor(tree->nil);
    UNITTEST(ret_nil_dtor, ==, 0);

    tree->root = NULL;
    tree->nil = NULL;
    tree->num_nodes = 0;

    free(tree);
////////////////////////////////////////////////////////////////////////////////
    tree = tree_ctor();
    UNITTEST(tree, !=, NULL);

    RB_node* node1 = node_ctor(1);
    UNITTEST(node1, !=, NULL);
    UNITTEST(node1->key, ==, 1);

    int ret_insert = RB_insert(tree, node1);
    UNITTEST(ret_insert, ==, 0);

    RB_node* node2 = node_ctor(2);
    UNITTEST(node2, !=, NULL);
    UNITTEST(node2->key, ==, 2);

    ret_insert = RB_insert(tree, node2);
    UNITTEST(ret_insert, ==, 0);

    tree->num_nodes = 0;

    int ret_bad_counter_tree_dtor = tree_dtor(tree);
    UNITTEST(ret_bad_counter_tree_dtor, ==, E_TOO_MUCH_ELEM);

    tree->num_nodes = 2;
    ret_tree_dtor = tree_dtor(tree);
    UNITTEST(ret_tree_dtor, ==, 0);
////////////////////////////////////////////////////////////////////////////////
    tree = tree_ctor();
    UNITTEST(tree, !=, NULL);

    node1 = node_ctor(2);
    UNITTEST(node1, !=, NULL);
    UNITTEST(node1->key, ==, 2);

    ret_insert = RB_insert(tree, node1);
    UNITTEST(ret_insert, ==, 0);

    node2 = node_ctor(1);
    UNITTEST(node2, !=, NULL);
    UNITTEST(node2->key, ==, 1);

    ret_insert = RB_insert(tree, node2);
    UNITTEST(ret_insert, ==, 0);

    tree->num_nodes = 0;

    ret_bad_counter_tree_dtor = tree_dtor(tree);
    UNITTEST(ret_bad_counter_tree_dtor, ==, E_TOO_MUCH_ELEM);

    tree->num_nodes = 2;
    ret_tree_dtor = tree_dtor(tree);
    UNITTEST(ret_tree_dtor, ==, 0);
////////////////////////////////////////////////////////////////////////////////
    tree = tree_ctor();
    UNITTEST(tree, !=, NULL);

    RB_node* save_nil = tree->nil;
    tree->nil = NULL;

    int ret_null_node_tree_dtor = tree_dtor(tree);
    UNITTEST(ret_null_node_tree_dtor, ==, BAD_ARGS);

    tree->nil = save_nil;
    ret_tree_dtor = tree_dtor(tree);
    UNITTEST(ret_tree_dtor, ==, 0);
}

void Testcase_dump()
{
    int fail_dump = tree_dump(NULL, NULL);
    UNITTEST(fail_dump, <, 0);
////////////////////////////////////////////////////////////////////////////////
    RB_tree* tree = tree_ctor();
    UNITTEST(tree, !=, NULL);

    FILE* out_empty = fopen("dump_empty.dot", "w");
    if (out_empty == NULL)
    {
        perror("Open out_empty file error\n");
        exit(-1);
    }
    int dump_empty_tree = tree_dump(out_empty, tree);
    UNITTEST(dump_empty_tree, ==, 0);
    fclose(out_empty);

////////////////////////////////////////////////////////////////////////////////
    FILE* out_tree = fopen("dump_tree.dot", "w");
    if (out_tree == NULL)
    {
        perror("Open out_tree file error\n");
        exit(-1);
    }

    int ret_dump_insert = 0;
    int arr_nums[3] = {11, 2, 14};

    for (int i = 0; i < 3; i++)
    {
        RB_node* new_node = node_ctor(arr_nums[i]);

        ret_dump_insert = RB_insert(tree, new_node);
        UNITTEST(ret_dump_insert, ==, 0);
    }

    int dump_tree = tree_dump(out_tree, tree);
    UNITTEST(dump_tree, ==, 0);
    fclose(out_tree);

    int ret_dtor = tree_dtor(tree);
    UNITTEST(ret_dtor, ==, 0);
////////////////////////////////////////////////////////////////////////////////
    tree = tree_ctor();
    UNITTEST(tree, !=, NULL);

    for (int i = 0; i < 3; i++)
    {
        RB_node* new_node = node_ctor(arr_nums[i]);

        ret_dump_insert = RB_insert(tree, new_node);
        UNITTEST(ret_dump_insert, ==, 0);
    }

    FILE* nothing = fopen("nothing.dot", "w");
    if (nothing == NULL)
    {
        perror("Open nothing file error\n");
        exit(-1);
    }

    RB_node* save_root = tree->root;
    tree->root = NULL;
    int dump_nothing_tree = tree_dump(nothing, tree);
    UNITTEST(dump_nothing_tree, ==, BAD_TREE_CONDITION);
    tree->root = save_root;
////////////////////////////////////////////////////////////////////////////////
    tree->num_nodes = 2;
    dump_nothing_tree = tree_dump(nothing, tree);
    UNITTEST(dump_nothing_tree, ==, BAD_TREE_CONDITION);
    tree->num_nodes = 3;
////////////////////////////////////////////////////////////////////////////////
    tree->num_nodes = 1;
    dump_nothing_tree = tree_dump(nothing, tree);
    UNITTEST(dump_nothing_tree, ==, BAD_TREE_CONDITION);
    tree->num_nodes = 3;
////////////////////////////////////////////////////////////////////////////////
    tree->root->right_ch->right_ch = NULL;
    dump_nothing_tree = tree_dump(nothing, tree);
    UNITTEST(dump_nothing_tree, ==, BAD_TREE_CONDITION);
    tree->root->right_ch->right_ch = tree->nil;
////////////////////////////////////////////////////////////////////////////////
    tree->root->right_ch->left_ch = NULL;
    dump_nothing_tree = tree_dump(nothing, tree);
    UNITTEST(dump_nothing_tree, ==, BAD_TREE_CONDITION);
    tree->root->right_ch->left_ch = tree->nil;
////////////////////////////////////////////////////////////////////////////////
    tree->root->right_ch->parent = NULL;
    dump_nothing_tree = tree_dump(nothing, tree);
    UNITTEST(dump_nothing_tree, ==, BAD_TREE_CONDITION);
    tree->root->right_ch->parent = tree->root;
////////////////////////////////////////////////////////////////////////////////
    fclose(nothing);
    ret_dtor = tree_dtor(tree);
    UNITTEST(ret_dtor, ==, 0);
}

void Testcase_insert_left_fixup()
{
    RB_tree* tree = tree_ctor();
    UNITTEST(tree, !=, NULL);

    int ret_val_RB_insert = 0;
    int arr_nums[9] = {11, 2, 14, 1, 7, 15, 5, 8, 4};

    for (int i = 0; i < 9; i++)
    {
        RB_node* new_node = node_ctor(arr_nums[i]);

        ret_val_RB_insert = RB_insert(tree, new_node);
        UNITTEST(ret_val_RB_insert, ==, 0);
    }

    UNITTEST(tree->root->color, ==, Black);
    UNITTEST(tree->root->key, ==, 7);
    UNITTEST(tree->root->left_ch->key, ==, 2);
    UNITTEST(tree->root->left_ch->color, ==, Red);
    UNITTEST(tree->root->left_ch->right_ch->key, ==, 5);
    UNITTEST(tree->root->left_ch->right_ch->color, ==, Black);
    UNITTEST(tree->root->left_ch->right_ch->left_ch->key, ==, 4);
    UNITTEST(tree->root->left_ch->right_ch->left_ch->color, ==, Red);

    FILE* out = fopen("insert_left_fixup.dot", "w");
    if (out == NULL)
        perror("Open file error\n");

    int ret_val_dump_left_insert = tree_dump(out, tree);
    UNITTEST(ret_val_dump_left_insert, ==, 0);
    fclose(out);

    int ret_dtor = tree_dtor(tree);
    UNITTEST(ret_dtor, ==, 0);
}

void Testcase_insert_right_fixup()
{
    RB_tree* tree = tree_ctor();
    UNITTEST(tree, !=, NULL);

    int fail_insert = RB_insert(NULL, NULL);
    UNITTEST(fail_insert, <, 0);

    int ret_val_RB_insert = 0;
    int arr_nums[9] = {11, 7, 16, 19, 13, 5, 14, 12, 15};

    for (int i = 0; i < 9; i++)
    {
        RB_node* new_node = node_ctor(arr_nums[i]);

        ret_val_RB_insert = RB_insert(tree, new_node);
        UNITTEST(ret_val_RB_insert, ==, 0);
    }

    UNITTEST(tree->root->color, ==, Black);
    UNITTEST(tree->root->key, ==, 13);
    UNITTEST(tree->root->right_ch->key, ==, 16);
    UNITTEST(tree->root->right_ch->left_ch->key, ==, 14);
    UNITTEST(tree->root->right_ch->left_ch->color, ==, Black);
    UNITTEST(tree->root->right_ch->left_ch->right_ch->key, ==, 15);
    UNITTEST(tree->root->right_ch->left_ch->right_ch->color, ==, Red);

    FILE* out = fopen("insert_right_fixup.dot", "w");
    if (out == NULL)
        perror("Open file error\n");

    int ret_val_dump_right_insert = tree_dump(out, tree);
    UNITTEST(ret_val_dump_right_insert, ==, 0);
    fclose(out);

    int ret_dtor = tree_dtor(tree);
    UNITTEST(ret_dtor, ==, 0);
}

void Testcase_delete_and_fixup()
{
    int ret_NULL_tree_delete = RB_delete(NULL, NULL);
    UNITTEST(ret_NULL_tree_delete, ==, BAD_ARGS);
////////////////////////////////////////////////////////////////////////////////
    RB_tree* tree = tree_ctor();
    UNITTEST(tree, !=, NULL);

    int ret_NULL_node_delete = RB_delete(tree, NULL);
    UNITTEST(ret_NULL_node_delete, ==, BAD_ARGS);
////////////////////////////////////////////////////////////////////////////////
    int ret_val_RB_insert = 0;
    int arr_nums[9] = {11, 2, 14, 1, 7, 15, 5, 8, 4};

    for (int i = 0; i < 9; i++)
    {
        RB_node* new_node = node_ctor(arr_nums[i]);

        ret_val_RB_insert = RB_insert(tree, new_node);
        UNITTEST(ret_val_RB_insert, ==, 0);
    }

    int ret_del_root = RB_delete(tree, tree->root);
    UNITTEST(ret_del_root, ==, 0);
    UNITTEST(tree->root->key, ==, 8);
    UNITTEST(tree->root->right_ch->key, ==, 14);
    UNITTEST(tree->root->right_ch->color, ==, Red);
////////////////////////////////////////////////////////////////////////////////
    int ret_no_right_delete = RB_delete(tree, tree->root->left_ch->right_ch);
    UNITTEST(ret_no_right_delete, ==, 0);
    UNITTEST(tree->root->left_ch->right_ch->key, ==, 4);
    UNITTEST(tree->root->left_ch->right_ch->color, ==, Black);
////////////////////////////////////////////////////////////////////////////////
    int ret_delete_min = RB_delete(tree, tree->root->left_ch->left_ch);
    UNITTEST(ret_delete_min, ==, 0);
    int ret_no_left_delete = RB_delete(tree, tree->root->left_ch);
    UNITTEST(ret_no_left_delete, ==, 0);

    int ret_dtor = tree_dtor(tree);
    UNITTEST(ret_dtor, ==, 0);
////////////////////////////////////////////////////////////////////////////////
    tree = tree_ctor();
    UNITTEST(tree, !=, NULL);

    ret_val_RB_insert = 0;
    int arr_nums2[16] = {11, 7, 16, 19, 13, 5, 14, 12, 15, 18, 20, 24, 42, 9, 17, 10};

    for (int i = 0; i < 16; i++)
    {
        RB_node* new_node = node_ctor(arr_nums2[i]);

        ret_val_RB_insert = RB_insert(tree, new_node);
        UNITTEST(ret_val_RB_insert, ==, 0);
    }

    int ret_del_right = RB_delete(tree, tree->root->left_ch);
    UNITTEST(ret_del_right, ==, 0);
    UNITTEST(tree->root->left_ch->right_ch->key, ==, 10);
    UNITTEST(tree->root->left_ch->right_ch->color, ==, Red);

    ret_dtor = tree_dtor(tree);
    UNITTEST(ret_dtor, ==, 0);
////////////////////////////////////////////////////////////////////////////////
    tree = tree_ctor();
    UNITTEST(tree, !=, NULL);

    int arr_nums3[10] = {11, 7, 16, 19, 13, 5, 14, 12, 15, 18};

    for (int i = 0; i < 10; i++)
    {
        RB_node* new_node = node_ctor(arr_nums3[i]);

        ret_val_RB_insert = RB_insert(tree, new_node);
        UNITTEST(ret_val_RB_insert, ==, 0);
    }

    int ret_zombie_first_left = RB_delete(tree, tree->root->left_ch);
    UNITTEST(ret_zombie_first_left, ==, 0);
    ret_zombie_first_left = RB_delete(tree, tree->root->left_ch->left_ch);
    UNITTEST(ret_zombie_first_left, ==, 0);
    tree->root->left_ch->right_ch->color = Black;
    tree->root->right_ch->left_ch->right_ch->color = Black;
    tree->root->right_ch->right_ch->left_ch->color = Black;
    ret_zombie_first_left = RB_delete(tree, tree->root->left_ch);
    UNITTEST(ret_zombie_first_left, ==, 0);
    UNITTEST(tree->root->left_ch->right_ch->color, ==, Red);

    int ret_zombie_second_right = RB_delete(tree, tree->root->left_ch->right_ch);
    UNITTEST(ret_zombie_second_right, ==, 0);
    ret_zombie_second_right = RB_delete(tree, tree->root->right_ch);
    UNITTEST(ret_zombie_second_right, ==, 0);

    ret_dtor = tree_dtor(tree);
    UNITTEST(ret_dtor, ==, 0);
////////////////////////////////////////////////////////////////////////////////
    tree = tree_ctor();
    UNITTEST(tree, !=, NULL);

    int arr_nums4[10] = {11, 7, 16, 19, 13, 14, 12, 15, 18};

    for (int i = 0; i < 9; i++)
    {
        RB_node* new_node = node_ctor(arr_nums4[i]);

        ret_val_RB_insert = RB_insert(tree, new_node);
        UNITTEST(ret_val_RB_insert, ==, 0);
    }

    int ret_zombie_third_left = RB_delete(tree, tree->root->left_ch->left_ch);
    UNITTEST(ret_zombie_third_left, ==, 0);

    tree->root->left_ch->right_ch->color = Black;
    tree->root->right_ch->color = Black;
    tree->root->right_ch->left_ch->color = Red;
    tree->root->right_ch->left_ch->right_ch->color = Black;

    ret_zombie_third_left = RB_delete(tree, tree->root->left_ch);
    UNITTEST(ret_zombie_third_left, ==, 0);
////////////////////////////////////////////////////////////////////////////////
    int ret_nil_delete = RB_delete(tree, tree->nil);
    UNITTEST(ret_nil_delete, ==, ERROR);
////////////////////////////////////////////////////////////////////////////////
    RB_node* save = tree->root->right_ch->right_ch->left_ch;
    tree->root->right_ch->right_ch->left_ch = NULL;

    int ret_NULL_fixup = RB_delete(tree, tree->root->right_ch->right_ch);
    UNITTEST(ret_NULL_fixup, ==, BAD_ARGS);

    tree->root->right_ch->right_ch->left_ch = save;
    ret_dtor = tree_dtor(tree);
    UNITTEST(ret_dtor, ==, 0);
////////////////////////////////////////////////////////////////////////////////
    tree = tree_ctor();
    UNITTEST(tree, !=, NULL);

    RB_node* new_root = node_ctor(11);
    ret_val_RB_insert = RB_insert(tree, new_root);
    UNITTEST(ret_val_RB_insert, ==, 0);

    tree->root->right_ch = NULL;
    int ret_NULL_child = RB_delete(tree, tree->root);
    UNITTEST(ret_NULL_child, !=, 0);
    tree->root->right_ch = tree->nil;
////////////////////////////////////////////////////////////////////////////////
    FILE* out = fopen("delete_left.dot", "w");
    if (out == NULL)
        perror("Open file error\n");

    int ret_dump_left_delete = tree_dump(out, tree);
    UNITTEST(ret_dump_left_delete, ==, 0);
    fclose(out);

    ret_dtor = tree_dtor(tree);
    UNITTEST(ret_dtor, ==, 0);
}

void Testcase_search()
{
    RB_node* ret_NULL_search = RB_search(NULL, 42);
    UNITTEST(ret_NULL_search, ==, NULL);
////////////////////////////////////////////////////////////////////////////////
    RB_tree* tree = tree_ctor();
    UNITTEST(tree, !=, NULL);

    RB_node* ret_empty_tree_search = RB_search(tree, 42);
    UNITTEST(ret_empty_tree_search, ==, NULL);
////////////////////////////////////////////////////////////////////////////////
    int ret_val_RB_insert = 0;
    int arr_nums[9] = {11, 2, 14, 1, 7, 15, 5, 8, 4};

    for (int i = 0; i < 9; i++)
    {
        RB_node* new_node = node_ctor(arr_nums[i]);

        ret_val_RB_insert = RB_insert(tree, new_node);
        UNITTEST(ret_val_RB_insert, ==, 0);
    }

    RB_node* ret_right_search = RB_search(tree, 15);
    UNITTEST(ret_right_search->key, ==, 15);

    RB_node* ret_left_search = RB_search(tree, 1);
    UNITTEST(ret_left_search->key, ==, 1);

    RB_node* ret_search_nonexistent_key = RB_search(tree, 42);
    UNITTEST(ret_search_nonexistent_key, ==, NULL);
////////////////////////////////////////////////////////////////////////////////
    tree->num_nodes = 0;
    RB_node* ret_loopcheck_search = RB_search(tree, 15);
    UNITTEST(ret_loopcheck_search, ==, NULL);
    tree->num_nodes = 9;
////////////////////////////////////////////////////////////////////////////////
    int ret_dtor = tree_dtor(tree);
    UNITTEST(ret_dtor, ==, 0);
}

void Testcase_max()
{
    RB_node* ret_NULL_tree_max = max_node(NULL, NULL);
    UNITTEST(ret_NULL_tree_max, ==, NULL);
////////////////////////////////////////////////////////////////////////////////
    RB_tree* tree = tree_ctor();
    UNITTEST(tree, !=, NULL);

    ret_NULL_tree_max = max_node(tree, NULL);
    UNITTEST(ret_NULL_tree_max, ==, NULL);
////////////////////////////////////////////////////////////////////////////////
    for (int i = 0; i < 3; i++)
    {
        RB_node* new_node = node_ctor(i);

        int ret_dump_insert = RB_insert(tree, new_node);
        UNITTEST(ret_dump_insert, ==, 0);
    }

    RB_node* ret_tree_max = max_node(tree, tree->root);
    UNITTEST(ret_tree_max->key, ==, 2);
////////////////////////////////////////////////////////////////////////////////
    tree->num_nodes = 0;
    RB_node* ret_loopcheck_max = max_node(tree, tree->root);
    UNITTEST(ret_loopcheck_max, ==, NULL);
    tree->num_nodes = 3;
////////////////////////////////////////////////////////////////////////////////
    int ret_dtor = tree_dtor(tree);
    UNITTEST(ret_dtor, ==, 0);
}

void Testcase_min()
{
    RB_node* ret_NULL_tree_min = min_node(NULL, NULL);
    UNITTEST(ret_NULL_tree_min, ==, NULL);
////////////////////////////////////////////////////////////////////////////////
    RB_tree* tree = tree_ctor();
    UNITTEST(tree, !=, NULL);

    ret_NULL_tree_min = min_node(tree, NULL);
    UNITTEST(ret_NULL_tree_min, ==, NULL);
////////////////////////////////////////////////////////////////////////////////
    for (int i = 0; i < 3; i++)
    {
        RB_node* new_node = node_ctor(i);

        int ret_dump_insert = RB_insert(tree, new_node);
        UNITTEST(ret_dump_insert, ==, 0);
    }

    RB_node* ret_tree_min = min_node(tree, tree->root);
    UNITTEST(ret_tree_min->key, ==, 0);
////////////////////////////////////////////////////////////////////////////////
    tree->num_nodes = 0;
    RB_node* ret_loopcheck_min = min_node(tree, tree->root);
    UNITTEST(ret_loopcheck_min, ==, NULL);
    tree->num_nodes = 3;
////////////////////////////////////////////////////////////////////////////////
    int ret_dtor = tree_dtor(tree);
    UNITTEST(ret_dtor, ==, 0);
}

static int sum(RB_tree* tree, RB_node* node, void* data)
{
    if (data == NULL)
        return -1;

    *((int*) data) += node->key;
    return *((int*) data);
}

static int crying_left_func(RB_tree* tree, RB_node* node, void* data)
{
    if (node->parent == tree->nil)
        return 0;
    if (node->parent->left_ch == node)
        return ERROR;
    return 0;
}

static int crying_right_func(RB_tree* tree, RB_node* node, void* data)
{
    if(node->parent == tree->nil)
        return 0;
    if(node->parent->right_ch == node)
        return ERROR;
    return 0;
}

void Testcase_foreach()
{
    int ret_NULL_tree_foreach = foreach(NULL, NULL, NULL);
    UNITTEST(ret_NULL_tree_foreach, ==, BAD_ARGS);
////////////////////////////////////////////////////////////////////////////////
    RB_tree* tree = tree_ctor();
    UNITTEST(tree, !=, NULL);

    int ret_NULL_func_foreach = foreach(tree, NULL, NULL);
    UNITTEST(ret_NULL_func_foreach, ==, BAD_ARGS);
////////////////////////////////////////////////////////////////////////////////
    int tree_sum = 0;
    int ret_empty_tree_foreach = foreach(tree, sum, &tree_sum);
    UNITTEST(ret_empty_tree_foreach, ==, EMPTY_TREE);
////////////////////////////////////////////////////////////////////////////////
    for (int i = 0; i < 3; i++)
    {
        RB_node* new_node = node_ctor(i);

        int ret_dump_insert = RB_insert(tree, new_node);
        UNITTEST(ret_dump_insert, ==, 0);
    }

    int ret_sum_foreach = foreach(tree, sum, &tree_sum);
    UNITTEST(ret_sum_foreach, ==, 0);
    UNITTEST(tree_sum, ==, 3);
////////////////////////////////////////////////////////////////////////////////
    int ret_left_error_foreach = foreach(tree, crying_left_func, NULL);
    UNITTEST(ret_left_error_foreach, ==, ERROR);

    int ret_right_error_foreach = foreach(tree, crying_right_func, NULL);
    UNITTEST(ret_right_error_foreach, ==, ERROR);
////////////////////////////////////////////////////////////////////////////////
    tree->num_nodes = 0;
    int ret_loopcheck_foreach = foreach(tree, sum, &tree_sum);
    UNITTEST(ret_loopcheck_foreach, ==, E_TOO_MUCH_ELEM);
    tree->num_nodes = 3;
////////////////////////////////////////////////////////////////////////////////
    RB_node* saved_root = tree->root;
    tree->root = NULL;
    int ret_bad_node_foreach = foreach(tree, sum, &tree_sum);
    UNITTEST(ret_bad_node_foreach, ==, BAD_ARGS);
    tree->root = saved_root;
////////////////////////////////////////////////////////////////////////////////
    int ret_dtor = tree_dtor(tree);
    UNITTEST(ret_dtor, ==, 0);
}
