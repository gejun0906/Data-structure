#define  _CRT_SECURE_NO_WARNINGS 1
//二叉排序树
#include <stdio.h>
#include <stdlib.h>

// 二叉排序树结点结构
typedef struct BSTNode {
    int data;               // 数据域
    struct BSTNode* lchild; // 左孩子指针
    struct BSTNode* rchild; // 右孩子指针
} BSTNode, * BSTree;

// 创建新结点
BSTNode* createNode(int data) {
    BSTNode* node = (BSTNode*)malloc(sizeof(BSTNode));
    node->data = data;
    node->lchild = node->rchild = NULL;
    return node;
}

// 插入结点(递归实现)
int BST_Insert(BSTree* T, int key) {
    if (*T == NULL) { // 树为空
        *T = createNode(key);
        return 1; // 插入成功
    }
    else if (key == (*T)->data) {
        return 0; // 树中已存在相同关键字，插入失败
    }
    else if (key < (*T)->data) {
        return BST_Insert(&(*T)->lchild, key); // 插入到左子树
    }
    else {
        return BST_Insert(&(*T)->rchild, key); // 插入到右子树
    }
}

// 插入结点(非递归实现)
int BST_Insert_NonRecur(BSTree* T, int key) {
    BSTNode* p = *T;
    BSTNode* parent = NULL;

    // 查找插入位置
    while (p != NULL) {
        parent = p;
        if (key == p->data) {
            return 0; // 已存在，插入失败
        }
        else if (key < p->data) {
            p = p->lchild;
        }
        else {
            p = p->rchild;
        }
    }

    // 创建新结点
    BSTNode* newNode = createNode(key);

    // 插入到树中
    if (parent == NULL) { // 树为空
        *T = newNode;
    }
    else if (key < parent->data) {
        parent->lchild = newNode;
    }
    else {
        parent->rchild = newNode;
    }

    return 1; // 插入成功
}

// 创建二叉排序树
void createBST(BSTree* T, int arr[], int n) {
    *T = NULL;
    for (int i = 0; i < n; i++) {
        BST_Insert(T, arr[i]);
        // 也可以使用非递归插入
        // BST_Insert_NonRecur(T, arr[i]);
    }
}

// 中序遍历(递归实现)
void InOrderTraverse(BSTree T) {
    if (T != NULL) {
        InOrderTraverse(T->lchild);
        printf("%d ", T->data);
        InOrderTraverse(T->rchild);
    }
}

// 查找结点(递归实现)
BSTNode* BST_Search(BSTree T, int key) {
    if (T == NULL || T->data == key) {
        return T;
    }
    else if (key < T->data) {
        return BST_Search(T->lchild, key);
    }
    else {
        return BST_Search(T->rchild, key);
    }
}

// 查找结点(非递归实现)
BSTNode* BST_Search_NonRecur(BSTree T, int key) {
    BSTNode* p = T;
    while (p != NULL && p->data != key) {
        if (key < p->data) {
            p = p->lchild;
        }
        else {
            p = p->rchild;
        }
    }
    return p;
}

// 删除结点
int BST_Delete(BSTree* T, int key) {
    if (*T == NULL) {
        return 0; // 删除失败，树为空
    }

    BSTNode* p = *T;
    BSTNode* parent = NULL;

    // 查找要删除的结点
    while (p != NULL && p->data != key) {
        parent = p;
        if (key < p->data) {
            p = p->lchild;
        }
        else {
            p = p->rchild;
        }
    }

    if (p == NULL) {
        return 0; // 未找到要删除的结点
    }

    // 情况1：要删除的结点是叶子结点
    if (p->lchild == NULL && p->rchild == NULL) {
        if (parent == NULL) { // 删除的是根结点
            *T = NULL;
        }
        else if (parent->lchild == p) {
            parent->lchild = NULL;
        }
        else {
            parent->rchild = NULL;
        }
        free(p);
    }
    // 情况2：要删除的结点只有左子树
    else if (p->rchild == NULL) {
        if (parent == NULL) { // 删除的是根结点
            *T = p->lchild;
        }
        else if (parent->lchild == p) {
            parent->lchild = p->lchild;
        }
        else {
            parent->rchild = p->lchild;
        }
        free(p);
    }
    // 情况3：要删除的结点只有右子树
    else if (p->lchild == NULL) {
        if (parent == NULL) { // 删除的是根结点
            *T = p->rchild;
        }
        else if (parent->lchild == p) {
            parent->lchild = p->rchild;
        }
        else {
            parent->rchild = p->rchild;
        }
        free(p);
    }
    // 情况4：要删除的结点有左右子树
    else {
        // 找到p的直接前驱(左子树的最右结点)
        BSTNode* pre = p->lchild;
        BSTNode* preParent = p;

        while (pre->rchild != NULL) {
            preParent = pre;
            pre = pre->rchild;
        }

        // 用前驱结点的值替换要删除结点的值
        p->data = pre->data;

        // 删除前驱结点
        if (preParent == p) {
            preParent->lchild = pre->lchild;
        }
        else {
            preParent->rchild = pre->lchild;
        }
        free(pre);
    }

    return 1; // 删除成功
}

// 计算树的高度
int BST_Height(BSTree T) {
    if (T == NULL) {
        return 0;
    }
    else {
        int leftHeight = BST_Height(T->lchild);
        int rightHeight = BST_Height(T->rchild);
        return (leftHeight > rightHeight ? leftHeight : rightHeight) + 1;
    }
}

// 销毁二叉排序树
void destroyBST(BSTree* T) {
    if (*T != NULL) {
        destroyBST(&(*T)->lchild);
        destroyBST(&(*T)->rchild);
        free(*T);
        *T = NULL;
    }
}

int main() {
    BSTree T = NULL;
    int arr[] = { 50, 30, 70, 20, 40, 60, 80 };
    int n = sizeof(arr) / sizeof(arr[0]);

    // 创建二叉排序树
    createBST(&T, arr, n);

    // 中序遍历(结果应为有序序列)
    printf("中序遍历结果: ");
    InOrderTraverse(T);
    printf("\n");

    // 查找结点
    int searchKey = 40;
    BSTNode* result = BST_Search(T, searchKey);
    if (result != NULL) {
        printf("找到结点 %d\n", searchKey);
    }
    else {
        printf("未找到结点 %d\n", searchKey);
    }

    // 计算树的高度
    printf("树的高度: %d\n", BST_Height(T));

    // 删除结点
    int deleteKey = 30;
    if (BST_Delete(&T, deleteKey)) {
        printf("成功删除结点 %d\n", deleteKey);
        printf("删除后的中序遍历结果: ");
        InOrderTraverse(T);
        printf("\n");
    }
    else {
        printf("删除结点 %d 失败\n", deleteKey);
    }

    // 销毁树
    destroyBST(&T);

    return 0;
}
int main() {
    BSTree T = NULL;
    int arr[] = { 50, 30, 70, 20, 40, 60, 80 };
    int n = sizeof(arr) / sizeof(arr[0]);

    // 创建二叉排序树
    createBST(&T, arr, n);

    // 中序遍历(结果应为有序序列)
    printf("中序遍历结果: ");
    InOrderTraverse(T);
    printf("\n");

    // 查找结点
    int searchKey = 40;
    BSTNode* result = BST_Search(T, searchKey);
    if (result != NULL) {
        printf("找到结点 %d\n", searchKey);
    }
    else {
        printf("未找到结点 %d\n", searchKey);
    }

    // 删除结点
    int deleteKey = 30;
    if (BST_Delete(&T, deleteKey)) {
        printf("成功删除结点 %d\n", deleteKey);
        printf("删除后的中序遍历结果: ");
        InOrderTraverse(T);
        printf("\n");
    }

    // 销毁树
    destroyBST(&T);

    return 0;
}