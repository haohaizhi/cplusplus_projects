# Array

简单的整数数组。具有I/O功能，排序功能和搜索功能。

## Sort Function

Sort函数按特定顺序对范围内的元素进行排序。不同类型的排序方法有冒泡排序、选择排序、合并排序和快速排序。冒泡排序重复排序相邻的元素，如果他们在错误的顺序。

## Structure

```C
typedef struct CArray {
		int *array;
		int size;
	} CArray;
```

## Files

* CArray.c - 数组实现
* CArray.h - 头文件
* CArrayTests.c - 示例
