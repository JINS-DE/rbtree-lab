#include "rbtree.h"
#include <stdlib.h>

//추가 함수
void rbtree_insert_fixup(rbtree *t, node_t *node);
void left_rotate(rbtree *t, node_t *node);
void right_rotate(rbtree *t, node_t *x);
void delete_node_postorder(rbtree *t,node_t *node);
void inorder(const rbtree *t,node_t *node, key_t **arr);
void rbtree_transplant(rbtree *t, node_t *u, node_t *v);
void delete_fixup(rbtree *t, node_t *x);
node_t *tree_min(const rbtree *t, node_t *n);


rbtree *new_rbtree(void) {
    // rbtree 구조체에 대한 메모리 할당
    rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
    if (p == NULL) {
        return NULL;  // 메모리 할당 실패 시 NULL 리턴
    }

    // nil 노드에 대한 메모리 할당
    node_t *nil = (node_t *)calloc(1, sizeof(node_t));
    if (nil == NULL) {
        free(nil);  // 할당된 트리 메모리 해제
        return NULL;  // 메모리 할당 실패 시 NULL 리턴
    }
    nil->color = RBTREE_BLACK;
    nil->parent = nil->left = nil->right = NULL;

    // 트리 초기화
    p->nil = nil;  // 트리의 nil 노드를 설정
    p->root = p->nil;   // 초기 루트는 nil을 가리킴 (빈 트리)
    return p;
}

void delete_node_postorder(rbtree *t, node_t *node) {
    if (node == t->nil) {
        return; 
    }
    delete_node_postorder(t, node->left);   // 왼쪽 서브트리 삭제
    delete_node_postorder(t, node->right);  // 오른쪽 서브트리 삭제
    free(node);  // 현재 노드 삭제
}

void delete_rbtree(rbtree *t) {
    // 트리의 모든 노드를 삭제합니다.
    delete_node_postorder(t, t->root);
    
    // nil 노드와 트리 구조체 자체의 메모리를 해제합니다.
    free(t->nil);
    free(t);  
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // 노드 생성
  node_t *new_node = (node_t *)calloc(1,sizeof(node_t));
  if (new_node==NULL){
    return NULL;
  }
  new_node->key = key;
  new_node->color = RBTREE_RED; // 새 노드는 항상 빨간색 삽입
  new_node->left = new_node->right = t->nil; // 리프 노드는 nil로 초기화

  // parent 초기화  
  node_t *parent = t->nil;
  // current 루트부터 시작
  node_t *current = t->root;
  
  // 삽입 위치 찾기
  while (current != t->nil){
    parent = current;
    if (new_node->key > current->key){
      current = current->right;
    } else{
      current = current->left;
    }
  }

  // 삽입
  new_node->parent = parent;
  if (parent==t->nil) // 
  {
    t->root = new_node; // 트리가 비어있는 경우 새로운 노드가 루트가 됨
  }
  else if (parent->key < new_node->key){
    parent->right = new_node;
  }
  else{
    parent->left = new_node;
  }

  // 레드-블랙 트리의 속성을 유지하기 위해 재조정 수행
  rbtree_insert_fixup(t, new_node);

  return new_node;

}
void rbtree_insert_fixup(rbtree *t, node_t *node){
  node_t* uncle;
  while (node->parent->color == RBTREE_RED)
  {
    // fix_up해야하는 node의 부모는 할아버지의 왼쪽 자식인가?
    if (node->parent == node->parent->parent->left){
      // 삼촌노드 = 부모의 형제노드
      uncle = node->parent->parent->right;
      // 삼촌노드가 레드인가? 삼촌과 부모가 둘다 red 일 때는 할아버지와 색깔 swap
      if (uncle->color == RBTREE_RED){
        uncle->color=RBTREE_BLACK; // 삼촌 BLACK
        node->parent->color = RBTREE_BLACK; // 부모 BLACK
        node->parent->parent->color = RBTREE_RED; // 할아버지 RED
        node=node->parent->parent; // 노드 cursor 할아버지로 초기화, 할아버지가 red로 바꼈기 때문에 조상들과 유효성 검사를 다시 해야 한다.
      }
      else
      {
         // 케이스 2와 3: 삼촌 노드가 검은색인 경우
          if (node == node->parent->right) {
              node = node->parent;
              left_rotate(t, node);
          }
          node->parent->color = RBTREE_BLACK;
          node->parent->parent->color = RBTREE_RED;
          right_rotate(t, node->parent->parent);
      }
    }
    // new_node의 부모는 할아버지의 오른쪽 자식인가?
    else{
        // node->parent가 node->parent->parent의 오른쪽 자식인 경우
        uncle = node->parent->parent->left;
        if (uncle->color == RBTREE_RED) {
            // 케이스 1: 삼촌 노드가 빨간색인 경우
            node->parent->color = RBTREE_BLACK;
            uncle->color = RBTREE_BLACK;
            node->parent->parent->color = RBTREE_RED;
            node = node->parent->parent;
        } else {
            // 케이스 2와 3: 삼촌 노드가 검은색인 경우
            if (node == node->parent->left) {
                node = node->parent;
                right_rotate(t, node);
            }
            node->parent->color = RBTREE_BLACK;
            node->parent->parent->color = RBTREE_RED;
            left_rotate(t, node->parent->parent);
        }
    }
  }
  t->root->color = RBTREE_BLACK;  // 루트 노드는 항상 검은색
}
void left_rotate(rbtree *t, node_t *x) {
    node_t *y;
    y = x->right; // y는 x의 오른쪽 자식입니다.
    
    x->right = y->left; // y의 왼쪽 서브트리를 x의 오른쪽 서브트리로 옮깁니다.
    if (y->left != t->nil) { 
        y->left->parent = x; // y의 왼쪽 자식이 nil이 아니면, 부모를 x로 설정합니다.
    }
    
    y->parent = x->parent; // y의 부모를 x의 부모로 변경합니다.
    if (x->parent == t->nil) { 
        t->root = y; // x가 루트였다면, y를 새로운 루트로 설정합니다.
    } 
    else if (x == x->parent->left) { 
        x->parent->left = y; // x가 왼쪽 자식이었다면, y를 왼쪽 자식으로 설정합니다.
    } 
    else { 
        x->parent->right = y; // x가 오른쪽 자식이었다면, y를 오른쪽 자식으로 설정합니다.
    }
    
    y->left = x; // x를 y의 왼쪽 자식으로 설정합니다.
    x->parent = y; // x의 부모를 y로 설정합니다.
}

void right_rotate(rbtree *t, node_t *x) {
  node_t *y;
  y = x->left;
  x->left = y->right;
  if (y->right != t->nil){
    y->right->parent = x;
  }
  y->parent = x->parent;
  if (x->parent == t->nil){
    t->root = y;
  }
  else if(x == x->parent->left){
    x->parent->left = y;
  }
  else{
    x->parent->right= y;
  }
  y->right=x;
  x->parent = y;
}
node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find
  node_t *current = t->root;
  // nil 노드에 도달할 때까지 순회
  while (current != t->nil){
    // key 찾았을 때
    if (current->key == key){
      return current;
    }
    // key가 현재 노드보다 작으면 왼쪽
    else if (current->key > key){
      current = current->left;
    }
    // key가 현재 노드보다 크면 오른쪽
    else{
      current = current -> right;
    }
  }
  // 키를 찾을 수 없는 경우 NULL 반환
  return NULL;
  
}

node_t *rbtree_min(const rbtree *t) {
    node_t *current = t->root;

    while (current->left != t->nil) {
        current = current->left;
    }
    return current;  // 가장 작은 값이 있는 노드를 반환
}

node_t *rbtree_max(const rbtree *t) {
    node_t *current = t->root;

    while (current->right != t->nil) {
        current = current->right;
    }
    return current;  // 가장 큰 값이 있는 노드를 반환
}

// 노드 바꿔치기
void rbtree_transplant(rbtree *t, node_t *u, node_t *v) {
  if (u->parent == t->nil) {
    t->root = v;
  } else if (u == u->parent->left) {
    u->parent->left = v;
  } else {
    u->parent->right = v;
  }
  v->parent = u->parent;
}

// delete 시 fixup
void delete_fixup(rbtree *t, node_t *x) {
  while (x != t->root && x->color == RBTREE_BLACK) {
    // x가 왼쪽 자식일 때
    if (x == x->parent->left) {
      // w = x의 형제
      node_t *w = x->parent->right;
      // CASE I
      if (w->color == RBTREE_RED) {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        left_rotate(t, x->parent);
        w = x->parent->right;
      }
      // CASE II
      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK) {
        w->color = RBTREE_RED;
        x = x->parent;
      } else {
        // CASE III
        if (w->right->color == RBTREE_BLACK) {
          w->left->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          right_rotate(t, w);
          w = x->parent->right;
        }
        // CASE IV
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->right->color = RBTREE_BLACK;
        left_rotate(t, x->parent);
        x = t->root;
      }
    // x가 오른쪽 자식일 때
    } else {
      node_t *w = x->parent->left;
      // CASE I
      if (w->color == RBTREE_RED) {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(t, x->parent);
        w = x->parent->left;
      }
      // CASE II
      if (w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK) {
        w->color = RBTREE_RED;
        x = x->parent;
      } else {
        // CASE III
        if (w->left->color == RBTREE_BLACK) {
          w->right->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          left_rotate(t, w);
          w = x->parent->left;
        }
        // CASE IV
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        right_rotate(t, x->parent);
        x = t->root;
      }
    }
  }
  x->color = RBTREE_BLACK;
}

node_t *tree_min(const rbtree *t, node_t *n) {
  if (n == NULL) {
    return NULL;
  }
  node_t *x = n;
  while (x->left != t->nil) {
    x = x->left;
  }
  return x;
}


int rbtree_erase(rbtree *t, node_t *z) {
  node_t *x;
  node_t *y = z;
  color_t y_original_color = y->color;
  // 왼쪽 자식노드가 없을 때
  if (z->left == t->nil) {
    x = z->right;
    // z를 오른쪽 자식으로 바꾼다
    rbtree_transplant(t, z, z->right);
  // 오른쪽 자식 노드가 없을 때
  } else if (z->right == t->nil) {
    x = z->left;
    // z를 왼쪽 자식으로 바꾼다
    rbtree_transplant(t, z, z->left);
  } else {
    // y = z의 후손
    y = tree_min(t, z->right);
    y_original_color = y->color;
    x = y->right;
    if (y != z->right) {
      rbtree_transplant(t, y, y->right);
      y->right = z->right;
      y->right->parent = y;
    } else {
      x->parent = y;
    }
    rbtree_transplant(t, z, y);
    y->left = z->left;
    y->left->parent = y;
    y->color = z->color;
  }

  // 삭제한 노드 free 처리
  free(z);

  // red-black 위반 발생 시 fixup
  if (y_original_color == RBTREE_BLACK) {
    delete_fixup(t, x);
  }
  return 0;
}

void inorder(const rbtree *t, node_t* root, key_t **arr) {
  if (root != t->nil) {
    // 왼쪽 서브트리 순회
    inorder(t,root->left, arr);

    // 현재 노드의 키를 배열에 저장하고, 배열 포인터 +1 해서 다음 배열 주소로 넘어가기
    **arr = root->key;
    (*arr)++;

    // 오른쪽 서브트리 순회
    inorder(t,root->right, arr);
  }
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  if (t->root == t->nil){
    return 0;
  }
  // 중위 순회
  inorder(t,t->root, &arr);
  
  return 0;
}

