#include <stdio.h>


struct big_ass_struct
{
    int data1;
    char id[400];
};

#define MYQUEUE_TYPE struct big_ass_struct
#include "queue.h"

int main(void)
{
    struct big_ass_struct tmp_data = {.data1 = 433, .id = {'i', 'd','i','o','t'}};

    myqueue queue;
    myqueue_init(&queue);
    
    myqueue_enqueue(&queue, tmp_data);

    struct big_ass_struct data_2 = myqueue_dequeue(&queue);

    printf("data_2.data1 = %d\n", data_2.data1);
    return 0;
}
