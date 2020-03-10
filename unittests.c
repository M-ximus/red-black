#include "rb-tree.h"
#include "stdio.h"

int main()
{
    RB_tree* My_rb = tree_ctor();
    if (My_rb == NULL)
    {
        printf("Tree constructor error\n");
        return -1;
    }

    int ret_val = 0;
    int arr_nums[9] = {11, 2, 14, 1, 7, 15, 5, 8, 4};

    for (int i = 0; i < 9; i++)
    {
        RB_node* new_node = node_ctor(arr_nums[i]);
        if (new_node == NULL)
        {
            printf("Node constructor error\n");
            return -1;
        }

        ret_val = RB_insert(My_rb, new_node);
        if (ret_val < 0)
            printf("insert fail = %d\n", ret_val);
    }

    FILE* out = fopen("debug.dot", "w");
    if (out == NULL)
        perror("Open file error\n");

     ret_val = tree_dump(out, My_rb);
    if (ret_val < 0)
        printf("ret_val tree dump = %d\n", ret_val);

    fclose(out);

    return 0;
}
