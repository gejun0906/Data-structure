#define  _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#include <stdlib.h>

// 获取数组中最大的数
int getMax(int arr[], int n) {
    int max = arr[0];
    for (int i = 1; i < n; i++)
        if (arr[i] > max)
            max = arr[i];
    return max;
}

// 使用计数排序对指定位数进行排序
void countSort(int arr[], int n, int exp) {
    int output[n]; // 输出数组
    int count[10] = { 0 }; // 计数数组

    // 统计每个数字出现的次数
    for (int i = 0; i < n; i++)
        count[(arr[i] / exp) % 10]++;

    // 计算累计次数
    for (int i = 1; i < 10; i++)
        count[i] += count[i - 1];

    // 构建输出数组
    for (int i = n - 1; i >= 0; i--) {
        output[count[(arr[i] / exp) % 10] - 1] = arr[i];
        count[(arr[i] / exp) % 10]--;
    }

    // 将排序结果复制回原数组
    for (int i = 0; i < n; i++)
        arr[i] = output[i];
}

// 基数排序主函数
void radixSort(int arr[], int n) {
    // 找到最大数，确定需要排序的位数
    int max = getMax(arr, n);

    // 对每一位进行计数排序
    for (int exp = 1; max / exp > 0; exp *= 10)
        countSort(arr, n, exp);
}

// 打印数组
void printArray(int arr[], int n) {
    for (int i = 0; i < n; i++)
        printf("%d ", arr[i]);
    printf("\n");
}

int main() {
    int arr[] = { 170, 45, 75, 90, 802, 24, 2, 66 };
    int n = sizeof(arr) / sizeof(arr[0]);

    printf("原始数组: \n");
    printArray(arr, n);

    radixSort(arr, n);

    printf("排序后数组: \n");
    printArray(arr, n);

    return 0;
}