# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <stdbool.h>
# include "extmem.h"


typedef struct list{
    int x;
    int y;
}List;


/*
    工具函数
*/

void readstr(unsigned char* blk,  List* list){
                char str[5];
                for (int k = 0; k < 4; k++)
                {
                    str[k] = *(blk  + k);
                }
                list->x = atoi(str);
                for (int k = 0; k < 4; k++)
                {
                    str[k] = *(blk + 4  + k);
                }
                 list->y = atoi(str);

}
void setstr(unsigned char* blk, List* list){
                char str[5];
               itoa(list->x, str, 10);
                for (int k = 0; k < 4; k++)
                    {
                        *(blk + k) = str[k];
                    }
                itoa(list->y, str, 10);
                for (int k = 0; k < 4; k++)
                    {
                        *(blk + 4 + k) = str[k];
                    }

}

void   printstr(unsigned char* blk){
                char str[5];
            for (int k = 0; k < 4; k++)
                {
                    str[k] = *(blk  + k);
                }
                printf("(%d ", atoi(str));
                for (int k = 0; k < 4; k++)
                {
                    str[k] = *(blk + 4  + k);
                }
                printf("%d)\n ", atoi(str));
}



void setstr_for_cat(unsigned char* blk, List* R_list, List* S_list){
              char str[5];
               itoa(R_list->x, str, 10);
                for (int k = 0; k < 4; k++)
                    {
                        *(blk + k) = str[k];
                    }
                itoa(R_list->y, str, 10);
                for (int k = 0; k < 4; k++)
                    {
                        *(blk + 4 + k) = str[k];
                    }
                itoa(S_list->x, str, 10);
                for (int k = 0; k < 4; k++)
                    {
                        *(blk + 8 +  k) = str[k];
                    }
                itoa(S_list->y, str, 10);
                for (int k = 0; k < 4; k++)
                    {
                        *(blk + 12 + k) = str[k];
                    }

}

void setlist(List R, List* S){
    S->x = R.x;
    S->y = R.y;
}



/*
* 线性搜索函数
*/
int liner_search(){
    Buffer buf;


    /*Initialize buf*/
    if(!initBuffer(520, 64 ,&buf)){
        perror("Buffer Initialization Failed!\n");
        return -1;
    }

    /* get new block in buffer*/
    unsigned char* pointer_read;
    unsigned char* pointer_write;
    pointer_read = getNewBlockInBuffer(&buf);
    pointer_write = getNewBlockInBuffer(&buf);
    /*S from 17.blk to 48.blk*/
    int read_addr = 17;
    int write_addr = 100;
    int cnt = 0; //cnt for result
    int write_cnt = 0;


    while(1){
        if((pointer_read = readBlockFromDisk(read_addr, &buf)) == NULL){
            perror("Reading Block Failed!\n");
            return -1;
        }

        printf("读入磁盘%d\n", read_addr);

        char strbuf[5];
        // 遍历块
        for(int i = 0; i <7; i++){
            for (int k = 0; k < 4; k++)
            {
                strbuf[k] = *(pointer_read + i*8 + k);
            }
            int c = atoi(strbuf);
            for (int k = 0; k < 4; k++)
            {
                strbuf[k] = *(pointer_read + 4 + i*8 + k);
            }
            int d = atoi(strbuf);
            if(c == 128){
                printf("(C=%d, D=%d)\n", c, d);
                memcpy(pointer_write + cnt*8, pointer_read+i*8, 8);
                cnt++;
                // write back
                if(cnt == 7){
                    itoa(write_addr+ write_cnt+1, pointer_write + cnt *8, 10);
                    /* Write the block to the hard disk */
                    if (writeBlockToDisk(pointer_write, write_addr+write_cnt, &buf) != 0)
                    {
                        perror("Writing Block Failed!\n");
                        return -1;
                    }
                    printf("写入磁盘%d\n", write_addr+write_cnt);
                    write_cnt++;
                    cnt = 0;

                }

            }
        }
        //
        for (int k = 0; k < 4; k++)
            {
                strbuf[k] = *(pointer_read + 56 + k);
            }
        int next_addr = atoi(strbuf);
        if(17 > next_addr  || next_addr > 48)
        {
            break;
        }
        read_addr = next_addr;
        freeBlockInBuffer(pointer_read, &buf);
    }
    // д???????
    if(cnt != 0){
        if (writeBlockToDisk(pointer_write, write_addr+write_cnt, &buf) != 0)
        {
            perror("Writing Block Failed!\n");
            return -1;
        }
        printf("写入磁盘%d\n", write_addr+write_cnt);
    }

    printf("\n");
    printf("满足条件元组:%d个\n", (write_cnt) * 7 + cnt);
    printf("IO times :%d \n", buf.numIO);
    freeBuffer(&buf);
    return 0;


}


/*
*  两阶段多路归并，第一次对子集合内部进行归并，第二次对子集合间进行归并
*/


int two_stage_multipath_merge_sort(int read_addr_begin, int read_length,int write_addr_begin){
    Buffer buf;

    if(!initBuffer(520, 64 ,&buf)){
        perror("Buffer Initialization Failed!\n");
        return -1;
    }

    int write_addr = write_addr_begin;
    int read_addr = read_addr_begin;

    //待排序块指针
    unsigned char* unsort_blk[10];
    unsigned char* pointer_write;



    int write_cnt = 0;
    int unsort_blk_num;
    int remain_length = read_length;
    char str[5];
    List lists[10];
    // 子集合内部排序
    while(remain_length != 0){
        if(remain_length < 6){
            unsort_blk_num = remain_length;
            remain_length = 0;
        }
        else{
            unsort_blk_num = 6;
            remain_length -= 6;
        }
        // 初始化
        for(int i = 0;i < unsort_blk_num; i++){
            if((unsort_blk[i] = readBlockFromDisk(read_addr +i, &buf)) == NULL){
                perror("Reading Block Failed!\n");
                return -1;
            }
        }
        //只会申请到第七块， 要注意申请位置，最好每次申请位次相同，否则会出现指针重合的问题
        pointer_write = getNewBlockInBuffer(&buf);

        for(int m = 0; m < unsort_blk_num; m++){

            for(int i =0; i <7; i++){
                readstr((unsort_blk[m])+i*8, &lists[i]);
                }

            for(int i = 0; i < 7; i++){
                int min_index = i;
                int miny = lists[i].y;
                int min = lists[i].x;
                for(int j = i + 1; j < 7; j++){
                    if(lists[j].x < min  || (lists[j].x == min && miny > lists[j].y)){
                        min_index = j;
                        min = lists[j].x;
                    }

                }
                List temp;
                temp.x = lists[i].x;
                temp.y = lists[i].y;
                lists[i].x = lists[min_index].x;
                lists[i].y = lists[min_index].y;
                lists[min_index].x = temp.x;
                lists[min_index].y = temp.y;

            }

            for(int i = 0; i < 7; i++){
                setstr((unsort_blk[m])+i*8, &lists[i]);
            }
            for(int i = 0; i < 7; i++){
                if(m >= 1 ){
                       // // printstr(unsort_blk[m - 1] + i *8, str);
                }
             }
        }
        // 归并多路排序
         int index[6];
            for(int i = 0; i <6 ;i++){
                index[i] = 0;
            }
            int write_num = unsort_blk_num * 7;
           for(int i = 0; i < unsort_blk_num; i++){
                    readstr(unsort_blk[i], &lists[i]);
                    index[i]++;
           }
           while(write_cnt < write_num){
                int min=lists[0].x;
                int min_index = 0;
                int miny = lists[0].y;
                for(int i = 0; i < unsort_blk_num; i++){
                    if(min > lists[i].x || (min == lists[i].x && lists[i].y < miny)){
                        min = lists[i].x;
                        min_index = i;
                        miny = lists[i].y;
                    }
                }
                setstr(pointer_write  + (write_cnt % 7)*8,  &lists[min_index]);

                write_cnt++;

                if(index[min_index]<=6){
                     for (int k = 0; k < 4; k++)
                        {
                            str[k] = *(unsort_blk[min_index] + index[min_index]*8 + k);
                        }
                        lists[min_index].x = atoi(str);
                        for (int k = 0; k < 4; k++)
                        {
                            str[k] = *(unsort_blk[min_index] + 4 + index[min_index]*8 + k);
                        }
                        lists[min_index].y = atoi(str);
                }
                else{
                    lists[min_index].x = 1111111111;
                    lists[min_index].y = 1111111111;
                 }
                index[min_index]++;
                // д???λ
                if(write_cnt % 7 == 0 && write_cnt != 0){
                    itoa(read_addr + 1, pointer_write + 56, 10);
                    if (writeBlockToDisk(pointer_write, read_addr, &buf) != 0)
                    {
                        perror("Writing Block Failed!\n");
                        return -1;
                    }
                     read_addr++;
                    }
                }
                write_cnt = 0;
            // ??????????
            for(int i = 0; i < unsort_blk_num; i++){
                freeBlockInBuffer(unsort_blk[i], &buf);

            }
            freeBlockInBuffer(pointer_write, &buf);
            if(read_addr == read_addr_begin + read_length){
                break;
            }
        }

           // 子集合间排序
           // 初始化
            int group_num = (read_length % 6 == 0) ? read_length/6 : read_length/6 + 1 ;
            int group_index[7];
            int block_index[7];
            read_addr = read_addr_begin;
            write_cnt = 0;
            for(int i =0; i < group_num; i++){
                group_index[i] = 0;
                block_index[i] = 0;

            }
            for(int i = 0; i < group_num; i++){
                 if ((unsort_blk[i] = readBlockFromDisk(read_addr + i*6, &buf)) == NULL)
                {
                    perror("Reading Block Failed!\n");
                    return -1;
                }
                readstr(unsort_blk[i], &lists[i]);
                block_index[i]++;
                group_index[i]++;
            }
            pointer_write = getNewBlockInBuffer(&buf);

            while(1){
                int min = lists[0].x;
                int min_index = 0;
                int miny = lists[0].y;
                for(int i =0 ;i < group_num; i++){
                    if(lists[i].x < min || (lists[i].x == min && lists[i].y < miny)){
                        min = lists[i].x;
                        min_index = i;
                        miny = lists[i].y;
                    }
                }

                setstr(pointer_write + write_cnt * 8, &lists[min_index]);
                 // printstr(pointer_write + write_cnt * 8 );
                write_cnt++;
                if(write_cnt == 7){
                    itoa(write_addr + 1, pointer_write + 56, 10);
                    if(writeBlockToDisk(pointer_write, write_addr,  &buf) != 0){
                             perror("Writing Block Failed!\n");
                            return -1;
                    }
                    write_addr++;
                    write_cnt = 0;
                }
                // 块更新
                if(block_index[min_index] >= 7){

                    if((group_index[min_index] < 6 && min_index != group_num - 1) || ((min_index == group_num -1) && group_index[min_index] < (read_length + 6 - 6 * group_num))){
                            freeBlockInBuffer(unsort_blk[min_index], &buf);
                            if((unsort_blk[min_index] = readBlockFromDisk(read_addr + group_index[min_index] + min_index * 6, &buf)) == NULL){
                                perror("Reading Block Failed!\n");
                                return -1;
                            }

                            block_index[min_index] = 1;
                            group_index[min_index]++;
                            readstr(unsort_blk[min_index],  &lists[min_index]);
                        }
                    else{
                        freeBlockInBuffer(unsort_blk[min_index], &buf);
                        block_index[min_index] = 8;
                        group_index[min_index] = 8;
                        lists[min_index].x = 11111111;
                        lists[min_index].y = 11111111;
                     }
                }
                else{
                    readstr((unsort_blk[min_index] + (block_index[min_index] * 8 )) , &lists[min_index]);
                     block_index[min_index]++;
                }
                int flag = 0;
                for(int i = 0; i < group_num; i++){
                    if(group_index[i] != 8){
                        flag = 1;
                    }

                }
                if(flag == 0){
                    break;
                }
            }
            for(int i =0; i < group_num; i++){
                freeBlockInBuffer(unsort_blk[i], &buf);
            }
            freeBlockInBuffer(pointer_write, &buf);
            int iotimes = buf.numIO;
            freeBuffer(&buf);
            return iotimes;


}


/*
* 建立索引到指定块，索引格式（字段值， 块号）
*/
int make_index(int addr_begin, int addr_store, int length){
        Buffer buf;
         /* Initialize the buffer */
        if (!initBuffer(520, 64, &buf))
        {
            perror("Buffer Initialization Failed!\n");
            return -1;
        }

        int read_addr = addr_begin;
        int write_addr = addr_store;
        int read_length = length;
        int X;
        int index = 0;
        int write_cnt = 0;
        List temp;
        // 若是当前索引过大，则可修改当前指针
        unsigned char* blk;
        unsigned char* pointer_write;

        pointer_write = getNewBlockInBuffer(&buf);

        // build index for x
        for(int i = 0; i < length; i++){
            if((blk = readBlockFromDisk(read_addr, &buf)) == NULL){
                perror("Read Failed!\n");
                return -1;
            }
            pointer_write = getNewBlockInBuffer(&buf);
            for(int j =0; j <7; j++){
               readstr(blk + j * 8, &X);
                if(X > index){
                    temp.x = X;
                    temp.y = read_addr;
                    setstr(pointer_write + write_cnt * 8, &temp);
                    // printstr(pointer_write + write_cnt *8);

                    index = X;
                    write_cnt++;
                    if(write_cnt == 7){
                        itoa(write_addr + 1, pointer_write + 56, 10);
                        if(writeBlockToDisk(pointer_write ,write_addr, &buf) != 0){
                             perror("Write Failed!\n");
                             return -1;
                        }
                        write_addr++;
                        write_cnt = 0;
                    }
                }


            }
            read_addr++;
            freeBlockInBuffer(blk, &buf);
            freeBlockInBuffer(pointer_write, &buf);

        }
        if(write_cnt != 0){
            temp.x = -1;
            temp.y = -1;
            setstr(pointer_write + write_cnt * 8, &temp);
            // printstr(pointer_write + write_cnt*8);


            write_cnt = 0;
            itoa(write_addr + 1, pointer_write + 56, 10);
            if(writeBlockToDisk(pointer_write ,write_addr, &buf) != 0){
                 perror("Write Failed!\n");
                 return -1;
            }
            write_addr++;

        }
        freeBlockInBuffer(blk, &buf);
        freeBlockInBuffer(pointer_write, &buf);
        freeBuffer(&buf);
        return write_addr - addr_store;

}

/*
 * 筛选指定字段的元组，并且写入指定块。
 */
void select_tuple(unsigned char* blk, unsigned char* pointer_write, int write_addr, int index, int* cnt, int* buf_io){
      List temp;
      unsigned char* target_blk;
      List index_list;
      int write_cnt =0;
      int target;
      Buffer buf;
      if (!initBuffer(520, 64, &buf))
        {
            perror("Buffer Initialization Failed!\n");
            return -1;
        }

      for(int i =0; i< 7; i++){
         readstr(blk + i * 8, &temp);
         if(temp.x == index){
            target = temp.y;
            if((target_blk = readBlockFromDisk(temp.y, &buf)) == NULL){
                perror("Read Failed!\n");
                return -1;
            }
            printf("读入磁盘%d\n", temp.y);
            for(int j = 0; j < 7; j++){
                readstr(target_blk + j * 8, &index_list);
                if(index_list.x == index){
                    printf("(C = %d, D = %d)\n", index_list.x, index_list.y);
                    (*cnt)++;
                    setstr(pointer_write + write_cnt * 8, &index_list);
                    // printstr(pointer_write + write_cnt*8);

                    write_cnt++;
                    if(write_cnt == 7){
                        itoa(write_addr + 1, pointer_write + 56, 10);
                        if(writeBlockToDisk(pointer_write, write_addr, &buf)!= 0){
                            perror("Write Failed!\n");
                            return -1;
                        }
                        printf("写入磁盘%d\n", write_addr);
                        write_addr++;
                        write_cnt= 0;
                    }
                }
                else{
                    break;
                }
            }
            freeBlockInBuffer(target_blk, &buf);
            target++;
            while(index_list.x == index){
                if((target_blk = readBlockFromDisk(target, &buf)) == NULL){
                    perror("Read Failed!\n");
                    return -1;
                }
                printf("读入磁盘%d\n", target);
                for(int j = 0; j < 7; j++){
                    readstr(target_blk + j*8, &index_list);
                    if(index_list.x == index){
                        printf("(C = %d, D = %d)\n", index_list.x, index_list.y);
                        (*cnt)++;
                         setstr(pointer_write + write_cnt * 8, &index_list);
                        // printstr(pointer_write + write_cnt * 8);

                            write_cnt++;
                            if(write_cnt == 7){
                                itoa(write_addr + 1, pointer_write + 56, 10);
                                if(writeBlockToDisk(pointer_write, write_addr, &buf)!= 0){
                                    perror("Write Failed!\n");
                                    return -1;
                                }
                                printf("写入磁盘%d\n", write_addr);
                                write_addr++;
                                write_cnt= 0;
                            }
                    }
                    else{
                        break;
                    }
                }
                target++;
                freeBlockInBuffer(target_blk, &buf);

            }
            if(write_cnt != 0){

                 itoa(write_addr + 1, pointer_write + 56, 10);
                if(writeBlockToDisk(pointer_write, write_addr, &buf)!= 0){
                    perror("Write Failed!\n");
                    return -1;
                }
                printf("写入磁盘%d\n", write_addr);
                write_addr++;
                write_cnt= 0;
            }
            break;
         }
      }
      *buf_io = buf.numIO;
      freeBuffer(&buf);

}

/*
 * 根据索引文件找到指定字段的元组，并且通过调用select_tuple函数将目标数据写到磁盘中
 *
*/

int index_search(int addr_begin, int store_length, int write_addr ,int index, List* result){
    Buffer buf;

    if (!initBuffer(520, 64, &buf))
    {
        perror("Buffer Initialization Failed!\n");
        return -1;
    }

    int index_addr = addr_begin;
    int index_store_length = store_length;
    int cnt = 0;
    int buf_io = 0;
    List temp;

    unsigned char* blk;
    unsigned char* pointer_write;


    for(int i = 0; i < index_store_length; i++){
        if((blk = readBlockFromDisk(index_addr, &buf)) == NULL){
             perror("Read Failed!\n");
            return -1;
        }
        printf("读入磁盘%d\n", index_addr);
        readstr(blk, &temp);
        if(temp.x > index){
                index_addr++;
            freeBlockInBuffer(blk, &buf);
            continue;
        }
        if(i != index_store_length - 1){
            readstr(blk + 48, &temp);
            if(temp.x < index){
                index_addr++;
                freeBlockInBuffer(blk, &buf);
                continue;
            }
        }
        else{
            for(int i =0 ;i < 7; i++){
                readstr(blk+i*8, &temp);
                if(temp.x == -1){
                    printf("索引已经用完，没有对应记录\n");
                    break;
                }
                else if(temp.x == index){
                    select_tuple(blk, pointer_write, write_addr , index, &cnt, &buf_io);
                    break;
                }
            }
        }
        pointer_write = getNewBlockInBuffer(&buf);
        select_tuple(blk, pointer_write, write_addr, index, &cnt, &buf_io);
        freeBlockInBuffer(blk, &buf);
        break;
    }
    result->x = cnt;
    result->y = buf.numIO + buf_io;
    freeBlockInBuffer(pointer_write, &buf);
    freeBuffer(&buf);
    return 0;



}

/*
 *排序链接函数，先要通过多路排序实现关系的有序存储
*/


int sort_merge_join(int read_addr_R, int read_addr_S, int write_addr, int read_lenght_R, int read_length_S){
    Buffer buf;
    if (!initBuffer(520, 64, &buf))
    {
        perror("Buffer Initialization Failed!\n");
        return -1;
    }

    int R_read_addr = read_addr_R;
    int S_read_addr = read_addr_S;
    int R_read_length_remain = read_lenght_R;
    int S_read_length_remain = read_length_S;

    unsigned char* R_read ;
    unsigned char* S_read;
    unsigned char* pointer_write;

    List R_list;
    List S_list;
    int R_write_cnt = 0 ;
    int S_write_cnt = 0;
    int cat_cnt =0;

    if((R_read = readBlockFromDisk(R_read_addr, &buf)) == NULL){
         perror("Read Failed!\n");
        return -1;
    }
    R_read_addr++;
    R_read_length_remain--;
    if((S_read = readBlockFromDisk(S_read_addr, &buf)) == NULL){
         perror("Read Failed!\n");
        return -1;
    }
    S_read_addr++;
    S_read_length_remain--;

    pointer_write = getNewBlockInBuffer(&buf);

    readstr(R_read, &R_list);
    readstr(S_read, &S_list);
    R_write_cnt++;
    S_write_cnt++;

    do{
        if(R_list.x < S_list.x){
            readstr(R_read +R_write_cnt * 8, &R_list);
            R_write_cnt++;
            if(R_write_cnt == 7 && R_read_length_remain != 0){
                freeBlockInBuffer(R_read, &buf);
                if((R_read = readBlockFromDisk(R_read_addr, &buf)) == NULL){
                     perror("Read Failed!\n");
                    return -1;
                }
                R_read_addr++;
                R_read_length_remain--;
                R_write_cnt = 0;
            }
        }
        else if(R_list.x > S_list.x){
            readstr(S_read +S_write_cnt * 8, &S_list);
            S_write_cnt++;
            if(S_write_cnt == 7 && S_read_length_remain != 0){
                freeBlockInBuffer(S_read, &buf);
                if((S_read = readBlockFromDisk(S_read_addr, &buf)) == NULL){
                     perror("Read Failed!\n");
                    return -1;
                }
                S_read_addr++;
                S_read_length_remain--;
                S_write_cnt = 0;
            }
        }
        else{

            List S_temp;
            int S_write_cnt_temp = S_write_cnt;
            int  S_remian_temp = S_read_length_remain;
            int S_read_addr_temp = S_read_addr;
            setlist(S_list, &S_temp);

            while(R_list.x == S_list.x){
                // printf("(R.A = %d, R.B = %d, S.C = %d, S.D = %d)\n",R_list.x , R_list.y, S_list.x, S_list.y);
                setstr_for_cat(pointer_write + (cat_cnt % 3) * 16, &R_list, &S_list);

                cat_cnt++;
                if(cat_cnt % 3 == 0){
                    itoa(write_addr+1, pointer_write + 56, 10);
                    if(writeBlockToDisk(pointer_write, write_addr, &buf) != 0){
                         perror("Write Failed!\n");
                        return -1;
                    }
                    printf("写入磁盘%d\n", write_addr);
                    write_addr++;
                }
                readstr(S_read +S_write_cnt * 8, &S_list);
                S_write_cnt++;
                if(S_write_cnt == 7 && S_read_length_remain != 0){
                    freeBlockInBuffer(S_read, &buf);
                    if((S_read = readBlockFromDisk(S_read_addr, &buf)) == NULL){
                         perror("Read Failed!\n");
                        return -1;
                    }
                    S_read_addr++;
                    S_read_length_remain--;
                    S_write_cnt = 0;
                }
            }
            readstr(R_read + R_write_cnt * 8, &R_list);
            R_write_cnt++;
            if(R_write_cnt == 7 && R_read_length_remain != 0){
                freeBlockInBuffer(R_read, &buf);
                if((R_read = readBlockFromDisk(R_read_addr, &buf)) == NULL){
                     perror("Read Failed!\n");
                    return -1;
                }
                R_read_addr++;
                R_read_length_remain--;
                R_write_cnt = 0;
            }
            if(S_read_length_remain == S_remian_temp){
                S_write_cnt = S_write_cnt_temp;
                setlist(S_temp, &S_list);

            }
            else{
                S_read_length_remain = S_remian_temp;
                freeBlockInBuffer(S_read, &buf);
                S_read_addr = S_read_addr_temp;
                if((S_read = readBlockFromDisk(S_read_addr-1, &buf)) == NULL){
                    perror("Read Failed!\n");
                    return -1;
                }
                S_write_cnt = S_write_cnt_temp;
                setlist(S_temp, &S_list);
            }
        }
    }while((R_read_length_remain > 0 || R_write_cnt <= 7) && (S_read_length_remain > 0 || S_write_cnt <= 7));
    if(cat_cnt%3  != 0){
        itoa(write_addr+1, pointer_write + 56, 10);
        if(writeBlockToDisk(pointer_write, write_addr, &buf) != 0){
             perror("Write Failed!\n");
            return -1;
        }
        printf("写入磁盘%d\n", write_addr);
        write_addr++;
    }
    freeBlockInBuffer(S_read, &buf);
    freeBlockInBuffer(R_read, &buf);
    freeBlockInBuffer(pointer_write, &buf);
    freeBuffer(&buf);
    return cat_cnt;



}
/*
    并函数，思路类似课上的算法
    return len(S∪R)
*/

int sort_merge_combine(int read_addr_R, int read_addr_S, int write_addr, int read_lenght_R, int read_length_S){
    Buffer buf;
    if (!initBuffer(520, 64, &buf))
    {
        perror("Buffer Initialization Failed!\n");
        return -1;
    }

    int R_read_addr = read_addr_R;
    int S_read_addr = read_addr_S;
    int R_read_length_remain = read_lenght_R;
    int S_read_length_remain = read_length_S;

    unsigned char* R_read ;
    unsigned char* S_read;
    unsigned char* pointer_write;

    List R_list;
    List S_list;
    List list_temp;
    int R_write_cnt = 0 ;
    int S_write_cnt = 0;
    int write_cnt =0;

    if((R_read = readBlockFromDisk(R_read_addr, &buf)) == NULL){
         perror("Read Failed!\n");
        return -1;
    }
    R_read_addr++;
    R_read_length_remain--;
    if((S_read = readBlockFromDisk(S_read_addr, &buf)) == NULL){
         perror("Read Failed!\n");
        return -1;
    }
    S_read_addr++;
    S_read_length_remain--;

    pointer_write = getNewBlockInBuffer(&buf);

    readstr(R_read, &R_list);
    readstr(S_read, &S_list);
    R_write_cnt++;
    S_write_cnt++;

    // 写数据和读数据可以考虑封装成为一个函数，但是封装的时候buf不能读，或者尝试的时候代码有问题，就导致代码如此臃肿。麻
    do{
        if((R_list.x < S_list.x && R_write_cnt <= 7) || (S_write_cnt > 7) || (R_list.x == S_list.x && R_list.y < S_list.y) ){
            if(list_temp.x == R_list.x && list_temp.y == R_list.y){
                write_cnt--;
            }
            setlist(R_list, &list_temp);
            setstr(pointer_write + (write_cnt%7)*8, &R_list);
            // printstr(pointer_write + (write_cnt%7)*8);

            write_cnt++;
            if(write_cnt %7 == 0){
                itoa(write_addr+1, pointer_write + 56, 10);
                if(writeBlockToDisk(pointer_write, write_addr, &buf) != 0){
                     perror("Write Failed!\n");
                    return -1;
                }
                printf("写入磁盘%d\n", write_addr);
                write_addr++;
            }
            readstr(R_read +R_write_cnt * 8, &R_list);
            R_write_cnt++;
            if(R_write_cnt == 7 && R_read_length_remain != 0){
                freeBlockInBuffer(R_read, &buf);
                if((R_read = readBlockFromDisk(R_read_addr, &buf)) == NULL){
                     perror("Read Failed!\n");
                    return -1;
                }
                R_read_addr++;
                R_read_length_remain--;
                R_write_cnt = 0;
            }
        }
        else if(S_list.x < R_list.x  || (S_list.x == R_list.x && S_list.y < R_list.y)){
            if(list_temp.x == S_list.x && list_temp.y == S_list.y){
                write_cnt--;
            }
           setstr(pointer_write + (write_cnt%7)*8, &S_list);
            // printstr(pointer_write + (write_cnt%7)*8);
            write_cnt++;
            if(write_cnt %7 == 0){
                itoa(write_addr+1, pointer_write + 56, 10);
                if(writeBlockToDisk(pointer_write, write_addr, &buf) != 0){
                     perror("Write Failed!\n");
                    return -1;
                }
                printf("写入磁盘%d\n", write_addr);
                write_addr++;
            }

            readstr(S_read +S_write_cnt * 8, &S_list);
            S_write_cnt++;
            if(S_write_cnt == 7 && S_read_length_remain != 0){
                freeBlockInBuffer(S_read, &buf);
                if((S_read = readBlockFromDisk(S_read_addr, &buf)) == NULL){
                     perror("Read Failed!\n");
                    return -1;
                }
                S_read_addr++;
                S_read_length_remain--;
                S_write_cnt = 0;
            }
        }
        else{
            setstr(pointer_write + (write_cnt%7)*8, &S_list);
            // printstr(pointer_write + (write_cnt%7)*8);

            write_cnt++;
            if(write_cnt %7 == 0){
                itoa(write_addr+1, pointer_write + 56, 10);
                if(writeBlockToDisk(pointer_write, write_addr, &buf) != 0){
                     perror("Write Failed!\n");
                    return -1;
                }
                printf("写入磁盘%d\n", write_addr);
                write_addr++;
            }

            readstr(S_read +S_write_cnt * 8, &S_list);
            S_write_cnt++;
            if(S_write_cnt == 7 && S_read_length_remain != 0){
                freeBlockInBuffer(S_read, &buf);
                if((S_read = readBlockFromDisk(S_read_addr, &buf)) == NULL){
                     perror("Read Failed!\n");
                    return -1;
                }
                S_read_addr++;
                S_read_length_remain--;
                S_write_cnt = 0;
            }

            readstr(R_read +R_write_cnt * 8, &R_list);
            R_write_cnt++;
            if(R_write_cnt == 7 && R_read_length_remain != 0){
                freeBlockInBuffer(R_read, &buf);
                if((R_read = readBlockFromDisk(R_read_addr, &buf)) == NULL){
                     perror("Read Failed!\n");
                    return -1;
                }
                R_read_addr++;
                R_read_length_remain--;
                R_write_cnt = 0;
            }
        }

    // 保证最后一个元组也可以遍历
    }while((R_read_length_remain > 0 || R_write_cnt <= 7) || (S_read_length_remain > 0 || S_write_cnt <= 7));
    if(write_cnt%7 != 0){
        itoa(write_addr+1, pointer_write + 56, 10);
        if(writeBlockToDisk(pointer_write, write_addr, &buf) != 0){
             perror("Write Failed!\n");
            return -1;
        }
        printf("写入磁盘%d\n", write_addr);
        write_addr++;
    }
    freeBlockInBuffer(S_read, &buf);
    freeBlockInBuffer(R_read, &buf);
    freeBlockInBuffer(pointer_write, &buf);
    freeBuffer(&buf);
    return write_cnt;



}

/*
    交函数
    return len(S^R)
*/
int sort_merge_over(int read_addr_R, int read_addr_S, int write_addr, int read_lenght_R, int read_length_S){
    Buffer buf;
    if (!initBuffer(520, 64, &buf))
    {
        perror("Buffer Initialization Failed!\n");
        return -1;
    }

    int R_read_addr = read_addr_R;
    int S_read_addr = read_addr_S;
    int R_read_length_remain = read_lenght_R;
    int S_read_length_remain = read_length_S;

    unsigned char* R_read ;
    unsigned char* S_read;
    unsigned char* pointer_write;

    List R_list;
    List S_list;
    int R_write_cnt = 0 ;
    int S_write_cnt = 0;
    int cat_cnt =0;

    if((R_read = readBlockFromDisk(R_read_addr, &buf)) == NULL){
         perror("Read Failed!\n");
        return -1;
    }
    R_read_addr++;
    R_read_length_remain--;
    if((S_read = readBlockFromDisk(S_read_addr, &buf)) == NULL){
         perror("Read Failed!\n");
        return -1;
    }
    S_read_addr++;
    S_read_length_remain--;

    pointer_write = getNewBlockInBuffer(&buf);

    readstr(R_read, &R_list);
    readstr(S_read, &S_list);
    R_write_cnt++;
    S_write_cnt++;

    do{
        if(R_list.x < S_list.x){
            readstr(R_read +R_write_cnt * 8, &R_list);
            R_write_cnt++;
            if(R_write_cnt == 7 && R_read_length_remain != 0){
                freeBlockInBuffer(R_read, &buf);
                if((R_read = readBlockFromDisk(R_read_addr, &buf)) == NULL){
                     perror("Read Failed!\n");
                    return -1;
                }
                R_read_addr++;
                R_read_length_remain--;
                R_write_cnt = 0;
            }
        }
        else if(R_list.x > S_list.x){
            readstr(S_read +S_write_cnt * 8, &S_list);
            S_write_cnt++;
            if(S_write_cnt == 7 && S_read_length_remain != 0){
                freeBlockInBuffer(S_read, &buf);
                if((S_read = readBlockFromDisk(S_read_addr, &buf)) == NULL){
                     perror("Read Failed!\n");
                    return -1;
                }
                S_read_addr++;
                S_read_length_remain--;
                S_write_cnt = 0;
            }
        }
        else{

            List S_temp;
            int S_write_cnt_temp = S_write_cnt;
            int  S_remian_temp = S_read_length_remain;
            int S_read_addr_temp = S_read_addr;
            setlist(S_list, &S_temp);

            while(R_list.x == S_list.x){
                if(R_list.y == S_list.y){
                    printf("(R.A = %d, R.B = %d, S.C = %d, S.D = %d)\n",R_list.x , R_list.y, S_list.x, S_list.y);
                    setstr_for_cat(pointer_write + (cat_cnt % 3) * 16, &R_list, &S_list);
                    cat_cnt++;
                    if(cat_cnt % 3 == 0){
                        itoa(write_addr+1, pointer_write + 56, 10);
                        if(writeBlockToDisk(pointer_write, write_addr, &buf) != 0){
                             perror("Write Failed!\n");
                            return -1;
                        }
                        printf("写入磁盘%d\n", write_addr);
                        write_addr++;
                    }
                }
                readstr(S_read +S_write_cnt * 8, &S_list);
                S_write_cnt++;
                if(S_write_cnt == 7 && S_read_length_remain != 0){
                    freeBlockInBuffer(S_read, &buf);
                    if((S_read = readBlockFromDisk(S_read_addr, &buf)) == NULL){
                         perror("Read Failed!\n");
                        return -1;
                    }
                    S_read_addr++;
                    S_read_length_remain--;
                    S_write_cnt = 0;
                }
            }
            readstr(R_read + R_write_cnt * 8, &R_list);
            R_write_cnt++;
            if(R_write_cnt == 7 && R_read_length_remain != 0){
                freeBlockInBuffer(R_read, &buf);
                if((R_read = readBlockFromDisk(R_read_addr, &buf)) == NULL){
                     perror("Read Failed!\n");
                    return -1;
                }
                R_read_addr++;
                R_read_length_remain--;
                R_write_cnt = 0;
            }
            if(S_read_length_remain == S_remian_temp){
                S_write_cnt = S_write_cnt_temp;
                setlist(S_temp, &S_list);

            }
            else{
                S_read_length_remain = S_remian_temp;
                freeBlockInBuffer(S_read, &buf);
                S_read_addr = S_read_addr_temp;
                if((S_read = readBlockFromDisk(S_read_addr-1, &buf)) == NULL){
                    perror("Read Failed!\n");
                    return -1;
                }
                S_write_cnt = S_write_cnt_temp;
                setlist(S_temp, &S_list);
            }
        }
    }while((R_read_length_remain > 0 || R_write_cnt <= 7) && (S_read_length_remain > 0 || S_write_cnt <= 7));
    if(cat_cnt%3  != 0){
        itoa(write_addr+1, pointer_write + 56, 10);
        if(writeBlockToDisk(pointer_write, write_addr, &buf) != 0){
             perror("Write Failed!\n");
            return -1;
        }
        printf("写入磁盘%d\n", write_addr);
        write_addr++;
    }
    freeBlockInBuffer(S_read, &buf);
    freeBlockInBuffer(R_read, &buf);
    freeBlockInBuffer(pointer_write, &buf);
    freeBuffer(&buf);
    return cat_cnt;



}

/*
    差函数
    return S-R的集合大小
*/

int sort_merge_sub(int read_addr_R, int read_addr_S, int write_addr, int read_lenght_R, int read_length_S){
    Buffer buf;
    if (!initBuffer(520, 64, &buf))
    {
        perror("Buffer Initialization Failed!\n");
        return -1;
    }

    int R_read_addr = read_addr_R;
    int S_read_addr = read_addr_S;
    int R_read_length_remain = read_lenght_R;
    int S_read_length_remain = read_length_S;

    unsigned char* R_read ;
    unsigned char* S_read;
    unsigned char* pointer_write;

    List R_list;
    List S_list;
    List list_temp;
    int R_write_cnt = 0 ;
    int S_write_cnt = 0;
    int write_cnt =0;

    if((R_read = readBlockFromDisk(R_read_addr, &buf)) == NULL){
         perror("Read Failed!\n");
        return -1;
    }
    R_read_addr++;
    R_read_length_remain--;
    if((S_read = readBlockFromDisk(S_read_addr, &buf)) == NULL){
         perror("Read Failed!\n");
        return -1;
    }
    S_read_addr++;
    S_read_length_remain--;

    pointer_write = getNewBlockInBuffer(&buf);

    readstr(R_read, &R_list);
    readstr(S_read, &S_list);
    R_write_cnt++;
    S_write_cnt++;

    // 写数据和读数据可以考虑封装成为一个函数，但是封装的时候buf不能读，就导致代码如此臃肿。麻
    do{
        if((R_list.x < S_list.x && R_write_cnt <= 7) || (S_write_cnt > 7) || (R_list.x == S_list.x && R_list.y < S_list.y) ){
            readstr(R_read +R_write_cnt * 8, &R_list);
            R_write_cnt++;
            if(R_write_cnt == 7 && R_read_length_remain != 0){
                freeBlockInBuffer(R_read, &buf);
                if((R_read = readBlockFromDisk(R_read_addr, &buf)) == NULL){
                     perror("Read Failed!\n");
                    return -1;
                }
                R_read_addr++;
                R_read_length_remain--;
                R_write_cnt = 0;
            }
        }
        else if(S_list.x < R_list.x  || (S_list.x == R_list.x && S_list.y < R_list.y)){
            setstr(pointer_write + (write_cnt%7)*8, &S_list);
            // printstr(pointer_write + (write_cnt%7)*8);
            write_cnt++;
            if(write_cnt %7 == 0){
                itoa(write_addr+1, pointer_write + 56, 10);
                if(writeBlockToDisk(pointer_write, write_addr, &buf) != 0){
                     perror("Write Failed!\n");
                    return -1;
                }
                printf("写入磁盘%d\n", write_addr);
                write_addr++;
            }

            readstr(S_read +S_write_cnt * 8, &S_list);
            S_write_cnt++;
            if(S_write_cnt == 7 && S_read_length_remain != 0){
                freeBlockInBuffer(S_read, &buf);
                if((S_read = readBlockFromDisk(S_read_addr, &buf)) == NULL){
                     perror("Read Failed!\n");
                    return -1;
                }
                S_read_addr++;
                S_read_length_remain--;
                S_write_cnt = 0;
            }
        }
        else{


            readstr(S_read +S_write_cnt * 8, &S_list);
            S_write_cnt++;
            if(S_write_cnt == 7 && S_read_length_remain != 0){
                freeBlockInBuffer(S_read, &buf);
                if((S_read = readBlockFromDisk(S_read_addr, &buf)) == NULL){
                     perror("Read Failed!\n");
                    return -1;
                }
                S_read_addr++;
                S_read_length_remain--;
                S_write_cnt = 0;
            }

            readstr(R_read +R_write_cnt * 8, &R_list);
            R_write_cnt++;
            if(R_write_cnt == 7 && R_read_length_remain != 0){
                freeBlockInBuffer(R_read, &buf);
                if((R_read = readBlockFromDisk(R_read_addr, &buf)) == NULL){
                     perror("Read Failed!\n");
                    return -1;
                }
                R_read_addr++;
                R_read_length_remain--;
                R_write_cnt = 0;
            }
        }


    }while((R_read_length_remain > 0 || R_write_cnt <= 7) || (S_read_length_remain > 0 || S_write_cnt <= 7));
    if(write_cnt%7 != 0){
        itoa(write_addr+1, pointer_write + 56, 10);
        if(writeBlockToDisk(pointer_write, write_addr, &buf) != 0){
             perror("Write Failed!\n");
            return -1;
        }
        printf("写入磁盘%d\n", write_addr);
        write_addr++;
    }
    freeBlockInBuffer(S_read, &buf);
    freeBlockInBuffer(R_read, &buf);
    freeBlockInBuffer(pointer_write, &buf);
    freeBuffer(&buf);
    return write_cnt;



}


void db_task1(){

     printf("==================================\n 基于线性搜索的关系选择算法\n==================================\n");
    liner_search();
    return;
}

void db_task2(){
     printf("==================================\n 两阶段多路归并排序算法\n==================================\n");
    int Riotimes =  two_stage_multipath_merge_sort(1, 16, 301);
     printf("R IO times:%d\n", Riotimes);
    int Siotimes =  two_stage_multipath_merge_sort(17, 32, 317);
     printf("S IO times:%d\n", Siotimes);
    return;
}


void db_task3(){
    printf("==================================\n基于索引的关系选择算法\n==================================\n");
    int index_store_length = make_index(317, 217, 32);
    List result;
    index_search(217, index_store_length, 117, 128, &result);
    printf("满足条件的元组：%d\n", result.x);
    printf("IO times:%d\n", result.y);
    return;
}

void db_task4(){
    printf("==================================\n基于排序的连接操作算法\n==================================\n");
    int cat_cnt = sort_merge_join(301, 317, 400, 16, 32);
    printf("共连接%d\n", cat_cnt);
    return;
}

void db_task5(){
    printf("==================================\n基于排序的两趟扫描算法-并\n==================================\n");
    int write_cnt = sort_merge_combine(301,317,600, 16,32);
    printf("集合个数%d\n", write_cnt);
    printf("==================================\n基于排序的两趟扫描算法-交\n==================================\n");
    write_cnt = sort_merge_over(301,317,800, 16,32);
    printf("集合个数%d\n", write_cnt);
    printf("==================================\n基于排序的两趟扫描算法-差\n==================================\n");
    write_cnt = sort_merge_sub(301,317,1000, 16,32);
    printf("集合个数%d\n", write_cnt);
    return;
}
int main()
{
    db_task1();
    db_task2();
    db_task3();
    db_task4();
    db_task5();
    return 0;
}
