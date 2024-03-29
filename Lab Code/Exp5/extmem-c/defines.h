#ifndef DEFINES_H_INCLUDED
#define DEFINES_H_INCLUDED

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include "extmem.h"

#define RED 4
#define WHITE 7
#define TUPLIE_LENGTH 8                 /// 元组占字符长度
#define ELEMENT_LENGTH 4                /// 元组元素占字符长度
#define TUPLES_PER_BLOCK 7              /// 每个BLOCK含有的TUPLE数
#define BLOCK_R_LENGTH 16               /// R占的块数
#define BLOCK_S_LENGTH 32               /// S占的块数
#define SRC_R_INIT_ADDR 1               /// 源R的首BLOCK地址
#define SRC_S_INIT_ADDR 17              /// 源S的首BLOCK地址
#define SORTED_R_INIT_ADDR 301          /// 排序后的R的首BLOCK地址
#define SORTED_S_INIT_ADDR 317          /// 排序后的R的首BLOCK地址
#define INDEX_INIT_ADDR 350             /// 索引的首BLOCK地址
#define LINEAR_SEARCH_INIT_ADDR 100     /// 线性搜索结果首BLOCK地址
#define INDEX_SEARCH_INIT_ADDR 120      /// 基于索引搜索结果首BLOCK地址
#define SORTED_INNERJOIN_INIT_ADDR 400  /// 基于排序的连接结果首BLOCK地址
#define SORTED_INTERSECT_INIT_ADDR 500  /// 基于排序的两趟扫描交结果首BLOCK地址
#define SORTED_UNION_INIT_ADDR 600      /// 基于排序的两趟扫描并结果首BLOCK地址
#define SORTED_DIFFERENCE_INIT_ADDR 700 /// 基于排序的两趟扫描差结果首BLOCK地址
#define UPPER_LIMIT 999                 /// 值域上界

typedef struct tuple { int x; int y; } Tuple;

void linearSearch();
void mergeSort(int mode);
void constructIndex();
void traverseIndex();
void sortedInnerjoin();
void intersectSchema();
void unionSchema();
void differenceSchema();

bool isMerged(int idx[], int num);
void extractTuple(char* bufferedBlock, Tuple* bufferedTuple);
void printTitle(char* title);
void setColor(const unsigned short textColor);

#endif // DEFINES_H_INCLUDED
