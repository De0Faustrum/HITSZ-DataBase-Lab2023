#include "defines.h"

void extractTuple(char* bufferedBlock, Tuple* bufferedTuple) {
    for (int i=0; i<TUPLES_PER_BLOCK; i++) {
        char temp[ELEMENT_LENGTH+1];
        memcpy(temp, bufferedBlock+i*TUPLIE_LENGTH, ELEMENT_LENGTH);
        bufferedTuple[i].x = atoi(temp);
        memcpy(temp, bufferedBlock+i*TUPLIE_LENGTH+ELEMENT_LENGTH, ELEMENT_LENGTH);
        bufferedTuple[i].y = atoi(temp);
    }
}


bool isMerged(int* order, int remnantBlock) {
    for(int i = 0; i < remnantBlock; i++) {
        if(order[i] < TUPLES_PER_BLOCK) {
            return false;
        }
    }
    return true;
}


void printTitle(char* title) {
    printf("\n------------------------------\n");
    setColor(RED);
    printf("%s\n", title);
    setColor(WHITE);
    printf("------------------------------\n");
}

void setColor(const unsigned short textColor){
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), textColor);
}
