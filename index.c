#include <stdlib.h>
#include <stdio.h>
#include <time.h>


void (*func_ptr_queue[16384])(int);
int func_ptr_queue_parameter[16384];
int func_ptr_queue_index = 0;

void (*func_ptr_exec_queue[16384])(int);
int func_ptr_exec_queue_parameter[16384];
int func_ptr_exec_queue_index = 0;

int interrupt_index = 0;

void task_create(void (*func_ptr)(int), int n) {
    if (func_ptr_queue_index >= 16384) {
        printf("Out of range!!\n");
        return;
    }

    // printf("func_ptr_queue_index: %d\n", func_ptr_queue_index);

    func_ptr_queue[func_ptr_queue_index] = func_ptr;
    func_ptr_queue_parameter[func_ptr_queue_index] = n;
    func_ptr_queue_index++;
}

void migrate_func_to_exec_queue(void (**func_ptr_queue)(int), int *func_ptr_queue_parameter, void (**func_ptr_exec_queue)(int), int *func_ptr_exec_queue_parameter) {
    for (;func_ptr_exec_queue_index < func_ptr_queue_index; func_ptr_exec_queue_index++) {
        func_ptr_exec_queue[func_ptr_exec_queue_index] = func_ptr_queue[func_ptr_exec_queue_index];
        func_ptr_exec_queue_parameter[func_ptr_exec_queue_index] = func_ptr_queue_parameter[func_ptr_exec_queue_index];
    }
}

void hand_func(void (**func_ptr_queue)(int), int *func_ptr_queue_parameter) {
    if (interrupt_index == 0) {
        migrate_func_to_exec_queue(func_ptr_queue, func_ptr_queue_parameter, func_ptr_exec_queue, func_ptr_exec_queue_parameter);
        func_ptr_queue_index = 0;
    }

    clock_t start_t, timeout_t;
    start_t = clock();

    for (int i = interrupt_index; i < func_ptr_exec_queue_index; i++) {
        timeout_t = clock();
        if (((timeout_t - start_t) / CLOCKS_PER_SEC) > 2.0) {
            printf("INTERRUPT!\n");
            interrupt_index = i;
            return;
        }
        (*func_ptr_exec_queue[i])(func_ptr_exec_queue_parameter[i]);
    }

    interrupt_index = 0;
    func_ptr_exec_queue_index = 0;
}

void show_number(int n) {
    printf("%d\n", n);
}

void number_power(int n) {
    printf("%d\n", n*n);
}

void double_number(int n) {
    printf("%d\n", n+n);
}

void longTimeShowNumber(int n) {
    for (double i=0; i < 500000000; i++) {

    }
    printf("%d\n", n);
}

int main(void) {
    int i = 11;
    clock_t t1, t2;
    t1 = clock();
    for (int i=0; i < 10;i++) {
        void (*func)(int) = longTimeShowNumber;
        task_create(func, i);
    }
    while(1) {
        hand_func(func_ptr_queue, func_ptr_queue_parameter);
        t2 = clock();
        if (((t2 - t1) / CLOCKS_PER_SEC) == 30.0) {
            void (*func)(int) = longTimeShowNumber;
            task_create(func, i++);
            task_create(func, i++);
            task_create(func, i++);
            task_create(func, i++);
            task_create(func, i++);
            task_create(func, i++);
            task_create(func, i++);
            task_create(func, i++);
            task_create(func, i++);
            task_create(func, i++);
        }
    }

    return 0;
}