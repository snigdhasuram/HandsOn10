#include <stdio.h>
#include <stdlib.h>

//doubly linked list
struct Node {
    int key;
    int data;
    struct Node* next;
    struct Node* prev;
};

// HashTable struct
struct HashTable {
    struct Node** table;
    int capacity;
    int size;
    float loadFactorThreshold;
};


int hash(const struct HashTable* ht, int key) {
    const float A = 0.61803398875;  // Multiplication constant (golden ratio)
    return (int)(ht->capacity * ((key * A) - (int)(key * A)));
}


struct Node* createNode(int key, int data) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    if (newNode == NULL) {
        printf("Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    newNode->key = key;
    newNode->data = data;
    newNode->next = NULL;
    newNode->prev = NULL;
    return newNode;
}


void resizeTable(struct HashTable* ht, int newCapacity) {
    struct Node** newTable = (struct Node**)malloc(newCapacity * sizeof(struct Node*));
    if (newTable == NULL) {
        printf("Memory allocation error\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < newCapacity; ++i) {
        newTable[i] = NULL;
    }

    for (int i = 0; i < ht->capacity; ++i) {
        struct Node* current = ht->table[i];
        while (current != NULL) {
            struct Node* next = current->next;
            int index = hash(ht, current->key) % newCapacity;

            if (newTable[index] == NULL) {
                newTable[index] = current;
                current->next = NULL;
            } else {
                current->next = newTable[index];
                newTable[index]->prev = current;
                newTable[index] = current;
            }
            current = next;
        }
    }

    free(ht->table);
    ht->table = newTable;
    ht->capacity = newCapacity;
}


struct HashTable* initHashTable(int initialCapacity, float loadFactor) {
    struct HashTable* ht = (struct HashTable*)malloc(sizeof(struct HashTable));
    if (ht == NULL) {
        printf("Memory allocation error\n");
        exit(EXIT_FAILURE);
    }

    ht->capacity = initialCapacity;
    ht->size = 0;
    ht->loadFactorThreshold = loadFactor;
    ht->table = (struct Node**)malloc(initialCapacity * sizeof(struct Node*));
    if (ht->table == NULL) {
        printf("Memory allocation error\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < initialCapacity; ++i) {
        ht->table[i] = NULL;
    }

    return ht;
}


void insert(struct HashTable* ht, int key, int data) {
    int index = hash(ht, key) % ht->capacity;
    struct Node* newNode = createNode(key, data);

    if (ht->table[index] == NULL) {
        ht->table[index] = newNode;
    } else {
        newNode->next = ht->table[index];
        ht->table[index]->prev = newNode;
        ht->table[index] = newNode;
    }

    ht->size++;

    if ((float)ht->size / ht->capacity > ht->loadFactorThreshold) {
        resizeTable(ht, 2 * ht->capacity);
    }
}


void removeKey(struct HashTable* ht, int key) {
    int index = hash(ht, key) % ht->capacity;
    struct Node* current = ht->table[index];

    while (current != NULL) {
        if (current->key == key) {
            if (current->prev == NULL) {
                ht->table[index] = current->next;
                if (current->next != NULL) {
                    current->next->prev = NULL;
                }
            } else {
                current->prev->next = current->next;
                if (current->next != NULL) {
                    current->next->prev = current->prev;
                }
            }
            free(current);
            ht->size--;

            
            if ((float)ht->size / ht->capacity < ht->loadFactorThreshold / 4) {
                resizeTable(ht, ht->capacity / 2);
            }

            return;
        }
        current = current->next;
    }
}

int get(const struct HashTable* ht, int key) {
    int index = hash(ht, key) % ht->capacity;
    struct Node* current = ht->table[index];

    while (current != NULL) {
        if (current->key == key) {
            return current->data;
        }
        current = current->next;
    }

    return -1;  
}


void print(const struct HashTable* ht) {
    for (int i = 0; i < ht->capacity; ++i) {
        printf("Bucket %d: ", i);
        struct Node* current = ht->table[i];
        while (current != NULL) {
            printf("(%d, %d) ", current->key, current->data);
            current = current->next;
        }
        printf("\n");
    }
}


int main() {
    
    struct HashTable* ht = initHashTable(5, 0.5);


    insert(ht, 5, 50);
    insert(ht, 10, 150);
    insert(ht, 15, 250);
    insert(ht, 20, 350);
  
    print(ht);

    printf("Value for key 5: %d\n", get(ht, 5));
    printf("Value for key 15: %d\n", get(ht, 15));

    removeKey(ht, 15);

    print(ht);

    free(ht);

    return 0;
}
