#include "rbtree.h"
#include <stdlib.h>
#include <stdio.h>

void print_tree_structure(rbtree *t, node_t *n, int depth) {
    if (n == t->nil) {
        return;
    }

    for (int i = 0; i < depth; i++) {
        printf("  ");
    }

    printf("Node key: %d, Color: %s, ", n->key, n->color == RBTREE_RED ? "RED" : "BLACK");
    if (n->parent != t->nil) {
        printf("Parent key: %d", n->parent->key);
    } else {
        printf("Parent: NULL");
    }
    printf("\n");

    print_tree_structure(t, n->left, depth + 1);
    print_tree_structure(t, n->right, depth + 1);
}

int main() {
    rbtree *t = new_rbtree();
    node_t *node;
    // 트리에 노드 삽입 및 회전 테스트
    rbtree_insert(t, 10);
    rbtree_insert(t, 5);
    rbtree_insert(t, 5);
    rbtree_insert(t, 34);
    rbtree_insert(t, 6);
    rbtree_insert(t, 23);
    rbtree_insert(t, 12);
    rbtree_insert(t, 12);
    rbtree_insert(t, 6);
    rbtree_insert(t, 12);


    printf("Tree structure after right rotation:\n");
    print_tree_structure(t, t->root, 0);

    printf("----------------------------\n\n");
    // node = rbtree_min(t);
    // printf("최소값 : %d \n",node->key);
    // node = rbtree_max(t);
    // printf("최대값 : %d \n",node->key);


    printf("----------------------------\n\n");

    size_t n = 10;  // 배열의 크기
    key_t arr[n];
    
    rbtree_to_array(t, &arr, n);
    
    for (size_t i = 0; i < n; i++) {
        printf("%d \n", arr[i]);  // 배열 출력
    }
    printf('\n');

    delete_rbtree(t);

    return 0;
}