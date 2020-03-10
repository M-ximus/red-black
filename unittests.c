#include "rb-tree.h"
#include "stdio.h"

#define UNITTEST( what, opera, truth) \
{\
    if(!((what) opera (truth)))\
        printf("FAIL test!!! %s != %s", #what, #truth);\
    else\
        printf("Test passed\n");\
};

void Testcase_dump();
void Testcase_insert_right_fixup();
void Testcase_insert_left_fixup();
void Testcase_ctors();

int main()
{

    Testcase_dump();
    Testcase_ctors();
    Testcase_insert_left_fixup();
    Testcase_insert_right_fixup();

    return 0;
}

void Testcase_dump()
{
    int fail_dump = tree_dump(NULL, NULL);
    UNITTEST(fail_dump, <, 0);

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
}

void Testcase_ctors()
{
    RB_tree* tree = tree_ctor();
    UNITTEST(tree, !=, NULL);

    RB_node* node = node_ctor(25);
    UNITTEST(node, !=, NULL);
    UNITTEST(node->key, ==, 25);
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

    FILE* out = fopen("insert_left_fixup.dot", "w");
    if (out == NULL)
        perror("Open file error\n");

    int ret_val_dump_left_insert = tree_dump(out, tree);
    UNITTEST(ret_val_dump_left_insert, ==, 0);

    fclose(out);
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

    FILE* out = fopen("insert_right_fixup.dot", "w");
    if (out == NULL)
        perror("Open file error\n");

    int ret_val_dump_right_insert = tree_dump(out, tree);
    UNITTEST(ret_val_dump_right_insert, ==, 0);

    fclose(out);
}
