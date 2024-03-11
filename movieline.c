//Assignment by Devon Villalona
//Programming Assignment 3
//COP3502C-24Spring 0025
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

// Define constants
#define MAX_NAME_LENGTH 51
#define MAX_TIME 2000000000
#define NUM_QUEUES 12
#define MAX_CUSTOMERS 500001

// Define structures for customer and queue
typedef struct {
    char name[MAX_NAME_LENGTH];
    int arrive_time;
    int checkout_time;
    int line_number;
} customer;

typedef struct {
    customer data[MAX_CUSTOMERS];
    int next_available_index;
} CustomerPool;

typedef struct node {
    customer *customer_ptr;
    struct node *next;
} node;

typedef struct {
    node *front;
    node *back;
    int size;
} queue;

// Global variable for customer pool
CustomerPool customer_pool;

// Function prototypes
queue *make_new_queue();
void enqueue(queue *q_ptr, customer *cust);
customer *dequeue(queue *q_ptr);
customer *peek(queue *q_ptr);
int is_empty(queue *q_ptr);
int get_size(queue *q_ptr);
int get_front_arrival_time(queue *q_ptr);
customer *make_customer(char *name, int num_tickets, int time_arrive, queue *lines[]);
int get_line(queue *lines[], char *name);
int *get_non_empty(queue *lines[], int *num_non_empty);
int get_next(queue *lines[], int *my_queues, int s_index, int e_index);
void initialize_customer_pool();
customer *get_customer_from_pool();
bool initialize_queues(queue *lines[NUM_QUEUES]);
bool read_operations_and_booths(int *num_operations, int *num_booths);
bool process_customers(queue *lines[NUM_QUEUES], int num_operations);
bool process_booths(queue *lines[NUM_QUEUES], int num_booths);
void free_queues(queue *lines[NUM_QUEUES]);

// Main function
int main() {
    queue *lines[NUM_QUEUES]; // Array of queues
    int num_operations, num_booths; // Variables to store number of operations and booths

    // Initialize queues
    if (!initialize_queues(lines)) {
        printf("Error initializing queues.\n");
        return EXIT_FAILURE;
    }

    // Read number of operations and booths
    if (!read_operations_and_booths(&num_operations, &num_booths)) {
        printf("Invalid input for operations and booths.\n");
        return EXIT_FAILURE;
    }

    // Process customers
    if (!process_customers(lines, num_operations)) {
        printf("Error processing customers.\n");
        return EXIT_FAILURE;
    }

    // Process booths
    if (!process_booths(lines, num_booths)) {
        printf("Error processing booths.\n");
        return EXIT_FAILURE;
    }

    // Free memory allocated for queues
    free_queues(lines);
    return EXIT_SUCCESS;
}

// Function to create a new queue
queue *make_new_queue() {
    queue *ptr = malloc(sizeof(queue));
    ptr->front = NULL;
    ptr->back = NULL;
    ptr->size = 0;
    return ptr;
}

// Function to enqueue a customer into the queue
void enqueue(queue *q_ptr, customer *cust) {
    node *tmp = malloc(sizeof(node));
    tmp->customer_ptr = cust;
    tmp->next = NULL;

    if (is_empty(q_ptr))
        q_ptr->front = tmp;
    else
        q_ptr->back->next = tmp;
    q_ptr->back = tmp;

    q_ptr->size++;
}

// Function to dequeue and return the customer at the front of the queue
customer *dequeue(queue *q_ptr) {
    if (is_empty(q_ptr))
        return NULL;

    node *tmp = q_ptr->front;
    customer *retval = tmp->customer_ptr;

    q_ptr->front = q_ptr->front->next;
    if (q_ptr->front == NULL)
        q_ptr->back = NULL;

    free(tmp);

    q_ptr->size--;

    return retval;
}

// Function to return the customer at the front of the queue without dequeuing
customer *peek(queue *q_ptr) {
    if (is_empty(q_ptr))
        return NULL;
    return q_ptr->front->customer_ptr;
}

// Function to check if the queue is empty
int is_empty(queue *q_ptr) {
    return q_ptr->size == 0;
}

// Function to return the size of the queue
int get_size(queue *q_ptr) {
    return q_ptr->size;
}

// Function to return the arrival time of the customer at the front of the queue
int get_front_arrival_time(queue *q_ptr) {
    if (is_empty(q_ptr))
        return MAX_TIME;
    return peek(q_ptr)->arrive_time;
}

// Function to create a new customer with the given information
customer *make_customer(char *name, int num_tickets, int time_arrive, queue *lines[]) {
    customer *tmp = get_customer_from_pool();
    if (tmp == NULL) {
        exit(EXIT_FAILURE);
    }

    strcpy(tmp->name, name);
    tmp->arrive_time = time_arrive;
    tmp->checkout_time = 30 + num_tickets * 5;
    tmp->line_number = get_line(lines, name);
    return tmp;
}

// Function to determine the line number for the customer based on their name
int get_line(queue *lines[], char *name) {
    int value = (name[0] - 'A') % 13;
    if (value != 0)
        return value - 1;
    int result = -1;
    for (int i = 0; i < NUM_QUEUES; i++) {
        if (!is_empty(lines[i])) {
            if (result == -1 || get_size(lines[i]) < get_size(lines[result]))
                result = i;
        }
    }
    if (result == -1)
        return 0;
    return result;
}

// Function to get the list of non-empty queues and return the number of non-empty queues
int *get_non_empty(queue *lines[], int *num_non_empty) {
    int *result = malloc(NUM_QUEUES * sizeof(int));
    int count = 0;
    for (int i = 0; i < NUM_QUEUES; i++) {
        if (!is_empty(lines[i])) {
            result[count++] = i;
        }
    }
    *num_non_empty = count;
    result = realloc(result, count * sizeof(int));
    return result;
}

// Function to get the index of the next customer to be served from the list of queues
int get_next(queue *lines[], int *my_queues, int start_index, int end_index) {
    int result = -1;
    int earliest_time = MAX_TIME;
    for (int i = start_index; i < end_index; i++) {
        int queue_index = my_queues[i];
        if (!is_empty(lines[queue_index])) {
            int front_arrival_time = get_front_arrival_time(lines[queue_index]);
            if (front_arrival_time < earliest_time) {
                result = queue_index;
                earliest_time = front_arrival_time;
            }
        }
    }
    return result;
}

// Function to initialize the customer pool
void initialize_customer_pool() {
    customer_pool.next_available_index = 0;
}

// Function to get a customer from the pool
customer *get_customer_from_pool() {
    if (customer_pool.next_available_index >= MAX_CUSTOMERS) {
        return NULL;
    }
    return &customer_pool.data[customer_pool.next_available_index++];
}

// Function to initialize all queues
bool initialize_queues(queue *lines[NUM_QUEUES]) {
    for (int i = 0; i < NUM_QUEUES; i++) {
        lines[i] = make_new_queue();
        if (lines[i] == NULL) {
            return false;
        }
    }
    return true;
}

// Function to read number of operations and booths
bool read_operations_and_booths(int *num_operations, int *num_booths) {
    if (scanf("%d%d", num_operations, num_booths) != 2) {
        return false;
    }
    return true;
}

// Function to process customers
bool process_customers(queue *lines[NUM_QUEUES], int num_operations) {
    for (int i = 0; i < num_operations; i++) {
        char name[MAX_NAME_LENGTH];
        int num_tickets, time_arrive;
        if (scanf("%s%d%d", name, &num_tickets, &time_arrive) != 3) {
            return false;
        }

        customer *tmp = make_customer(name, num_tickets, time_arrive, lines);
        if (tmp == NULL) {
            return false;
        }

        enqueue(lines[tmp->line_number], tmp);
    }
    return true;
}

// Function to process booths
bool process_booths(queue *lines[NUM_QUEUES], int num_booths) {
    int *my_queues;
    int num_non_empty;

    my_queues = get_non_empty(lines, &num_non_empty);

    int each = num_non_empty / num_booths;
    int get_extra = num_non_empty % num_booths;
    int j = 0;

    for (int i = 0; i < num_booths; i++) {
        printf("Booth %d\n", i + 1);
        int current_time = 0;
        int need = each;

        if (i < get_extra) {
            need++;
        }

        int next_person = get_next(lines, my_queues, j, j + need);

        while (next_person != -1) {
            customer *tmp = dequeue(lines[next_person]);

            if (tmp->arrive_time > current_time) {
                current_time = tmp->arrive_time;
            }

            current_time += tmp->checkout_time;

            printf("%s from line %d checks out at time %d.\n", tmp->name, tmp->line_number + 1, current_time);
            next_person = get_next(lines, my_queues, j, j + need);
        }

        j += need;
        printf("\n");

        if (j >= num_non_empty) {
            break;
        }
    }

    free(my_queues);
    return true;
}

// Function to free memory allocated for queues
void free_queues(queue *lines[NUM_QUEUES]) {
    for (int i = 0; i < NUM_QUEUES; i++) {
        while (!is_empty(lines[i])) {
            customer *tmp = dequeue(lines[i]);
            free(tmp);
        }
        free(lines[i]);
    }
}



