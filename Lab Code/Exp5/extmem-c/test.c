#include "defines.h"

int main(void) {

    printTitle("基于线性搜索的选择算法 S.C=107");
    linearSearch();

    printTitle("两阶段多路归并排序算法");
    mergeSort(1);
    mergeSort(2);

    printTitle("基于索引的关系选择算法");
    constructIndex();
    traverseIndex();

    printTitle("基于排序的连接操作算法");
    sortedInnerjoin();

    printTitle("基于排序的集合交操作算法");
    intersectSchema();

    printTitle("基于排序的集合并操作算法");
    unionSchema();

    printTitle("基于排序的集合差操作算法");
    differenceSchema();

    printf("\n5个实验任务(包括附加题三种算法)已完成，输入任意字符退出：");
    getchar();
	return 0;
}






