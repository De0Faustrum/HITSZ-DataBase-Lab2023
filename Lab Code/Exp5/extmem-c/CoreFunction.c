#include "defines.h"

static unsigned char* emptyString = "                                                                ";
static int indexLength = 0;


void linearSearch() {
    Buffer buf;
    if (!initBuffer(520, 64, &buf)) {   //初始化缓冲区
        perror("Buffer Initialization Failed!\n");
        return;
    }
    char temp[5];
    int foundCount = 0;     //满足选择条件的元组数
    int writeCount = 0;     //已写入的Block数
    int bufferedReaderAddress = SRC_S_INIT_ADDR;    //S首地址
    int bufferedWriterAddress = LINEAR_SEARCH_INIT_ADDR;    //结果首地址
    unsigned char* bufferedReader = getNewBlockInBuffer(&buf);  //读缓冲
    unsigned char* bufferedWriter = getNewBlockInBuffer(&buf);  //写缓冲

    while(TRUE) {           //开始读磁盘中的Block

        bufferedReader = readBlockFromDisk(bufferedReaderAddress, &buf);
        printf("读入数据块%d\n", bufferedReaderAddress);

        for (int i=0; i<TUPLES_PER_BLOCK; i++) {

            Tuple bufferedTuple;                            //从读缓冲区获得元组,从char*转化为int型
            memcpy(temp, bufferedReader+i*TUPLIE_LENGTH, ELEMENT_LENGTH);
            bufferedTuple.x = atoi(temp);
            memcpy(temp, bufferedReader+i*TUPLIE_LENGTH+ELEMENT_LENGTH, ELEMENT_LENGTH);
            bufferedTuple.y = atoi(temp);

            if (bufferedTuple.x == 107) {               //选择条件：S.C=107
                printf("(X=%d, Y=%d)\n", bufferedTuple.x, bufferedTuple.y);     //打印满足选择条件的元组
                memcpy(bufferedWriter+foundCount*TUPLIE_LENGTH, bufferedReader+i*TUPLIE_LENGTH, TUPLIE_LENGTH);     //将满足条件的元组添加到写缓冲区
                foundCount++;

                if (foundCount == TUPLES_PER_BLOCK) {   //此块已写7个元组?
                    itoa(bufferedWriterAddress+writeCount+1, bufferedWriter+TUPLES_PER_BLOCK*TUPLIE_LENGTH, 10);    //将下个block的地址添加到写缓冲区
                    if (writeBlockToDisk(bufferedWriter, bufferedWriterAddress + writeCount, &buf) != 0) {      //将缓冲区内的数据写入磁盘block
                        perror("Writing Block Failed!\n");
                        return;
                    }
                    writeCount++;
                    foundCount = 0;                     //重置块内元组计数器
                    freeBlockInBuffer(bufferedWriter, &buf);
                    bufferedWriter = getNewBlockInBuffer(&buf);
                }
            }
        }
        memcpy(temp, bufferedReader+TUPLES_PER_BLOCK*TUPLIE_LENGTH, ELEMENT_LENGTH);        //读取block末尾的数据标识下个block地址
        bufferedReaderAddress = atoi(temp);
        if (bufferedReaderAddress >= SRC_S_INIT_ADDR+BLOCK_S_LENGTH) {          //若读地址已过48则读终止
            break;
        }
        freeBlockInBuffer(bufferedReader, &buf);
    }

    if (foundCount) {   //将写缓冲区内的剩余数据写入磁盘block
        memcpy(bufferedWriter+foundCount*TUPLIE_LENGTH, emptyString, (TUPLES_PER_BLOCK-foundCount)*TUPLIE_LENGTH);
        itoa(bufferedWriterAddress+writeCount+1, bufferedWriter+TUPLES_PER_BLOCK*TUPLIE_LENGTH, 10);    //将下个block的地址添加到写缓冲区
        if (writeBlockToDisk(bufferedWriter, bufferedWriterAddress+writeCount, &buf) != 0) {    //写磁盘
            perror("Writing Block Failed!\n");
            return;
        }
        writeCount++;
    }
    for (int i=0; i<writeCount; i++) {
        printf("注：结果写入磁盘:%d\n", bufferedWriterAddress + i);
    }
    setColor(RED);
    printf("满足选择条件的元组一共:%d个\n", (writeCount-1)*TUPLES_PER_BLOCK + foundCount);
    printf("IO读写一共%d次\n", buf.numIO);
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
    unsigned char* bufferedBlock[6] = {NULL}; //由于缓冲区长度限制，每次读入6个block进行排序
    unsigned char* bufferedWriter = NULL;     //写缓冲区
    int writeOrder = 0;      //记录带写入块的写入地址下标，为7时需要写入磁盘

    while (TRUE) {
        bufferedWriter = getNewBlockInBuffer(&buf);
        remnantBlock = termReadAddress-readAdrress;     //剩余未参与排序的块数
        Tuple tempTuple;
        if((++remnantBlock) >= 6) {
            remnantBlock = 6;                   //大于6块则选择前6块进行排序
        }
        for(int i=0; i<remnantBlock; i++) {     //读入block并进行内部排序
            if ((bufferedBlock[i] = readBlockFromDisk(readAdrress+i, &buf)) == NULL) {
                perror("Reading Block Failed!\n");
                return;
            }
            extractTuple(bufferedBlock[i], bufferedTuple);
            for(int j=0; j<TUPLES_PER_BLOCK; j++) {       //内排序，采用选择排序
                temp = bufferedTuple[j].x;                 //每轮选择Tuple中X值最小的元组提至最前
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
                itoa(bufferedTuple[j].x, tempConvert, 10);      //内排序后写回block
                memcpy(bufferedBlock[i]+j*TUPLIE_LENGTH, tempConvert, ELEMENT_LENGTH);
                itoa(bufferedTuple[j].y, tempConvert, 10);
                memcpy(bufferedBlock[i]+j*TUPLIE_LENGTH+ELEMENT_LENGTH, tempConvert, ELEMENT_LENGTH);
            }
        }


        int indexVector[6] = {0};        //第1次多路归并，indexVector的每个维标识一个block中归并进度的下标
        while (!isMerged(indexVector, remnantBlock)) {
            Tuple element[6];            //暂存每路中的极小值
            for (int i = 0; i < remnantBlock; i++) {
                if (indexVector[i] < TUPLES_PER_BLOCK) {
                    memcpy(tempConvert, bufferedBlock[i]+indexVector[i]*TUPLIE_LENGTH, ELEMENT_LENGTH);
                    element[i].x = atoi(tempConvert);
                    memcpy(tempConvert, bufferedBlock[i]+indexVector[i]*TUPLIE_LENGTH+ELEMENT_LENGTH, ELEMENT_LENGTH);
                    element[i].y = atoi(tempConvert);
                } else{
                    element[i].x = UPPER_LIMIT;     //此路已归并完毕，将此路的S.C设为上界，确保以后不会再选择此路
                }
            }
            temp = UPPER_LIMIT;
            for(int i=0; i<remnantBlock; i++) {
                if (element[i].x < temp) {
                    temp = element[i].x;
                    order = i;                              //选择6路中极小值的下标
                }
            }
            itoa(element[order].x, tempConvert, 10);        //将极小值并入总集
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
            if(indexVector[order] < TUPLES_PER_BLOCK){      //若此块还未归并完毕
                indexVector[order]++;                       //此块的归并进度下标自增
            }
        }
        readAdrress += remnantBlock;
        freeBlockInBuffer(bufferedWriter, &buf);
        for (int i = 0; i < remnantBlock; i++) {
            freeBlockInBuffer(bufferedBlock[i], &buf);
        }
        if (readAdrress == termReadAddress + 1) {           //若所有快都已归并过，则跳出循环
            break;
        }
    }

    bufferedWriter = getNewBlockInBuffer(&buf);     // 第2次多路归并
    int segmentRange = termReadAddress-initReadAddress+1;
    int segmentCount = segmentRange/6;    //组数量
    int fragmentCount = 0;                //第二次归并中，一个segement包含<=6个fragmemt
    if(segmentRange%6){                   //fragmentCount标识最后一个segment中fragment的数量
        fragmentCount = segmentRange % 6; //fragment即block
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
        for(int i=0; i<segmentCount; i++) {     //复制每个segment中剩余block的第一个元组
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
        itoa(bufferedTuple[order].x, tempConvert, 10);      //选择最小元组写入写缓冲区
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
    int bufferedBlockCount = termReadAddress-initReadAddress+1;        //清理暂存磁盘block
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
        printf("对R的多路归并排序完成，IO读写%d次\n", buf.numIO);
    } else if(mode == 2) {
        printf("对S的多路归并排序完成，IO读写%d次\n", buf.numIO);
    }
    freeBuffer(&buf);
    return;
}


void constructIndex() {
    printf("开始建立索引：\n");
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
            if((nextKey=bufferedTuple[i].x) != currentKey) {            //找到相异元素则建立索引
                int indexValue = (readAddress-SORTED_S_INIT_ADDR)*TUPLES_PER_BLOCK+i;   //索引值
                char temp[ELEMENT_LENGTH+1];
                itoa(bufferedTuple[i].x, temp, 10);
                memcpy(bufferedWriter+writeCount*TUPLIE_LENGTH, temp, ELEMENT_LENGTH);
                sprintf(temp, "%-4d", indexValue);
                memcpy(bufferedWriter+writeCount*TUPLIE_LENGTH+ELEMENT_LENGTH, temp, ELEMENT_LENGTH);
                if ((++writeCount) == 7) {
                    itoa(writeAddress+1, bufferedWriter+TUPLES_PER_BLOCK*TUPLIE_LENGTH, 10);
                    printf("索引写入磁盘：%d\n", writeAddress);
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
        printf("索引写入磁盘：%d\n", writeAddress);
        if (writeBlockToDisk(bufferedWriter, writeAddress++, &buf) != 0) {
            perror("Writing Block Failed!\n");
            return;
        }
        indexLength++;
    }
    printf("建立索引完成，索引长度为%d个Block，IO次数为：%d\n", indexLength, buf.numIO);
}


void traverseIndex() {
    printf("根据索引进行查找：\n");
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

    for(int readAddress = INDEX_INIT_ADDR; readAddress < INDEX_INIT_ADDR+indexLength; readAddress++) {    //遍历索引块
        printf("读入索引块%d\n", readAddress);
        bufferedReader = readBlockFromDisk(readAddress, &buf);
        Tuple bufferedTuple[TUPLES_PER_BLOCK];
        extractTuple(bufferedReader, bufferedTuple);
        for(int i=0; i<TUPLES_PER_BLOCK-1; i++) {
            if (i == 0 && bufferedTuple[i].x > 107) {   //每个索引块的第一个索引键与上个块的最后一个索引键对比
                if (readAddress == INDEX_INIT_ADDR) {   //第一个索引块无需和前块对比
                    initIndex = dataAddrOffset;
                    termIndex = bufferedTuple[i].y+dataAddrOffset;
                    break;
                } else {                                //回溯上个块
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
            if (bufferedTuple[i].x <= 107 && bufferedTuple[i + 1].x > 107) {    //如果S.C=107在索引区间内
                initIndex = bufferedTuple[i].y + dataAddrOffset;
                termIndex = bufferedTuple[i + 1].y + dataAddrOffset;
                printf("已找到索引\n");
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
        Tuple bufferedTuple[7];         //根据索引值读取磁盘数据块
        printf("读入数据块%d\n", readAddress);
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
                    printf("注：结果写入磁盘:%d\n", writeAddress);
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
        printf("注：结果写入磁盘:%d\n", writeAddress);
        if (writeBlockToDisk(bufferedWriter, writeAddress++, &buf) != 0) {
            perror("Writing Block Failed!\n");
            return;
        }
    }
    setColor(RED);
    printf("满足选择条件的元组一共:%d个\n", validTuples);
    printf("IO读写一共%d次\n", buf.numIO);
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
    bool isJoinedR = false, isJoinedS = false;    // 记录前一个元素和当前元素是否发生了连接

    while(TRUE) {      //开始遍历两个关系模式
        if (bufferedTupleR[currentIndexR].x == bufferedTupleS[currentIndexS].x) {   //R.A=S.C，可以进行连接
            innerjoinOperationTicker++;
            isJoinedS = true;
            char temp[ELEMENT_LENGTH+1];        //将连接结果写入block缓冲
            itoa(bufferedTupleS[currentIndexS].x, temp, 10);
            memcpy(bufferedWriter+writeCount*4*ELEMENT_LENGTH, temp, ELEMENT_LENGTH);
            itoa(bufferedTupleS[currentIndexS].y, temp, 10);
            memcpy(bufferedWriter+writeCount*4*ELEMENT_LENGTH+ELEMENT_LENGTH, temp, ELEMENT_LENGTH);
            itoa(bufferedTupleR[currentIndexR].x, temp, 10);
            memcpy(bufferedWriter+writeCount*4*ELEMENT_LENGTH+2*ELEMENT_LENGTH, temp, ELEMENT_LENGTH);
            itoa(bufferedTupleR[currentIndexR].y, temp, 10);
            memcpy(bufferedWriter+writeCount*4*ELEMENT_LENGTH+3*ELEMENT_LENGTH, temp, ELEMENT_LENGTH);
            writeCount++;
            if (writeCount == 3) {  //每个块可容纳3个4元组
                memset(bufferedWriter+48, 0, 4*ELEMENT_LENGTH+ELEMENT_LENGTH);      //写入磁盘
                itoa(writeAddress+1, bufferedWriter+TUPLES_PER_BLOCK*TUPLIE_LENGTH, 10);
                printf("注：结果写入磁盘:%d\n", writeAddress);
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
        } else if (bufferedTupleR[currentIndexR].x > bufferedTupleS[currentIndexS].x) {     //R.A>S.C,将S向后遍历
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
            if (bufferedTupleR[currentIndexR].x == innerjoinKey && isJoinedR && !isJoinedS) {   //R.A=S.C,将S向前回溯
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
        printf("注：结果写入磁盘:%d\n", writeAddress);
        if (writeBlockToDisk(bufferedWriter, writeAddress++, &buf) != 0) {
            perror("Writing Block Failed!\n");
            return;
        }
    }
    freeBlockInBuffer(bufferedReaderR, &buf);
    freeBlockInBuffer(bufferedReaderS, &buf);
    printf("连接完成，共执行连接操作%d次\n", innerjoinOperationTicker);
    printf("IO操作次数为：%d\n", buf.numIO);
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
        if (bufferedTupleR[currentIndexR].x == bufferedTuples[currentIndexS].x) {   //如果R.A=S.C，则比较R.B和S.D
            if (!currentIndexS) {    //回溯S,从首次交点开始
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
                while(bufferedTuples[currentIndexS].x == bufferedTupleR[currentIndexR].x) { //R.A=S.C,且是块的第一个元组，因此需要回溯前一个块
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
                    itoa(bufferedTuples[currentIndexS].x, temp, 10);     //找到交元素，写入block
                    memcpy(bufferedWriter+writeIndex*TUPLIE_LENGTH, temp, ELEMENT_LENGTH);
                    itoa(bufferedTuples[currentIndexS].y, temp, 10);
                    memcpy(bufferedWriter+writeIndex*TUPLIE_LENGTH+ELEMENT_LENGTH, temp, ELEMENT_LENGTH);
                    if ((++writeIndex) == TUPLES_PER_BLOCK) {
                        memset(bufferedWriter+TUPLES_PER_BLOCK*TUPLIE_LENGTH, 0, TUPLIE_LENGTH);
                        itoa(writeAddress+1, bufferedWriter+TUPLES_PER_BLOCK*TUPLIE_LENGTH, 10);
                        printf("注：结果写入磁盘:%d\n", writeAddress);
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
        printf("注：结果写入磁盘:%d\n", writeAddress);
        if (writeBlockToDisk(bufferedWriter, (writeAddress++), &buf) != 0) {
            perror("Writing Block Failed!\n");
            return;
        }
    }
    setColor(RED);
    printf("R和S的交集中共有%d个元组\n", intersectionCount);
    printf("IO操作共%d次\n", buf.numIO);
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
                    printf("注:结果写入磁盘:%d\n", writeAddress+(writeCount++));
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
                printf("注:结果写入磁盘:%d\n", writeAddress+(writeCount++));
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
                    printf("注:结果写入磁盘:%d\n", writeAddress+(writeCount++));
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
        printf("注:结果写入磁盘:%d\n", writeAddress+(writeCount++));
        bufferedWriter = getNewBlockInBuffer(&buf);
    }
    setColor(RED);
    printf("R和S的并集中共有%d个元组\n", unionTicker);
    printf("IO操作共%d次\n", buf.numIO);
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
                    printf("注:结果写入磁盘:%d\n",writeAddress+(writeCount++));
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
        printf("注:结果写入磁盘:%d\n",writeAddress+writeCount);
        bufferedWriter = getNewBlockInBuffer(&buf);
    }
    setColor(RED);
    printf("R和S的差集中共有%d个元组\n", differenceTicker);
    printf("IO操作共%d次\n", buf.numIO);
    setColor(WHITE);
    freeBlockInBuffer(bufferedWriter,&buf);
    freeBuffer(&buf);
    return;
}
