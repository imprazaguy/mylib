/*
The MIT License (MIT)

Copyright (c) 2016 Kuan-Chung Huang

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */

#ifdef RB_COMPACT
#include "rbtree_compact.h"
#else
#include "rbtree.h"
#endif
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define __UNUSED __attribute__((unused))

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

typedef struct A_NODE_
{
    int val;
    RB_NODE node;
} A_NODE;

#define A_NODE_KEY_CMP(key, node) ((key) - (node)->val)
#define A_NODE_CMP(n1, n2) ((n1)->val - (n2)->val)
RB_GEN(A_NODE_MAP, int, A_NODE, node, A_NODE_KEY_CMP, A_NODE_CMP)

static int get_rbtree_black_height(RB_NODE *node)
{
    if (node == NULL)
    {
        return 0;
    }
    int bh = (rb_color(node) == RB_BLACK ? 1 : 0);
    int lbh = get_rbtree_black_height(rb_child(node, RB_LEFT));
    int rbh = get_rbtree_black_height(rb_child(node, RB_RIGHT));
    //printf("%d:(%d, %d, %d)\n", RB_ENTRY(node, A_NODE, node)->val, bh, lbh, rbh);
    assert_int_equal(lbh, rbh);
    return bh + lbh;
}

static inline void validate_rbtree_black_height(RB_NODE *node, int bh)
{
    assert_int_equal(get_rbtree_black_height(node), bh);
}

static void traverse_rbtree(RB_NODE *node, A_NODE *sorted_node, size_t *index)
{
    if (node == NULL)
    {
        return;
    }
    traverse_rbtree(rb_child(node, RB_LEFT), sorted_node, index);
    assert_int_equal(sorted_node[*index].val, RB_ENTRY(node, A_NODE, node)->val);
    *index += 1;
    traverse_rbtree(rb_child(node, RB_RIGHT), sorted_node, index);
}

static void validate_rbtree_sorted_order(RB_ROOT *root, A_NODE *sorted_node, size_t n_node)
{
    size_t index = 0;
    traverse_rbtree(root->rb_root, sorted_node, &index);
    assert_int_equal(index, n_node);
}

static void test_rbtree_init(void **state __UNUSED)
{
    RB_ROOT root = RB_ROOT_INITIALIZER(&root);
    A_NODE node[] = {
        { .val = 1 },
    };
    RB_INSERT(A_NODE_MAP, &root, &node[0]);
    assert_non_null(root.rb_root);
    validate_rbtree_black_height(root.rb_root, 1);
    RB_REMOVE(A_NODE_MAP, &root, 1);
    assert_null(root.rb_root);
    validate_rbtree_black_height(root.rb_root, 0);
}

static void test_rbtree_insert_left(void **state __UNUSED)
{
    RB_ROOT root = RB_ROOT_INITIALIZER(&root);
    A_NODE node[] = {
        { .val = 1 },
        { .val = 2 },
        { .val = 3 },
        { .val = 4 },
        { .val = 5 },
        { .val = 6 },
        { .val = 7 },
        { .val = 8 },
    };

    /* Test case 2, 3 */
    RB_INSERT(A_NODE_MAP, &root, &node[7]);
    validate_rbtree_black_height(root.rb_root, 1);
    RB_INSERT(A_NODE_MAP, &root, &node[6]);
    validate_rbtree_black_height(root.rb_root, 1);
    RB_INSERT(A_NODE_MAP, &root, &node[1]);
    validate_rbtree_black_height(root.rb_root, 1);
    /* Test casse 1 */
    RB_INSERT(A_NODE_MAP, &root, &node[3]);
    validate_rbtree_black_height(root.rb_root, 2);
    RB_INSERT(A_NODE_MAP, &root, &node[0]);
    validate_rbtree_black_height(root.rb_root, 2);
    /* Test case 1 */
    RB_INSERT(A_NODE_MAP, &root, &node[2]);
    validate_rbtree_black_height(root.rb_root, 2);
    RB_INSERT(A_NODE_MAP, &root, &node[5]);
    validate_rbtree_black_height(root.rb_root, 2);
    /* Test case 1, 2, 3 */
    RB_INSERT(A_NODE_MAP, &root, &node[4]);
    validate_rbtree_black_height(root.rb_root, 2);

    validate_rbtree_sorted_order(&root, node, ARRAY_SIZE(node));
}

static void test_rbtree_insert_right(void **state __UNUSED)
{
    RB_ROOT root = RB_ROOT_INITIALIZER(&root);
    A_NODE node[] = {
        { .val = 1 },
        { .val = 2 },
        { .val = 3 },
        { .val = 4 },
        { .val = 5 },
        { .val = 6 },
        { .val = 7 },
        { .val = 8 },
    };

    /* Test case 2, 3 */
    RB_INSERT(A_NODE_MAP, &root, &node[0]);
    validate_rbtree_black_height(root.rb_root, 1);
    RB_INSERT(A_NODE_MAP, &root, &node[1]);
    validate_rbtree_black_height(root.rb_root, 1);
    RB_INSERT(A_NODE_MAP, &root, &node[6]);
    validate_rbtree_black_height(root.rb_root, 1);
    /* Test casse 1 */
    RB_INSERT(A_NODE_MAP, &root, &node[4]);
    validate_rbtree_black_height(root.rb_root, 2);
    RB_INSERT(A_NODE_MAP, &root, &node[7]);
    validate_rbtree_black_height(root.rb_root, 2);
    /* Test case 1 */
    RB_INSERT(A_NODE_MAP, &root, &node[5]);
    validate_rbtree_black_height(root.rb_root, 2);
    RB_INSERT(A_NODE_MAP, &root, &node[2]);
    validate_rbtree_black_height(root.rb_root, 2);
    /* Test case 1, 2, 3 */
    RB_INSERT(A_NODE_MAP, &root, &node[3]);
    validate_rbtree_black_height(root.rb_root, 2);

    validate_rbtree_sorted_order(&root, node, ARRAY_SIZE(node));
}

void shuffle(A_NODE **buf, const int N)
{
    int repeat;
    for (repeat = 0; repeat < N * 2; ++repeat)
    {
        int i = rand() % N;
        int j = rand() % (N - 1);
        if (j >= i) {
            j += 1;
        }
        A_NODE *t = buf[i];
        buf[i] = buf[j];
        buf[j] = t;
    }
}

static void test_rbtree_insert_random(void **state __UNUSED)
{
    int i;
    const int N = 1000;
    A_NODE node_buf[N];
    A_NODE *node[N];
    for (i = 0; i < N; ++i)
    {
        node_buf[i].val = i + 1;
        node[i] = &node_buf[i];
    }

    int run;
    for (run = 0; run < 1000; ++run)
    {
        shuffle(node, N);
        RB_ROOT root = RB_ROOT_INITIALIZER(&root);
        for (i = 0; i < N; ++i)
        {
            RB_INSERT(A_NODE_MAP, &root, node[i]);
            get_rbtree_black_height(root.rb_root);
        }
        validate_rbtree_sorted_order(&root, node_buf, N);
    }
}

void print_rbtree_walk(RB_NODE *node, int depth)
{
    int i;
    if (node == NULL)
    {
        for (i = 0; i < depth; ++i)
        {
            putchar('\t');
        }
        printf("NIL\n");
    }
    else
    {
        print_rbtree_walk(rb_child(node, RB_RIGHT), depth + 1);
        for (i = 0; i < depth; ++i)
        {
            putchar('\t');
        }
        printf("%d(%c)\n", RB_ENTRY(node, A_NODE, node)->val,
                rb_color(node) == RB_BLACK ? 'B' : 'R');
        print_rbtree_walk(rb_child(node, RB_LEFT), depth + 1);
    }
}

void print_rbtree(RB_ROOT *root)
{
    print_rbtree_walk(root->rb_root, 0);
}

static void init_rbtree(RB_ROOT *root, A_NODE *node, int N, int *seq)
{
    int i;
    RB_ROOT_INIT(root);
    for (i = 0; i < N; ++i)
    {
        node[i].val = i + 1;
    }
    for (i = 0; i < N; ++i)
    {
        RB_INSERT(A_NODE_MAP, root, &node[seq[i]]);
    }
}

static void test_rbtree_remove_simple(void **state __UNUSED)
{
    RB_ROOT root;
    A_NODE node[4];
    int seq[] = {2, 0, 3, 1};

    /* Test case: left==NULL, right==NULL */
    init_rbtree(&root, node, 4, seq);
    RB_REMOVE(A_NODE_MAP, &root, 4);
    validate_rbtree_black_height(root.rb_root, 2);

    /* Test case: left==NULL, right!=NULL */
    init_rbtree(&root, node, 4, seq);
    RB_REMOVE(A_NODE_MAP, &root, 1);
    validate_rbtree_black_height(root.rb_root, 2);
}

static void test_rbtree_remove_simple2(void **state __UNUSED)
{
    RB_ROOT root;
    A_NODE node[10];
    int seq[] = {5, 0, 9, 2, 6, 3, 7, 8, 4, 1};

    /* Test case: left!=NULL, righ!=NULL; successor==node */
    init_rbtree(&root, node, 10, seq);
    RB_REMOVE(A_NODE_MAP, &root, 3);
    validate_rbtree_black_height(root.rb_root, 2);

    /* Test case: lef!=NULL, right!=NULL; successor!=node*/
    init_rbtree(&root, node, 10, seq);
    RB_REMOVE(A_NODE_MAP, &root, 8);
    validate_rbtree_black_height(root.rb_root, 2);
}

static int node_cmp(const void *a, const void *b)
{
    const A_NODE *a_node = *(const A_NODE **) a;
    const A_NODE *b_node = *(const A_NODE **) b;
    return a_node->val - b_node->val;
}

static void test_rbtree_remove_random(void **state __UNUSED)
{
    int i;
    const int N = 1000;
    const int N2 = N / 4 * 3;
    A_NODE sorted_node[N2];
    A_NODE node_buf[N];
    A_NODE *node[N];
    for (i = 0; i < N; ++i)
    {
        node_buf[i].val = i + 1;
        node[i] = &node_buf[i];
    }

    int run;
    for (run = 0; run < 1000; ++run)
    {
        shuffle(node, N);
        /*
        for (i = 0; i < N; ++i)
        {
           printf(" %d", node[i]->val);
        }
        printf("\n");
        // */
        RB_ROOT root = RB_ROOT_INITIALIZER(&root);
        for (i = 0; i < N; ++i)
        {
            RB_INSERT(A_NODE_MAP, &root, node[i]);
            get_rbtree_black_height(root.rb_root);
        }
        shuffle(node, N);
        /*
        for(i = N2; i < N; ++i)
        {
           printf(" %d", node[i]->val);
        }
        printf("\n");
        // */
        for (i = N2; i < N; ++i)
        {
            RB_REMOVE(A_NODE_MAP, &root, node[i]->val);
            get_rbtree_black_height(root.rb_root);
        }
        qsort(node, N2, sizeof(node[0]), node_cmp);

        for (i = 0; i < N2; ++i)
        {
            sorted_node[i].val = node[i]->val;
        }
        validate_rbtree_sorted_order(&root, sorted_node, N2);
    }
}

static void test_rbtree_iter(void ** state __UNUSED)
{
    int i;
    const int N = 10;
    A_NODE node_buf[N];
    A_NODE *node[N];
    for (i = 0; i < N; ++i)
    {
        node_buf[i].val = i + 1;
        node[i] = &node_buf[i];
    }
    shuffle(node, N);

    RB_ROOT root = RB_ROOT_INITIALIZER(&root);
    for (i = 0; i < N; ++i)
    {
        RB_INSERT(A_NODE_MAP, &root, node[i]);
    }

    /* Test rb_first(), rb_next() */
    RB_NODE *iter;
#ifdef RB_COMPACT
    RB_PATH rp;
    iter = rb_first(&root, &rp);
#else
    iter = rb_first(&root);
#endif
    i = 0;
    while (iter != NULL)
    {
        assert_int_equal(node_buf[i].val, RB_ENTRY(iter, A_NODE, node)->val);
#ifdef RB_COMPACT
        iter = rb_next(iter, &rp);
#else
        iter = rb_next(iter);
#endif
        ++i;
    }
    assert_int_equal(i, N);

    /* Test rb_last(), rb_prev() */
#ifdef RB_COMPACT
    iter = rb_last(&root, &rp);
#else
    iter = rb_last(&root);
#endif
    i = N - 1;
    while (iter != NULL)
    {
        assert_int_equal(node_buf[i].val, RB_ENTRY(iter, A_NODE, node)->val);
#ifdef RB_COMPACT
        iter = rb_prev(iter, &rp);
#else
        iter = rb_prev(iter);
#endif
        --i;
    }
    assert_int_equal(i, -1);

    /* Test rb_find() */
    A_NODE *t;
#ifdef RB_COMPACT
    t = RB_FIND(A_NODE_MAP, &root, N / 2, &rp);
#else
    t = RB_FIND(A_NODE_MAP, &root, N / 2);
#endif
    iter = &t->node;
    i = N / 2 - 1;
    while (iter != NULL)
    {
        assert_int_equal(node_buf[i].val, RB_ENTRY(iter, A_NODE, node)->val);
#ifdef RB_COMPACT
        iter = rb_next(iter, &rp);
#else
        iter = rb_next(iter);
#endif
        ++i;
    }
    assert_int_equal(i, N);

    /* Test rb_remove() directly on iterator */
    int del_i;
    for (del_i = 0; del_i < i; ++del_i)
    {
        RB_ROOT_INIT(&root);
        for (i = 0; i < N; ++i)
        {
            RB_INSERT(A_NODE_MAP, &root, node[i]);
        }

#ifdef RB_COMPACT
        iter = rb_first(&root, &rp);
#else
        iter = rb_first(&root);
#endif
        for (i = 0; i < del_i; ++i)
        {
#ifdef RB_COMPACT
            iter = rb_next(iter, &rp);
#else
            iter = rb_next(iter);
#endif
        }
        assert_int_equal(node_buf[del_i].val, RB_ENTRY(iter, A_NODE, node)->val);
#ifdef RB_COMPACT
        rb_remove(&root, iter, &rp);
#else
        rb_remove(&root, iter);
#endif
        A_NODE sorted_node[N - 1];
        for (i = 0; i < del_i; ++i)
        {
            sorted_node[i].val = i + 1;
        }
        for (i = del_i + 1; i < N; ++i)
        {
            sorted_node[i - 1].val = i + 1;
        }
        validate_rbtree_sorted_order(&root, sorted_node, N - 1);
    }
}

int main(void)
{
    srand(time(NULL));
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_rbtree_init),
        cmocka_unit_test(test_rbtree_insert_left),
        cmocka_unit_test(test_rbtree_insert_right),
        cmocka_unit_test(test_rbtree_insert_random),
        cmocka_unit_test(test_rbtree_remove_simple),
        cmocka_unit_test(test_rbtree_remove_simple2),
        cmocka_unit_test(test_rbtree_remove_random),
        cmocka_unit_test(test_rbtree_iter),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
