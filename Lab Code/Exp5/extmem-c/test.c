#include "defines.h"

int main(void) {

    printTitle("��������������ѡ���㷨 S.C=107");
    linearSearch();

    printTitle("���׶ζ�·�鲢�����㷨");
    mergeSort(1);
    mergeSort(2);

    printTitle("���������Ĺ�ϵѡ���㷨");
    constructIndex();
    traverseIndex();

    printTitle("������������Ӳ����㷨");
    sortedInnerjoin();

    printTitle("��������ļ��Ͻ������㷨");
    intersectSchema();

    printTitle("��������ļ��ϲ������㷨");
    unionSchema();

    printTitle("��������ļ��ϲ�����㷨");
    differenceSchema();

    printf("\n5��ʵ������(���������������㷨)����ɣ����������ַ��˳���");
    getchar();
	return 0;
}






