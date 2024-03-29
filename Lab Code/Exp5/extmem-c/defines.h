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
#define TUPLIE_LENGTH 8                 /// Ԫ��ռ�ַ�����
#define ELEMENT_LENGTH 4                /// Ԫ��Ԫ��ռ�ַ�����
#define TUPLES_PER_BLOCK 7              /// ÿ��BLOCK���е�TUPLE��
#define BLOCK_R_LENGTH 16               /// Rռ�Ŀ���
#define BLOCK_S_LENGTH 32               /// Sռ�Ŀ���
#define SRC_R_INIT_ADDR 1               /// ԴR����BLOCK��ַ
#define SRC_S_INIT_ADDR 17              /// ԴS����BLOCK��ַ
#define SORTED_R_INIT_ADDR 301          /// ������R����BLOCK��ַ
#define SORTED_S_INIT_ADDR 317          /// ������R����BLOCK��ַ
#define INDEX_INIT_ADDR 350             /// ��������BLOCK��ַ
#define LINEAR_SEARCH_INIT_ADDR 100     /// �������������BLOCK��ַ
#define INDEX_SEARCH_INIT_ADDR 120      /// �����������������BLOCK��ַ
#define SORTED_INNERJOIN_INIT_ADDR 400  /// ������������ӽ����BLOCK��ַ
#define SORTED_INTERSECT_INIT_ADDR 500  /// �������������ɨ�轻�����BLOCK��ַ
#define SORTED_UNION_INIT_ADDR 600      /// �������������ɨ�貢�����BLOCK��ַ
#define SORTED_DIFFERENCE_INIT_ADDR 700 /// �������������ɨ�������BLOCK��ַ
#define UPPER_LIMIT 999                 /// ֵ���Ͻ�

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
