#include "string.h"
#include "stdlib.h"
#include "stdio.h"

typedef struct node {
    struct node* next;
    unsigned char bits[13]; //104 bits
    int weight;
    int price;
} node;

void print(node* list, char* comment) {
    printf("%s\n", comment);
    while (list != NULL) {
        printf("weight: %d, price: %d\n", list->weight, list->price);
        list = list->next;
    }
    printf("\n");
}


node* merge_sort(node* first, node* second) {
    node* start = first;
    node* current = first->next;
    node* previous = first;
    node* tmp;

    first = first->next;

    while (first != NULL && second != NULL) {

        int weight1 = first->weight, weight2 = second->weight;
        int price1 = first->price, price2 = second->price;

        if (weight1 < weight2 && price1 < price2) {
            //add element from the first list
            previous->next = first;
            previous = first;
            first = first->next;
            current = first;
        } else if ((weight1 < weight2 && price1 >= price2) || (weight1 == weight2 && price1 >= price2)) {
            //first list dominating second list
            tmp = second;
            second = second->next;
            free(tmp);
        } else if (weight1 > weight2 && price1 > price2) {
            //add element from the second list
            previous->next = second;
            previous = second;
            second = second->next;
            current = second;
        } else if ((weight1 > weight2 && price1 <= price2) || (weight1 == weight2 && price1 < price2)) {
            // second list dominating first list
            tmp = first;
            first = first->next;
            free(tmp);
        }
    }

    while (first != NULL) {
        previous->next = first;
        previous = first;
        first = first->next;
    }

    while (second != NULL) {
        previous->next = second;
        previous = second;
        second = second->next;
    }

    return start;
}


int main() {

    int C, n;
    scanf("%d%d", &n, &C);

    int weight, price;

    node* list = (node*)malloc(sizeof(node));
    list->weight = 0;
    list->price = 0;
    list->next = NULL;
    memset(list->bits, 0, sizeof(list->bits));

    node* start_list = (node*)malloc(sizeof(node));
    start_list->weight = -1;
    start_list->price = -1;
    start_list->next = list;

    node* iterator;

    for (int i = 0; i < n; i++) {

        scanf("%d%d", &weight, &price);

        node* start_new_list = (node*)malloc(sizeof(node));
        start_new_list->weight = -1;
        start_new_list->price = -1;
        start_new_list->next = NULL;

        node* new_node;
        node* new_list = start_new_list;
        iterator = start_list->next;

        while (iterator != NULL) {
            int new_weight = iterator->weight + weight;
            if (new_weight <= C) {
                new_node = (node*)malloc(sizeof(node));
                memcpy(new_node->bits, iterator->bits, 13);
//                new_node->bits[i/8] |= (1 << (i%8));
                new_node->bits[i >> 3] |= (1 << (i & 7));
                new_node->weight = new_weight;
                new_node->price = iterator->price + price;
                new_node->next = NULL;

                new_list->next = new_node;
                new_list = new_list->next;
            }
            iterator = iterator->next;
        }

        //merge and sort list
        start_list = merge_sort(start_list, start_new_list->next);

        free(start_new_list);
    }

    node* previous;
    while (start_list->next != NULL) {
        previous = start_list;
        start_list = start_list->next;
        free(previous);
    }

    int i, count = 0;
    for (i = 0; i < 100; i++) {
        if ((start_list->bits[i/8] & (1 << (i%8))) != 0)
            count++;
    }

    printf("%d %d\n", start_list->price, count);

    for (i = 0; i < 100; i++) {
        if ((start_list->bits[i/8] & (1 << (i%8))) != 0)
            printf("%d\n", i);
    }

    free(start_list);

    return 0;
}
