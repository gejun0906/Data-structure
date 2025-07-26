#define  _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>

// 简单选择排序函数
void selectionSort(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        // 找到未排序部分的最小元素索引
        int min_idx = i;
        for (int j = i + 1; j < n; j++) {
            if (arr[j] < arr[min_idx]) {
                min_idx = j;
            }
        }

        // 将找到的最小元素与未排序部分的第一个元素交换
        if (min_idx != i) {
            int temp = arr[i];
            arr[i] = arr[min_idx];
            arr[min_idx] = temp;
        }
    }
}

// 打印数组函数
void printArray(int arr[], int size) {
    for (int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int main() {
    int arr[] = { 64, 25, 12, 22, 11 };
    int n = sizeof(arr) / sizeof(arr[0]);

    printf("排序前的数组: \n");
    printArray(arr, n);

    selectionSort(arr, n);

    printf("排序后的数组: \n");
    printArray(arr, n);

    return 0;
}