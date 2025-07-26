#define  _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>

// 希尔排序函数
void shellSort(int arr[], int n) {
    // 初始间隔gap设为数组长度的一半，然后逐步缩小
    for (int gap = n / 2; gap > 0; gap /= 2) {
        // 对每个间隔进行插入排序
        for (int i = gap; i < n; i++) {
            int temp = arr[i];
            int j;
            // 对间隔为gap的元素进行插入排序
            for (j = i; j >= gap && arr[j - gap] > temp; j -= gap) {
                arr[j] = arr[j - gap];
            }
            arr[j] = temp;
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
    int arr[] = { 12, 34, 54, 2, 3 };
    int n = sizeof(arr) / sizeof(arr[0]);

    printf("排序前的数组: \n");
    printArray(arr, n);

    shellSort(arr, n);

    printf("排序后的数组: \n");
    printArray(arr, n);

    return 0;
}