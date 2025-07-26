#define  _CRT_SECURE_NO_WARNINGS 1
//平衡二叉树
#include <stdio.h>
#include <stdlib.h>

// AVL树节点结构
typedef struct AVLNode {
    int key;                // 关键字
    int height;             // 节点高度
    struct AVLNode* left;   // 左子树
    struct AVLNode* right;  // 右子树
} AVLNode;

// 获取节点高度
int height(AVLNode* node) {
    if (node == NULL)
        return 0;
    return node->height;
}

// 获取两个数中的较大值
int max(int a, int b) {
    return (a > b) ? a : b;
}

// 创建新节点
AVLNode* newNode(int key) {
    AVLNode* node = (AVLNode*)malloc(sizeof(AVLNode));
    node->key = key;
    node->left = NULL;
    node->right = NULL;
    node->height = 1;  // 新节点初始高度为1
    return node;
}

// 右旋操作
AVLNode* rightRotate(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;

    // 执行旋转
    x->right = y;
    y->left = T2;

    // 更新高度
    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;

    // 返回新的根节点
    return x;
}

// 左旋操作
AVLNode* leftRotate(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;

    // 执行旋转
    y->left = x;
    x->right = T2;

    // 更新高度
    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;

    // 返回新的根节点
    return y;
}

// 获取平衡因子
int getBalance(AVLNode* node) {
    if (node == NULL)
        return 0;
    return height(node->left) - height(node->right);
}

// 插入节点
AVLNode* insert(AVLNode* node, int key) {
    // 1. 执行标准的BST插入
    if (node == NULL)
        return newNode(key);

    if (key < node->key)
        node->left = insert(node->left, key);
    else if (key > node->key)
        node->right = insert(node->right, key);
    else // 不允许重复键
        return node;

    // 2. 更新节点高度
    node->height = 1 + max(height(node->left), height(node->right));

    // 3. 获取平衡因子检查是否平衡
    int balance = getBalance(node);

    // 如果不平衡，有4种情况

    // 左左情况
    if (balance > 1 && key < node->left->key)
        return rightRotate(node);

    // 右右情况
    if (balance < -1 && key > node->right->key)
        return leftRotate(node);

    // 左右情况
    if (balance > 1 && key > node->left->key) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    // 右左情况
    if (balance < -1 && key < node->right->key) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    // 返回未改变的节点指针
    return node;
}

// 找到最小值的节点
AVLNode* minValueNode(AVLNode* node) {
    AVLNode* current = node;
    while (current->left != NULL)
        current = current->left;
    return current;
}

// 删除节点
AVLNode* deleteNode(AVLNode* root, int key) {
    // 1. 执行标准BST删除
    if (root == NULL)
        return root;

    if (key < root->key)
        root->left = deleteNode(root->left, key);
    else if (key > root->key)
        root->right = deleteNode(root->right, key);
    else {
        // 节点有一个子节点或没有子节点
        if ((root->left == NULL) || (root->right == NULL)) {
            AVLNode* temp = root->left ? root->left : root->right;

            // 没有子节点的情况
            if (temp == NULL) {
                temp = root;
                root = NULL;
            }
            else // 有一个子节点的情况
                *root = *temp; // 复制非空子节点内容

            free(temp);
        }
        else {
            // 节点有两个子节点：获取中序后继(右子树中的最小值)
            AVLNode* temp = minValueNode(root->right);

            // 复制中序后继的数据到当前节点
            root->key = temp->key;

            // 删除中序后继
            root->right = deleteNode(root->right, temp->key);
        }
    }

    // 如果树只有一个节点则返回
    if (root == NULL)
        return root;

    // 2. 更新当前节点的高度
    root->height = 1 + max(height(root->left), height(root->right));

    // 3. 获取平衡因子检查是否平衡
    int balance = getBalance(root);

    // 如果不平衡，有4种情况

    // 左左情况
    if (balance > 1 && getBalance(root->left) >= 0)
        return rightRotate(root);

    // 左右情况
    if (balance > 1 && getBalance(root->left) < 0) {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }

    // 右右情况
    if (balance < -1 && getBalance(root->right) <= 0)
        return leftRotate(root);

    // 右左情况
    if (balance < -1 && getBalance(root->right) > 0) {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    return root;
}

// 前序遍历
void preOrder(AVLNode* root) {
    if (root != NULL) {
        printf("%d ", root->key);
        preOrder(root->left);
        preOrder(root->right);
    }
}

// 中序遍历
void inOrder(AVLNode* root) {
    if (root != NULL) {
        inOrder(root->left);
        printf("%d ", root->key);
        inOrder(root->right);
    }
}

// 释放树的内存
void freeTree(AVLNode* root) {
    if (root != NULL) {
        freeTree(root->left);
        freeTree(root->right);
        free(root);
    }
}

int main() {
    AVLNode* root = NULL;

    // 插入测试
    root = insert(root, 10);
    root = insert(root, 20);
    root = insert(root, 30);
    root = insert(root, 40);
    root = insert(root, 50);
    root = insert(root, 25);

    /* 构造的AVL树
            30
           /  \
         20   40
        /  \    \
       10  25    50
    */

    printf("前序遍历结果: ");
    preOrder(root);
    printf("\n");

    printf("中序遍历结果: ");
    inOrder(root);
    printf("\n");

    // 删除测试
    root = deleteNode(root, 20);

    printf("\n删除20后的前序遍历结果: ");
    preOrder(root);
    printf("\n");

    printf("删除20后的中序遍历结果: ");
    inOrder(root);
    printf("\n");

    // 释放内存
    freeTree(root);

    return 0;
}
