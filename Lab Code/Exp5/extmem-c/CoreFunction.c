#include "defines.h"

static unsigned char* emptyString = "                                                                ";
static int indexLength = 0;


void linearSearch() {
    Buffer buf;
    if (!initBuffer(520, 64, &buf)) {   //��ʼ��������
        perror("Buffer Initialization Failed!\n");
        return;
    }
    char temp[5];
    int foundCount = 0;     //����ѡ��������Ԫ����
    int writeCount = 0;     //��д���Block��
    int bufferedReaderAddress = SRC_S_INIT_ADDR;    //S�׵�ַ
    int bufferedWriterAddress = LINEAR_SEARCH_INIT_ADDR;    //����׵�ַ
    unsigned char* bufferedReader = getNewBlockInBuffer(&buf);  //������
    unsigned char* bufferedWriter = getNewBlockInBuffer(&buf);  //д����

    while(TRUE) {           //��ʼ�������е�Block

        bufferedReader = readBlockFromDisk(bufferedReaderAddress, &buf);
        printf("�������ݿ�%d\n", bufferedReaderAddress);

        for (int i=0; i<TUPLES_PER_BLOCK; i++) {

            Tuple bufferedTuple;                            //�Ӷ����������Ԫ��,��char*ת��Ϊint��
            memcpy(temp, bufferedReader+i*TUPLIE_LENGTH, ELEMENT_LENGTH);
            bufferedTuple.x = atoi(temp);
            memcpy(temp, bufferedReader+i*TUPLIE_LENGTH+ELEMENT_LENGTH, ELEMENT_LENGTH);
            bufferedTuple.y = atoi(temp);

            if (bufferedTuple.x == 107) {               //ѡ��������S.C=107
                printf("(X=%d, Y=%d)\n", bufferedTuple.x, bufferedTuple.y);     //��ӡ����ѡ��������Ԫ��
                memcpy(bufferedWriter+foundCount*TUPLIE_LENGTH, bufferedReader+i*TUPLIE_LENGTH, TUPLIE_LENGTH);     //������������Ԫ����ӵ�д������
                foundCount++;

                if (foundCount == TUPLES_PER_BLOCK) {   //�˿���д7��Ԫ��?
                    itoa(bufferedWriterAddress+writeCount+1, bufferedWriter+TUPLES_PER_BLOCK*TUPLIE_LENGTH, 10);    //���¸�block�ĵ�ַ��ӵ�д������
                    if (writeBlockToDisk(bufferedWriter, bufferedWriterAddress + writeCount, &buf) != 0) {      //���������ڵ�����д�����block
                        perror("Writing Block Failed!\n");
                        return;
                    }
                    writeCount++;
                    foundCount = 0;                     //���ÿ���Ԫ�������
                    freeBlockInBuffer(bufferedWriter, &buf);
                    bufferedWriter = getNewBlockInBuffer(&buf);
                }
            }
        }
        memcpy(temp, bufferedReader+TUPLES_PER_BLOCK*TUPLIE_LENGTH, ELEMENT_LENGTH);        //��ȡblockĩβ�����ݱ�ʶ�¸�block��ַ
        bufferedReaderAddress = atoi(temp);
        if (bufferedReaderAddress >= SRC_S_INIT_ADDR+BLOCK_S_LENGTH) {          //������ַ�ѹ�48�����ֹ
            break;
        }
        freeBlockInBuffer(bufferedReader, &buf);
    }

    if (foundCount) {   //��д�������ڵ�ʣ������д�����block
        memcpy(bufferedWriter+foundCount*TUPLIE_LENGTH, emptyString, (TUPLES_PER_BLOCK-foundCount)*TUPLIE_LENGTH);
        itoa(bufferedWriterAddress+writeCount+1, bufferedWriter+TUPLES_PER_BLOCK*TUPLIE_LENGTH, 10);    //���¸�block�ĵ�ַ��ӵ�д������
        if (writeBlockToDisk(bufferedWriter, bufferedWriterAddress+writeCount, &buf) != 0) {    //д����
            perror("Writing Block Failed!\n");
            return;
        }
        writeCount++;
    }
    for (int i=0; i<writeCount; i++) {
        printf("ע�����д�����:%d\n", bufferedWriterAddress + i);
    }
    setColor(RED);
    printf("����ѡ��������Ԫ��һ��:%d��\n", (writeCount-1)*TUPLES_PER_BLOCK + foundCount);
    printf("IO��дһ��%d��\n", buf.numIO);
    setColor(WHITE);
    freeBuffer(&buf);
    return;
}


void mergeSort(int mode) {
    Buffer buf;
    if (!initBuffer(520, 64, &buf)) {
        perror("Buffer Initialization Failed!\n");
        return;
    }
    int initReadAddress, initWriteAddress, readAdrress, writeAddress, termReadAddress, remnantBlock;
    if(mode == 1) {
        readAdrress = initReadAddress = SRC_R_INIT_ADDR;
        termReadAddress = SRC_R_INIT_ADDR+BLOCK_R_LENGTH-1;
        writeAddress = initWriteAddress = SORTED_R_INIT_ADDR;
    } else if(mode == 2) {
        readAdrress = initReadAddress = SRC_S_INIT_ADDR;
        termReadAddress = SRC_S_INIT_ADDR+BLOCK_S_LENGTH-1;
        writeAddress = initWriteAddress = SORTED_S_INIT_ADDR;
    }
    int temp, order;
    Tuple bufferedTuple[TUPLES_PER_BLOCK];
    char tempConvert[TUPLIE_LENGTH+1];
    unsigned char* bufferedBlock[6] = {NULL}; //���ڻ������������ƣ�ÿ�ζ���6��block��������
    unsigned char* bufferedWriter = NULL;     //д������
    int writeOrder = 0;      //��¼��д����д���ַ�±꣬Ϊ7ʱ��Ҫд�����

    while (TRUE) {
        bufferedWriter = getNewBlockInBuffer(&buf);
        remnantBlock = termReadAddress-readAdrress;     //ʣ��δ��������Ŀ���
        Tuple tempTuple;
        if((++remnantBlock) >= 6) {
            remnantBlock = 6;                   //����6����ѡ��ǰ6���������
        }
        for(int i=0; i<remnantBlock; i++) {     //����block�������ڲ�����
            if ((bufferedBlock[i] = readBlockFromDisk(readAdrress+i, &buf)) == NULL) {
                perror("Reading Block Failed!\n");
                return;
            }
            extractTuple(bufferedBlock[i], bufferedTuple);
            for(int j=0; j<TUPLES_PER_BLOCK; j++) {       //�����򣬲���ѡ������
                temp = bufferedTuple[j].x;                 //ÿ��ѡ��Tuple��Xֵ��С��Ԫ��������ǰ
                order = j;
                for (int k=j; k<TUPLES_PER_BLOCK; k++) {
                    if (bufferedTuple[k].x < temp) {
                        temp = bufferedTuple[k].x;
                        order = k;
                    }
                }
                tempTuple = bufferedTuple[j];
                bufferedTuple[j] = bufferedTuple[order];
                bufferedTuple[order] = tempTuple;
                itoa(bufferedTuple[j].x, tempConvert, 10);      //�������д��block
                memcpy(bufferedBlock[i]+j*TUPLIE_LENGTH, tempConvert, ELEMENT_LENGTH);
                itoa(bufferedTuple[j].y, tempConvert, 10);
                memcpy(bufferedBlock[i]+j*TUPLIE_LENGTH+ELEMENT_LENGTH, tempConvert, ELEMENT_LENGTH);
            }
        }


        int indexVector[6] = {0};        //��1�ζ�·�鲢��indexVector��ÿ��ά��ʶһ��block�й鲢���ȵ��±�
        while (!isMerged(indexVector, remnantBlock)) {
            Tuple element[6];            //�ݴ�ÿ·�еļ�Сֵ
            for (int i = 0; i < remnantBlock; i++) {
                if (indexVector[i] < TUPLES_PER_BLOCK) {
                    memcpy(tempConvert, bufferedBlock[i]+indexVector[i]*TUPLIE_LENGTH, ELEMENT_LENGTH);
                    element[i].x = atoi(tempConvert);
                    memcpy(tempConvert, bufferedBlock[i]+indexVector[i]*TUPLIE_LENGTH+ELEMENT_LENGTH, ELEMENT_LENGTH);
                    element[i].y = atoi(tempConvert);
                } else{
                    element[i].x = UPPER_LIMIT;     //��·�ѹ鲢��ϣ�����·��S.C��Ϊ�Ͻ磬ȷ���Ժ󲻻���ѡ���·
                }
            }
            temp = UPPER_LIMIT;
            for(int i=0; i<remnantBlock; i++) {
                if (element[i].x < temp) {
                    temp = element[i].x;
                    order = i;                              //ѡ��6·�м�Сֵ���±�
                }
            }
            itoa(element[order].x, tempConvert, 10);        //����Сֵ�����ܼ�
            memcpy(bufferedWriter+writeOrder*TUPLIE_LENGTH, tempConvert, ELEMENT_LENGTH);
            itoa(element[order].y, tempConvert, 10);
            memcpy(bufferedWriter+(writeOrder++)*TUPLIE_LENGTH+ELEMENT_LENGTH, tempConvert, ELEMENT_LENGTH);
            if(writeOrder % TUPLES_PER_BLOCK == 0) {
                itoa(writeAddress+1, bufferedWriter+TUPLES_PER_BLOCK*TUPLIE_LENGTH, 10);
                if (writeBlockToDisk(bufferedWriter, writeAddress, &buf) != 0) {
                    perror("Writing Block Failed!\n");
                    return;
                }
                writeOrder = 0;
                writeAddress++;
                bufferedWriter = getNewBlockInBuffer(&buf);
            }
            if(indexVector[order] < TUPLES_PER_BLOCK){      //���˿黹δ�鲢���
                indexVector[order]++;                       //�˿�Ĺ鲢�����±�����
            }
        }
        readAdrress += remnantBlock;
        freeBlockInBuffer(bufferedWriter, &buf);
        for (int i = 0; i < remnantBlock; i++) {
            freeBlockInBuffer(bufferedBlock[i], &buf);
        }
        if (readAdrress == termReadAddress + 1) {           //�����п춼�ѹ鲢����������ѭ��
            break;
        }
    }

    bufferedWriter = getNewBlockInBuffer(&buf);     // ��2�ζ�·�鲢
    int segmentRange = termReadAddress-initReadAddress+1;
    int segmentCount = segmentRange/6;    //������
    int fragmentCount = 0;                //�ڶ��ι鲢�У�һ��segement����<=6��fragmemt
    if(segmentRange%6){                   //fragmentCount��ʶ���һ��segment��fragment������
        fragmentCount = segmentRange % 6; //fragment��block
        segmentCount++;
    }
    int fragmentIndexVector[TUPLES_PER_BLOCK]={0};
    int segmentIndexVector[6]={0};
    for(int i=0; i<segmentCount; i++) {
        if((bufferedBlock[i] = readBlockFromDisk(initWriteAddress+i*6, &buf)) == NULL) {
            perror("Reading Block Failed!\n");
            return;
        }
    }

    while(TRUE) {
        for(int i=0; i<segmentCount; i++) {     //����ÿ��segment��ʣ��block�ĵ�һ��Ԫ��
            if(fragmentIndexVector[i] < TUPLES_PER_BLOCK) {
                memcpy(tempConvert, bufferedBlock[i]+fragmentIndexVector[i]*TUPLIE_LENGTH, ELEMENT_LENGTH);
                bufferedTuple[i].x = atoi(tempConvert);
                memcpy(tempConvert, bufferedBlock[i]+fragmentIndexVector[i]*TUPLIE_LENGTH+ELEMENT_LENGTH, ELEMENT_LENGTH);
                bufferedTuple[i].y = atoi(tempConvert);
            } else {
                bufferedTuple[i].x = UPPER_LIMIT;
            }
        }
        temp = UPPER_LIMIT;
        for(int i=0; i<segmentCount; i++) {
            if(bufferedTuple[i].x < temp) {
                temp = bufferedTuple[i].x;
                order = i;
            }
        }
        itoa(bufferedTuple[order].x, tempConvert, 10);      //ѡ����СԪ��д��д������
        memcpy(bufferedWriter+writeOrder*TUPLIE_LENGTH, tempConvert, ELEMENT_LENGTH);
        itoa(bufferedTuple[order].y, tempConvert, 10);
        memcpy(bufferedWriter+writeOrder*TUPLIE_LENGTH+ELEMENT_LENGTH, tempConvert, ELEMENT_LENGTH);
        if ((++writeOrder) == TUPLES_PER_BLOCK) {
            itoa(writeAddress+1, bufferedWriter+TUPLES_PER_BLOCK*TUPLIE_LENGTH, 10);
            if (writeBlockToDisk(bufferedWriter, (writeAddress++), &buf) != 0) {
                perror("Writing Block Failed!\n");
                return;
            }
            writeOrder = 0;
            bufferedWriter = getNewBlockInBuffer(&buf);
        }
        if(fragmentIndexVector[order] == 6) {
            if(order == segmentCount-1) {
                if(segmentIndexVector[order] == fragmentCount-1){
                    fragmentIndexVector[order] = 7;
                } else {
                    segmentIndexVector[order]++;
                    fragmentIndexVector[order] = 0;
                    freeBlockInBuffer(bufferedBlock[order], &buf);
                    bufferedBlock[order] = readBlockFromDisk(initWriteAddress+order*6+segmentIndexVector[order], &buf);
                }
            } else {
                if(segmentIndexVector[order] == 5){
                    fragmentIndexVector[order] = 7;
                } else {
                    segmentIndexVector[order]++;
                    fragmentIndexVector[order] = 0;
                    freeBlockInBuffer(bufferedBlock[order], &buf);
                    bufferedBlock[order] = readBlockFromDisk(initWriteAddress+order*6+segmentIndexVector[order], &buf);
                }
            }
        } else {
            fragmentIndexVector[order]++;
        }
        if(isMerged(fragmentIndexVector, segmentCount)){
            break;
        }
    }

    freeBlockInBuffer(bufferedWriter, &buf);
    for(int i = 0; i < segmentCount; i++){
        freeBlockInBuffer(bufferedBlock[i], &buf);
    }
    int bufferedBlockCount = termReadAddress-initReadAddress+1;        //�����ݴ����block
    int currentBlockAddress = writeAddress-bufferedBlockCount;
    for(int i=0; i<bufferedBlockCount; i++) {
        bufferedWriter = readBlockFromDisk(currentBlockAddress+i, &buf);
        itoa(initWriteAddress+i+1, bufferedWriter+TUPLES_PER_BLOCK*TUPLIE_LENGTH, 10);
        if (writeBlockToDisk(bufferedWriter, initWriteAddress+i, &buf) != 0) {
            perror("Writing Block Failed!\n");
            return;
        }
        dropBlockOnDisk(currentBlockAddress + i);
    }
    if(mode == 1) {
        printf("��R�Ķ�·�鲢������ɣ�IO��д%d��\n", buf.numIO);
    } else if(mode == 2) {
        printf("��S�Ķ�·�鲢������ɣ�IO��д%d��\n", buf.numIO);
    }
    freeBuffer(&buf);
    return;
}


void constructIndex() {
    printf("��ʼ����������\n");
    Buffer buf;
    if(!initBuffer(520, 64, &buf)) {
        perror("Buffer Initialization Failed!\n");
        return;
    }
    int writeCount = 0;
    int writeAddress = INDEX_INIT_ADDR;
    char *bufferedReader, *bufferedWriter;
    bufferedWriter = getNewBlockInBuffer(&buf);

    for(int readAddress=SORTED_S_INIT_ADDR; readAddress<SORTED_S_INIT_ADDR+BLOCK_S_LENGTH; readAddress++) {
        bufferedReader = readBlockFromDisk(readAddress, &buf);
        Tuple bufferedTuple[TUPLES_PER_BLOCK];
        extractTuple(bufferedReader, bufferedTuple);
        int currentKey = bufferedTuple[0].x;
        int nextKey = currentKey;
        for(int i=1; i<TUPLES_PER_BLOCK; i++) {
            if((nextKey=bufferedTuple[i].x) != currentKey) {            //�ҵ�����Ԫ����������
                int indexValue = (readAddress-SORTED_S_INIT_ADDR)*TUPLES_PER_BLOCK+i;   //����ֵ
                char temp[ELEMENT_LENGTH+1];
                itoa(bufferedTuple[i].x, temp, 10);
                memcpy(bufferedWriter+writeCount*TUPLIE_LENGTH, temp, ELEMENT_LENGTH);
                sprintf(temp, "%-4d", indexValue);
                memcpy(bufferedWriter+writeCount*TUPLIE_LENGTH+ELEMENT_LENGTH, temp, ELEMENT_LENGTH);
                if ((++writeCount) == 7) {
                    itoa(writeAddress+1, bufferedWriter+TUPLES_PER_BLOCK*TUPLIE_LENGTH, 10);
                    printf("����д����̣�%d\n", writeAddress);
                    if (writeBlockToDisk(bufferedWriter, writeAddress++, &buf) != 0) {
                        perror("Writing Block Failed!\n");
                        return;
                    }
                    indexLength++;
                    writeCount = 0;
                    bufferedWriter = getNewBlockInBuffer(&buf);
                }
                freeBlockInBuffer(bufferedReader, &buf);
                break;
            }
            currentKey = nextKey;
            if (i == TUPLES_PER_BLOCK-1) {
                freeBlockInBuffer(bufferedReader, &buf);
            }
        }
    }

    if (writeCount) {
        memset(bufferedWriter+writeCount*TUPLIE_LENGTH, 0, 64-writeCount*TUPLIE_LENGTH);
        itoa(writeAddress+1, bufferedWriter+TUPLES_PER_BLOCK*TUPLIE_LENGTH, 10);
        printf("����д����̣�%d\n", writeAddress);
        if (writeBlockToDisk(bufferedWriter, writeAddress++, &buf) != 0) {
            perror("Writing Block Failed!\n");
            return;
        }
        indexLength++;
    }
    printf("����������ɣ���������Ϊ%d��Block��IO����Ϊ��%d\n", indexLength, buf.numIO);
}


void traverseIndex() {
    printf("�����������в��ң�\n");
    int validTuples = 0;
    Buffer buf;
    if (!initBuffer(520, 64, &buf)) {
        perror("Buffer Initialization Failed!\n");
        return;
    }
    char *bufferedReader, *bufferedWriter;
    int initIndex, termIndex;
    int dataAddrOffset = SORTED_S_INIT_ADDR*TUPLES_PER_BLOCK;
    bool isIndexFound = false;

    for(int readAddress = INDEX_INIT_ADDR; readAddress < INDEX_INIT_ADDR+indexLength; readAddress++) {    //����������
        printf("����������%d\n", readAddress);
        bufferedReader = readBlockFromDisk(readAddress, &buf);
        Tuple bufferedTuple[TUPLES_PER_BLOCK];
        extractTuple(bufferedReader, bufferedTuple);
        for(int i=0; i<TUPLES_PER_BLOCK-1; i++) {
            if (i == 0 && bufferedTuple[i].x > 107) {   //ÿ��������ĵ�һ�����������ϸ�������һ���������Ա�
                if (readAddress == INDEX_INIT_ADDR) {   //��һ�������������ǰ��Ա�
                    initIndex = dataAddrOffset;
                    termIndex = bufferedTuple[i].y+dataAddrOffset;
                    break;
                } else {                                //�����ϸ���
                    char* tempReader = readBlockFromDisk(readAddress-1, &buf);
                    Tuple tempTuple[TUPLES_PER_BLOCK];
                    extractTuple(tempReader, tempTuple);
                    if (tempTuple[TUPLES_PER_BLOCK-1].x <= 107) {
                        initIndex = tempTuple[TUPLES_PER_BLOCK-1].y+dataAddrOffset;
                        termIndex = bufferedTuple[i].y+dataAddrOffset;
                        freeBlockInBuffer(tempReader, &buf);
                        break;
                    }
                }
            }
            if (bufferedTuple[i].x <= 107 && bufferedTuple[i + 1].x > 107) {    //���S.C=107������������
                initIndex = bufferedTuple[i].y + dataAddrOffset;
                termIndex = bufferedTuple[i + 1].y + dataAddrOffset;
                printf("���ҵ�����\n");
                isIndexFound = true;
                break;
            }
        }
        freeBlockInBuffer(bufferedReader, &buf);
        if(isIndexFound){
            break;
        }
    }
    if (initIndex == termIndex) {
        printf("Can not find by index!\n");
        return;
    }

    int initReadAddress = initIndex / TUPLES_PER_BLOCK;
    int initReadIndex = initIndex % TUPLES_PER_BLOCK;
    int termReadAddress = termIndex / 7;
    int termReadIndex = termIndex % 7;
    bufferedWriter = getNewBlockInBuffer(&buf);
    int writeCount = 0;
    int writeAddress = INDEX_SEARCH_INIT_ADDR;

    for (int readAddress = initReadAddress; readAddress <= termReadAddress; readAddress++) {
        Tuple bufferedTuple[7];         //��������ֵ��ȡ�������ݿ�
        printf("�������ݿ�%d\n", readAddress);
        bufferedReader = readBlockFromDisk(readAddress, &buf);
        extractTuple(bufferedReader, bufferedTuple);
        int currentIndex = 0, termIndex = 7;
        if(readAddress == initReadAddress){
            currentIndex = initReadIndex;
        }
        if(readAddress == termReadAddress) {
            termIndex = termReadIndex;
        }
        while(currentIndex < termIndex) {
            if (bufferedTuple[currentIndex].x == 107) {
                validTuples++;
                printf("(X=%d, Y=%d)\n", bufferedTuple[currentIndex].x, bufferedTuple[currentIndex].y);
                char temp[ELEMENT_LENGTH+1];
                itoa(bufferedTuple[currentIndex].x, temp, 10);
                memcpy(bufferedWriter+writeCount*TUPLIE_LENGTH, temp, ELEMENT_LENGTH);
                itoa(bufferedTuple[currentIndex].y, temp, 10);
                memcpy(bufferedWriter+writeCount*TUPLIE_LENGTH+ELEMENT_LENGTH, temp, ELEMENT_LENGTH);
                if ((++writeCount) == TUPLES_PER_BLOCK) {
                    itoa(writeAddress+1, bufferedWriter+TUPLES_PER_BLOCK*TUPLIE_LENGTH, 10);
                    printf("ע�����д�����:%d\n", writeAddress);
                    if (writeBlockToDisk(bufferedWriter, writeAddress++, &buf) != 0) {
                        perror("Writing Block Failed!\n");
                        return;
                    }
                    writeCount = 0;
                    bufferedWriter = getNewBlockInBuffer(&buf);
                }
            }
            currentIndex++;
        }
        freeBlockInBuffer(bufferedReader, &buf);
    }

    if (writeCount != 0) {
        memset(bufferedWriter+writeCount*TUPLIE_LENGTH, 0, 64-writeCount*TUPLIE_LENGTH);
        itoa(writeAddress+1, bufferedWriter+TUPLES_PER_BLOCK*TUPLIE_LENGTH, 10);
        printf("ע�����д�����:%d\n", writeAddress);
        if (writeBlockToDisk(bufferedWriter, writeAddress++, &buf) != 0) {
            perror("Writing Block Failed!\n");
            return;
        }
    }
    setColor(RED);
    printf("����ѡ��������Ԫ��һ��:%d��\n", validTuples);
    printf("IO��дһ��%d��\n", buf.numIO);
    setColor(WHITE);
    freeBuffer(&buf);
    return;
}


void sortedInnerjoin() {
    Buffer buf;
    if (!initBuffer(520, 64, &buf)) {
        perror("Buffer Initialization Failed!\n");
        return;
    }
    int innerjoinKey = 0, innerjoinOperationTicker = 0;
    char* bufferedReaderR = readBlockFromDisk(SORTED_R_INIT_ADDR, &buf);
    char* bufferedReaderS = readBlockFromDisk(SORTED_S_INIT_ADDR, &buf);
    char* bufferedWriter = getNewBlockInBuffer(&buf);
    int writeAddress = SORTED_INNERJOIN_INIT_ADDR;
    int writeCount = 0, writeIndexR = 0, writeIndexS = 0, currentIndexR = 0, currentIndexS = 0;
    Tuple bufferedTupleR[TUPLES_PER_BLOCK], bufferedTupleS[TUPLES_PER_BLOCK];
    extractTuple(bufferedReaderR, bufferedTupleR);
    extractTuple(bufferedReaderS, bufferedTupleS);
    bool isJoinedR = false, isJoinedS = false;    // ��¼ǰһ��Ԫ�غ͵�ǰԪ���Ƿ���������

    while(TRUE) {      //��ʼ����������ϵģʽ
        if (bufferedTupleR[currentIndexR].x == bufferedTupleS[currentIndexS].x) {   //R.A=S.C�����Խ�������
            innerjoinOperationTicker++;
            isJoinedS = true;
            char temp[ELEMENT_LENGTH+1];        //�����ӽ��д��block����
            itoa(bufferedTupleS[currentIndexS].x, temp, 10);
            memcpy(bufferedWriter+writeCount*4*ELEMENT_LENGTH, temp, ELEMENT_LENGTH);
            itoa(bufferedTupleS[currentIndexS].y, temp, 10);
            memcpy(bufferedWriter+writeCount*4*ELEMENT_LENGTH+ELEMENT_LENGTH, temp, ELEMENT_LENGTH);
            itoa(bufferedTupleR[currentIndexR].x, temp, 10);
            memcpy(bufferedWriter+writeCount*4*ELEMENT_LENGTH+2*ELEMENT_LENGTH, temp, ELEMENT_LENGTH);
            itoa(bufferedTupleR[currentIndexR].y, temp, 10);
            memcpy(bufferedWriter+writeCount*4*ELEMENT_LENGTH+3*ELEMENT_LENGTH, temp, ELEMENT_LENGTH);
            writeCount++;
            if (writeCount == 3) {  //ÿ���������3��4Ԫ��
                memset(bufferedWriter+48, 0, 4*ELEMENT_LENGTH+ELEMENT_LENGTH);      //д�����
                itoa(writeAddress+1, bufferedWriter+TUPLES_PER_BLOCK*TUPLIE_LENGTH, 10);
                printf("ע�����д�����:%d\n", writeAddress);
                if (writeBlockToDisk(bufferedWriter, writeAddress++, &buf) != 0) {
                    perror("Writing Block Failed!\n");
                    return;
                }
                writeCount = 0;
                freeBlockInBuffer(bufferedWriter, &buf);
                bufferedWriter = getNewBlockInBuffer(&buf);
            }
            if ((++currentIndexS) == TUPLES_PER_BLOCK) {
                if ((++writeIndexS) == BLOCK_S_LENGTH) {
                    break;
                }
                currentIndexS = 0;
                freeBlockInBuffer(bufferedReaderS, &buf);
                bufferedReaderS = readBlockFromDisk(SORTED_S_INIT_ADDR+writeIndexS, &buf);
                extractTuple(bufferedReaderS, bufferedTupleS);
            }
        } else if (bufferedTupleR[currentIndexR].x > bufferedTupleS[currentIndexS].x) {     //R.A>S.C,��S������
            currentIndexS++;
            if (currentIndexS == TUPLES_PER_BLOCK) {
                writeIndexS++;
                if (writeIndexS == BLOCK_S_LENGTH){
                    break;
                }
                currentIndexS = 0;
                freeBlockInBuffer(bufferedReaderS, &buf);
                bufferedReaderS = readBlockFromDisk(SORTED_S_INIT_ADDR + writeIndexS, &buf);
                extractTuple(bufferedReaderS, bufferedTupleS);
            }
        } else {
            if (bufferedTupleR[currentIndexR].x == innerjoinKey && isJoinedR && !isJoinedS) {   //R.A=S.C,��S��ǰ����
                if (currentIndexS == 0) {
                    if (writeIndexS == 0) {
                        break;
                    } else {
                        currentIndexS = 6;
                        freeBlockInBuffer(bufferedReaderS, &buf);
                        bufferedReaderS = readBlockFromDisk(SORTED_S_INIT_ADDR+(--writeIndexS), &buf);
                        extractTuple(bufferedReaderS, bufferedTupleS);
                    }
                } else {
                    currentIndexS--;
                }
                while(bufferedTupleS[currentIndexS].x == bufferedTupleR[currentIndexR].x) {
                    if (currentIndexS == 0) {
                        if (writeIndexS == 0) {
                            break;
                        } else {
                            currentIndexS = 6;
                            freeBlockInBuffer(bufferedReaderS, &buf);
                            bufferedReaderS = readBlockFromDisk(SORTED_S_INIT_ADDR+(--writeIndexS), &buf);
                            extractTuple(bufferedReaderS, bufferedTupleS);
                        }
                    }
                    currentIndexS--;
                }
            } else {
                isJoinedR = isJoinedS;
                isJoinedS = false;
                innerjoinKey = bufferedTupleR[currentIndexR++].x;
                if (currentIndexR == TUPLES_PER_BLOCK) {
                    if ((++writeIndexR) == BLOCK_R_LENGTH) {
                        break;
                    }
                    currentIndexR = 0;
                    freeBlockInBuffer(bufferedReaderR, &buf);
                    bufferedReaderR = readBlockFromDisk(SORTED_R_INIT_ADDR + writeIndexR, &buf);
                    extractTuple(bufferedReaderR, bufferedTupleR);
                }
            }
        }
    }
    if (writeCount) {
        memset(bufferedWriter+writeCount*4*ELEMENT_LENGTH, 0, 64-writeCount*4*ELEMENT_LENGTH);
        itoa(writeAddress+1, bufferedWriter+TUPLES_PER_BLOCK*TUPLIE_LENGTH, 10);
        printf("ע�����д�����:%d\n", writeAddress);
        if (writeBlockToDisk(bufferedWriter, writeAddress++, &buf) != 0) {
            perror("Writing Block Failed!\n");
            return;
        }
    }
    freeBlockInBuffer(bufferedReaderR, &buf);
    freeBlockInBuffer(bufferedReaderS, &buf);
    printf("������ɣ���ִ�����Ӳ���%d��\n", innerjoinOperationTicker);
    printf("IO��������Ϊ��%d\n", buf.numIO);
    freeBuffer(&buf);
    return;
}


void intersectSchema() {
    Buffer buf;
    if(!initBuffer(520, 64, &buf)) {
        perror("Buffer Initialization Failed!\n");
        return;
    }
    int intersectKey = 0, intersectionCount = 0;
    char* bufferedReaderR = readBlockFromDisk(SORTED_R_INIT_ADDR, &buf);
    char* bufferedReaderS = readBlockFromDisk(SORTED_S_INIT_ADDR, &buf);
    char* bufferedWriter = getNewBlockInBuffer(&buf);
    int writeIndex = 0, writeAddress = SORTED_INTERSECT_INIT_ADDR;
    int readIndexR = 0, writeIndexS = 0, currentIndexR = 0, currentIndexS = 0;
    Tuple bufferedTupleR[TUPLES_PER_BLOCK], bufferedTuples[TUPLES_PER_BLOCK];
    extractTuple(bufferedReaderR, bufferedTupleR);
    extractTuple(bufferedReaderS, bufferedTuples);

    while(TRUE) {
        if (bufferedTupleR[currentIndexR].x == bufferedTuples[currentIndexS].x) {   //���R.A=S.C����Ƚ�R.B��S.D
            if (!currentIndexS) {    //����S,���״ν��㿪ʼ
                if (writeIndexS) {
                    currentIndexS = 6;
                    freeBlockInBuffer(bufferedReaderS, &buf);
                    bufferedReaderS = readBlockFromDisk(SORTED_S_INIT_ADDR + (--writeIndexS), &buf);
                    extractTuple(bufferedReaderS, bufferedTuples);
                }
            } else {
                currentIndexS--;
            }
            if (writeIndexS!= 0||currentIndexS!= 0) {
                while(bufferedTuples[currentIndexS].x == bufferedTupleR[currentIndexR].x) { //R.A=S.C,���ǿ�ĵ�һ��Ԫ�飬�����Ҫ����ǰһ����
                    if (!currentIndexS) {
                        if (!writeIndexS) {
                            currentIndexS--;
                            break;
                        } else {
                            currentIndexS = TUPLES_PER_BLOCK;
                            freeBlockInBuffer(bufferedReaderS, &buf);
                            bufferedReaderS=readBlockFromDisk(SORTED_S_INIT_ADDR+(--writeIndexS), &buf);
                            extractTuple(bufferedReaderS, bufferedTuples);
                        }
                    }
                    currentIndexS--;
                }
                if ((++currentIndexS) == TUPLES_PER_BLOCK) {
                    if ((++writeIndexS) == BLOCK_S_LENGTH) {
                        break;
                    }
                    currentIndexS = 0;
                    freeBlockInBuffer(bufferedReaderS, &buf);
                    bufferedReaderS = readBlockFromDisk(SORTED_S_INIT_ADDR + writeIndexS, &buf);
                    extractTuple(bufferedReaderS, bufferedTuples);
                }
            }


            while(bufferedTuples[currentIndexS].x == bufferedTupleR[currentIndexR].x) {     //R.A=S.C
                if (bufferedTuples[currentIndexS].y == bufferedTupleR[currentIndexR].y) {   //R.B=S.D
                    printf("(X=%d, Y=%d)\n", bufferedTuples[currentIndexS].x, bufferedTuples[currentIndexS].y);
                    intersectionCount++;
                    char temp[ELEMENT_LENGTH+1];
                    itoa(bufferedTuples[currentIndexS].x, temp, 10);     //�ҵ���Ԫ�أ�д��block
                    memcpy(bufferedWriter+writeIndex*TUPLIE_LENGTH, temp, ELEMENT_LENGTH);
                    itoa(bufferedTuples[currentIndexS].y, temp, 10);
                    memcpy(bufferedWriter+writeIndex*TUPLIE_LENGTH+ELEMENT_LENGTH, temp, ELEMENT_LENGTH);
                    if ((++writeIndex) == TUPLES_PER_BLOCK) {
                        memset(bufferedWriter+TUPLES_PER_BLOCK*TUPLIE_LENGTH, 0, TUPLIE_LENGTH);
                        itoa(writeAddress+1, bufferedWriter+TUPLES_PER_BLOCK*TUPLIE_LENGTH, 10);
                        printf("ע�����д�����:%d\n", writeAddress);
                        if (writeBlockToDisk(bufferedWriter, (writeAddress++), &buf) != 0) {
                            perror("Writing Block Failed!\n");
                            return;
                        }
                        writeIndex = 0;
                        memcpy(bufferedWriter, emptyString, 64);
                        bufferedWriter = getNewBlockInBuffer(&buf);
                    }
                }
                if((++currentIndexS) == TUPLES_PER_BLOCK) {
                    if((++writeIndexS) == BLOCK_S_LENGTH){
                        break;
                    }
                    currentIndexS = 0;
                    freeBlockInBuffer(bufferedReaderS, &buf);
                    bufferedReaderS = readBlockFromDisk(SORTED_S_INIT_ADDR+writeIndexS, &buf);
                    extractTuple(bufferedReaderS, bufferedTuples);
                }
            }
            intersectKey = bufferedTupleR[currentIndexR].x;
            if((++currentIndexR) == TUPLES_PER_BLOCK) {
                readIndexR++;
                if(readIndexR == BLOCK_R_LENGTH){
                    break;
                }
                currentIndexR = 0;
                freeBlockInBuffer(bufferedReaderR, &buf);
                bufferedReaderR = readBlockFromDisk(SORTED_R_INIT_ADDR + readIndexR, &buf);
                extractTuple(bufferedReaderR, bufferedTupleR);
            }
            if(currentIndexS == TUPLES_PER_BLOCK) {
                if((++writeIndexS) == BLOCK_S_LENGTH){
                    break;
                }
                currentIndexS = 0;
                freeBlockInBuffer(bufferedReaderS, &buf);
                bufferedReaderS = readBlockFromDisk(SORTED_S_INIT_ADDR + writeIndexS, &buf);
                extractTuple(bufferedReaderS, bufferedTuples);
            }
        } else if(bufferedTupleR[currentIndexR].x > bufferedTuples[currentIndexS].x) {
            intersectKey = bufferedTuples[currentIndexS].x;
            if ((++currentIndexS) == TUPLES_PER_BLOCK) {
                if ((++writeIndexS) == BLOCK_S_LENGTH){
                    break;
                }
                currentIndexS = 0;
                freeBlockInBuffer(bufferedReaderS, &buf);
                bufferedReaderS = readBlockFromDisk(SORTED_S_INIT_ADDR + writeIndexS, &buf);
                extractTuple(bufferedReaderS, bufferedTuples);
            }
        } else {
            if(bufferedTupleR[currentIndexR].x==intersectKey) {
                if (currentIndexS == 0) {
                    if (writeIndexS) {
                        currentIndexS = 6;
                        freeBlockInBuffer(bufferedReaderS, &buf);
                        bufferedReaderS = readBlockFromDisk(SORTED_S_INIT_ADDR+(--writeIndexS), &buf);
                        extractTuple(bufferedReaderS, bufferedTuples);
                    }
                } else {
                    currentIndexS--;
                }
            } else {
                if ((++currentIndexR) == TUPLES_PER_BLOCK) {
                    if ((++readIndexR) == BLOCK_R_LENGTH){
                        break;
                    }
                    currentIndexR = 0;
                    freeBlockInBuffer(bufferedReaderR, &buf);
                    bufferedReaderR = readBlockFromDisk(SORTED_R_INIT_ADDR + readIndexR, &buf);
                    extractTuple(bufferedReaderR, bufferedTupleR);
                }
            }
        }
    }

    if (writeIndex) {
        memset(bufferedWriter+writeIndex*2*TUPLIE_LENGTH, 0, 64-writeIndex*2*TUPLIE_LENGTH);
        itoa(writeAddress+1, bufferedWriter+TUPLES_PER_BLOCK*TUPLIE_LENGTH, 10);
        printf("ע�����д�����:%d\n", writeAddress);
        if (writeBlockToDisk(bufferedWriter, (writeAddress++), &buf) != 0) {
            perror("Writing Block Failed!\n");
            return;
        }
    }
    setColor(RED);
    printf("R��S�Ľ����й���%d��Ԫ��\n", intersectionCount);
    printf("IO������%d��\n", buf.numIO);
    freeBlockInBuffer(bufferedReaderR, &buf);
    freeBlockInBuffer(bufferedReaderS, &buf);
    setColor(WHITE);
    freeBuffer(&buf);
    return;
}


void unionSchema() {
    Buffer buf;
    if (!initBuffer(520, 64, &buf)) {
        perror("Buffer Initialization Failed!\n");
        return;
    }
    char temp[ELEMENT_LENGTH+1];
    Tuple bufferedTupleS = {0}, bufferedTupleR = {0};
    bool completeFlag = false, unionFlag = false, completeKeyFlag = false, isUnited = false, isShifted = false;
    int unionTicker = 0, writeIndexR = 0, writeIndexS = 0, writeCount = 0, readIndex = 0, currentTupleKey = 0;
    int bufferedDataS = 0, bufferedDataR = 0, currentDataS = 0, currentDataR = 0, previousDataS = 0, previousDataR = 0;
    int readAddressR = SORTED_R_INIT_ADDR, currentAddressKey = SORTED_R_INIT_ADDR, readAddressS = SORTED_S_INIT_ADDR, writeAddress = SORTED_UNION_INIT_ADDR;
    unsigned char *bufferedReaderR = readBlockFromDisk(SORTED_R_INIT_ADDR, &buf);
    unsigned char *bufferedReaderS = readBlockFromDisk(SORTED_S_INIT_ADDR, &buf);
    unsigned char *bufferedWriter = getNewBlockInBuffer(&buf);
    memcpy(temp, bufferedReaderS, ELEMENT_LENGTH);
    currentDataS = bufferedDataS = atoi(temp);
    memcpy(temp, bufferedReaderR, ELEMENT_LENGTH);
    currentDataR = bufferedDataR = atoi(temp);

    while(!completeFlag) {
        if(bufferedDataR > bufferedDataS) {
            if(!isUnited) {
                itoa(bufferedDataS, temp, 10);
                memcpy(bufferedWriter+readIndex*TUPLIE_LENGTH, temp, ELEMENT_LENGTH);
                memcpy(temp, bufferedReaderS+writeIndexS*TUPLIE_LENGTH+ELEMENT_LENGTH, ELEMENT_LENGTH);
                bufferedTupleS.y = atoi(temp);
                memcpy(bufferedWriter+readIndex*TUPLIE_LENGTH+ELEMENT_LENGTH, temp, ELEMENT_LENGTH);
                unionTicker++;
                if((++readIndex) == TUPLES_PER_BLOCK) {
                    itoa(writeAddress+writeCount+1, bufferedWriter+readIndex*TUPLIE_LENGTH, 10);
                    if (writeBlockToDisk(bufferedWriter, writeAddress+writeCount, &buf) != 0) {
                        perror("Writing Block Failed!\n");
                        return;
                    }
                    readIndex = 0;
                    printf("ע:���д�����:%d\n", writeAddress+(writeCount++));
                    bufferedWriter = getNewBlockInBuffer(&buf);
                }
            }
            isUnited = 0;
            if((++writeIndexS) == TUPLES_PER_BLOCK) {
                if((++readAddressS) <= SORTED_S_INIT_ADDR+BLOCK_S_LENGTH-1) {
                    freeBlockInBuffer(bufferedReaderS, &buf);
                    bufferedReaderS = readBlockFromDisk(readAddressS, &buf);
                    writeIndexS = 0;
                } else {
                    completeFlag=true;
                }
            }
            if(!completeFlag) {
                memcpy(temp, bufferedReaderS+writeIndexS*TUPLIE_LENGTH, ELEMENT_LENGTH);
                currentDataS = atoi(temp);
                previousDataS = bufferedDataS;
            }
            bool retraceFlag = !completeFlag && unionFlag && currentDataS==bufferedDataS;
            if(retraceFlag) {
                if(readAddressR != currentAddressKey) {
                    freeBlockInBuffer(bufferedReaderR, &buf);
                    bufferedReaderR = readBlockFromDisk(currentAddressKey, &buf);
                }
                writeIndexR = currentTupleKey;
                readAddressR = currentAddressKey;
                memcpy(temp, bufferedReaderR+writeIndexR*TUPLIE_LENGTH, ELEMENT_LENGTH);
                bufferedDataR = atoi(temp);
            } else {
                unionFlag = false;
                if(currentDataS!=bufferedDataS) {
                    bufferedDataS = currentDataS;
                    isShifted = true;
                }
            }
        } else if(bufferedDataR < bufferedDataS) {
            itoa(bufferedDataR,temp,10);
            memcpy(bufferedWriter+readIndex*TUPLIE_LENGTH, temp, ELEMENT_LENGTH);
            memcpy(temp, bufferedReaderR+writeIndexR*TUPLIE_LENGTH+ELEMENT_LENGTH, ELEMENT_LENGTH);
            bufferedTupleR.y = atoi(temp);
            memcpy(bufferedWriter+readIndex*TUPLIE_LENGTH+ELEMENT_LENGTH,temp,ELEMENT_LENGTH);
            unionTicker++;
            if((++readIndex)==TUPLES_PER_BLOCK) {
                itoa(writeAddress+writeCount+1, bufferedWriter+readIndex*TUPLIE_LENGTH, 10);
                if (writeBlockToDisk(bufferedWriter, writeAddress+writeCount, &buf) != 0)  {
                    perror("Writing Block Failed!\n");
                    return;
                }
                printf("ע:���д�����:%d\n", writeAddress+(writeCount++));
                readIndex = 0;
                bufferedWriter = getNewBlockInBuffer(&buf);
            }
            if((++writeIndexR) == TUPLES_PER_BLOCK) {
                if((++readAddressR) <= SORTED_R_INIT_ADDR+BLOCK_R_LENGTH-1) {
                    writeIndexR = 0;
                    freeBlockInBuffer(bufferedReaderR,&buf);
                    bufferedReaderR = readBlockFromDisk(readAddressR, &buf);
                } else {
                    completeFlag=true;
                }
            }
            if(!completeFlag) {
                previousDataR = bufferedDataR;
                memcpy(temp, bufferedReaderR+writeIndexR*TUPLIE_LENGTH, ELEMENT_LENGTH);
                currentDataR = atoi(temp);
            }
            if(currentDataR != bufferedDataR) {
                bufferedDataR = currentDataR;
                currentTupleKey = writeIndexR;
                currentAddressKey = readAddressR;
            }
        } else if(bufferedDataS == bufferedDataR) {
            memcpy(temp, bufferedReaderS+writeIndexS*TUPLIE_LENGTH+ELEMENT_LENGTH, ELEMENT_LENGTH);
            bufferedTupleS.y = atoi(temp);
            memcpy(temp, bufferedReaderR+writeIndexR*TUPLIE_LENGTH+ELEMENT_LENGTH, ELEMENT_LENGTH);
            bufferedTupleR.y = atoi(temp);
            unionFlag = true;
            if(bufferedTupleS.y == bufferedTupleR.y && previousDataS == bufferedDataS) {
                isUnited = true;
            }else if(bufferedTupleS.y != bufferedTupleR.y && previousDataS != bufferedDataS) {
                itoa(bufferedDataR, temp,10);
                memcpy(bufferedWriter+readIndex*TUPLIE_LENGTH, temp, ELEMENT_LENGTH);
                itoa(bufferedTupleR.y, temp, 10);
                memcpy(bufferedWriter+readIndex*TUPLIE_LENGTH+ELEMENT_LENGTH, temp, ELEMENT_LENGTH);
                unionTicker++;
                if((++readIndex) == TUPLES_PER_BLOCK) {
                    itoa(writeAddress+writeCount+1, bufferedWriter+readIndex*TUPLIE_LENGTH, 10);
                    if (writeBlockToDisk(bufferedWriter, writeAddress+writeCount, &buf) != 0) {
                        perror("Writing Block Failed!\n");
                        return;
                    }
                    printf("ע:���д�����:%d\n", writeAddress+(writeCount++));
                    readIndex = 0;
                    bufferedWriter = getNewBlockInBuffer(&buf);
                }
            }
            if(previousDataR != bufferedDataR && isShifted) {
                isShifted = 0;
                currentTupleKey = writeIndexR;
                currentAddressKey = readAddressR;
            }
            if((++writeIndexR) == TUPLES_PER_BLOCK) {
                if((++readAddressR) <= SORTED_R_INIT_ADDR+BLOCK_R_LENGTH-1) {
                    writeIndexR = 0;
                    freeBlockInBuffer(bufferedReaderR, &buf);
                    bufferedReaderR = readBlockFromDisk(readAddressR, &buf);
                } else {
                    completeKeyFlag = true;
                    bufferedDataR = UPPER_LIMIT;
                }
            }
            if(!completeKeyFlag) {
                memcpy(temp, bufferedReaderR+writeIndexR*TUPLIE_LENGTH, ELEMENT_LENGTH);
                currentDataR = atoi(temp);
                previousDataR = bufferedDataR;
                bufferedDataR = currentDataR;
            }
        }
    }
    if(readIndex) {
        itoa(writeAddress+writeCount+1, bufferedWriter+TUPLES_PER_BLOCK*TUPLIE_LENGTH, 10);
        if(writeBlockToDisk(bufferedWriter, writeAddress+writeCount, &buf) != 0) {
            perror("Writing Block Failed!\n");
            return;
        }
        printf("ע:���д�����:%d\n", writeAddress+(writeCount++));
        bufferedWriter = getNewBlockInBuffer(&buf);
    }
    setColor(RED);
    printf("R��S�Ĳ����й���%d��Ԫ��\n", unionTicker);
    printf("IO������%d��\n", buf.numIO);
    setColor(WHITE);
    freeBlockInBuffer(bufferedWriter,&buf);
    freeBuffer(&buf);
    return;
}


void differenceSchema() {
    Buffer buf;
    if (!initBuffer(520, 64, &buf)) {
        perror("Buffer Initialization Failed!\n");
        return;
    }
    char temp[ELEMENT_LENGTH+1];
    Tuple bufferedTupleS = {0} , bufferedTupleR = {0};
    int differenceTicker = 0, writeIndexS = 0, writeIndexR = 0, writeCount = 0, readIndex = 0, currentTupleKey = 0;
    int bufferedDataS = 0, bufferedDataR = 0, currentDataR = 0, currentDataS = 0, previousDataR = 0, previousDataS = 0;
    bool completeFlag = false, differenceFlag = false, completeKeyFlag = false, isDifferentiated = false, isShifted = false;
    int currentAddressKey = SORTED_R_INIT_ADDR,readAddressR = SORTED_R_INIT_ADDR, readAddressS = SORTED_S_INIT_ADDR, writeAddress = SORTED_DIFFERENCE_INIT_ADDR;
    unsigned char *bufferedReaderS = readBlockFromDisk(SORTED_S_INIT_ADDR, &buf);
    unsigned char *bufferedReaderR = readBlockFromDisk(SORTED_R_INIT_ADDR, &buf);
    unsigned char *bufferedWriter = getNewBlockInBuffer(&buf);
    memcpy(temp, bufferedReaderS, ELEMENT_LENGTH);
    currentDataS = bufferedDataS = atoi(temp);
    memcpy(temp, bufferedReaderR, ELEMENT_LENGTH);
    currentDataR = bufferedDataR = atoi(temp);

    while(!completeFlag) {
        if(bufferedDataR > bufferedDataS) {
            if(!isDifferentiated) {
                differenceTicker++;
                itoa(bufferedDataS, temp, 10);
                memcpy(bufferedWriter+readIndex*TUPLIE_LENGTH, temp, ELEMENT_LENGTH);
                memcpy(temp, bufferedReaderS+writeIndexS*TUPLIE_LENGTH+ELEMENT_LENGTH, ELEMENT_LENGTH);
                bufferedTupleS.y = atoi(temp);
                memcpy(bufferedWriter+readIndex*TUPLIE_LENGTH+ELEMENT_LENGTH, temp, ELEMENT_LENGTH);
                if((++readIndex) == TUPLES_PER_BLOCK) {
                    itoa(writeAddress+writeCount+1, bufferedWriter+readIndex*TUPLIE_LENGTH, 10);
                    if (writeBlockToDisk(bufferedWriter, writeAddress+writeCount, &buf) != 0) {
                        perror("Writing Block Failed!\n");
                        return;
                    }
                    printf("ע:���д�����:%d\n",writeAddress+(writeCount++));
                    readIndex = 0;
                    bufferedWriter = getNewBlockInBuffer(&buf);
                }
            }
            isDifferentiated = false;
            if((++writeIndexS) == TUPLES_PER_BLOCK) {
                if((++readAddressS) <= SORTED_S_INIT_ADDR+BLOCK_S_LENGTH-1) {
                    freeBlockInBuffer(bufferedReaderS, &buf);
                    writeIndexS = 0;
                    bufferedReaderS = readBlockFromDisk(readAddressS, &buf);
                } else {
                    completeFlag = true;
                }
            }
            if(!completeFlag) {
                memcpy(temp, bufferedReaderS+writeIndexS*TUPLIE_LENGTH, ELEMENT_LENGTH);
                currentDataS = atoi(temp);
                previousDataS = bufferedDataS;
            }
            bool retraceFlag = !completeFlag && differenceFlag && currentDataS == bufferedDataS;
            if(retraceFlag) {
                if(readAddressR != currentAddressKey) {
                    freeBlockInBuffer(bufferedReaderR,&buf);
                    bufferedReaderR = readBlockFromDisk(currentAddressKey, &buf);
                }
                writeIndexR = currentTupleKey;
                readAddressR = currentAddressKey;
                memcpy(temp, bufferedReaderR+writeIndexR*TUPLIE_LENGTH, ELEMENT_LENGTH);
                bufferedDataR = atoi(temp);
            } else {
                if(currentDataS != bufferedDataS) {
                    isShifted = false;
                    bufferedDataS = currentDataS;
                }
                differenceFlag = false;
            }
        } else if(bufferedDataR < bufferedDataS) {
            if((++writeIndexR) == TUPLES_PER_BLOCK) {
                if((++readAddressR) <= SORTED_R_INIT_ADDR+BLOCK_R_LENGTH-1) {
                    writeIndexR = 0;
                    freeBlockInBuffer(bufferedReaderR, &buf);
                    bufferedReaderR = readBlockFromDisk(readAddressR, &buf);
                }
                else {
                    completeFlag = true;
                }
            }
            if(!completeFlag) {
                memcpy(temp, bufferedReaderR+writeIndexR*TUPLIE_LENGTH, ELEMENT_LENGTH);
                currentDataR = atoi(temp);
                previousDataR = bufferedDataR;
            }
            if(currentDataR != bufferedDataR) {
                bufferedDataR = currentDataR;
                currentTupleKey = writeIndexR;
                currentAddressKey = readAddressR;
            }
        } else if(bufferedDataS == bufferedDataR) {
            memcpy(temp, bufferedReaderS+writeIndexS*TUPLIE_LENGTH+ELEMENT_LENGTH, ELEMENT_LENGTH);
            bufferedTupleS.y = atoi(temp);
            memcpy(temp, bufferedReaderR+writeIndexR*TUPLIE_LENGTH+ELEMENT_LENGTH, ELEMENT_LENGTH);
            bufferedTupleR.y = atoi(temp);
            differenceFlag = true;
            if(bufferedTupleS.y == bufferedTupleR.y) {
                isDifferentiated = true;
            } else if(previousDataR != bufferedDataR && isShifted) {
                isShifted = false;
                currentTupleKey = writeIndexR;
                currentAddressKey = readAddressR;
            }
            if((++writeIndexR) == TUPLES_PER_BLOCK) {
                if((++readAddressR) <= SORTED_R_INIT_ADDR+BLOCK_R_LENGTH-1) {
                    writeIndexR = 0;
                    freeBlockInBuffer(bufferedReaderR,&buf);
                    bufferedReaderR = readBlockFromDisk(readAddressR, &buf);
                } else {
                    completeKeyFlag = true;
                    bufferedDataR = UPPER_LIMIT;
                }
            }
            if(!completeKeyFlag) {
                memcpy(temp, bufferedReaderR+writeIndexR*TUPLIE_LENGTH, ELEMENT_LENGTH);
                currentDataR = atoi(temp);
                bufferedDataR = currentDataR;
                previousDataR = bufferedDataR;
            }
        }
    }
    if(readIndex) {
        itoa(writeAddress+writeCount+1, bufferedWriter+TUPLES_PER_BLOCK*TUPLIE_LENGTH, 10);
        if (writeBlockToDisk(bufferedWriter, writeAddress+(writeCount++), &buf) != 0) {
            perror("Writing Block Failed!\n");
            return;
        }
        printf("ע:���д�����:%d\n",writeAddress+writeCount);
        bufferedWriter = getNewBlockInBuffer(&buf);
    }
    setColor(RED);
    printf("R��S�Ĳ�й���%d��Ԫ��\n", differenceTicker);
    printf("IO������%d��\n", buf.numIO);
    setColor(WHITE);
    freeBlockInBuffer(bufferedWriter,&buf);
    freeBuffer(&buf);
    return;
}
