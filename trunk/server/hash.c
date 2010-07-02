#include "MyleftServer.h"

static int hash_size = 0;
// BKDR Hash Function

unsigned int BKDRHash(const char *str) {
    log_write(LOG_ERR, "BKDRHash", __FILE__, __LINE__);
    unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
    unsigned int hash = 0;

    log_write(LOG_ERR, str, __FILE__, __LINE__);
    while (*str){
        log_write(LOG_ERR, "while", __FILE__, __LINE__);
        hash=hash*seed+(*str++);
    }
    log_write(LOG_ERR, "BKDRHash2", __FILE__, __LINE__);
    return (hash & 0x7FFFFFFF);
}

int hash_create(int size) {
    hash_size = size;
    hash_table = (hash_item **) malloc(size * sizeof (hash_item*));
    if (hash_table == NULL) return 0;
    int i;
    for (i = 0; i < size; i++) {
        hash_table[i] = NULL;
    }
    return 1;
}

void hash_destroy() {
    int i;
    for (i = 0; i < hash_size; i++) {
        if (hash_table[i] != NULL) free(hash_table[i]);
    }
    if (hash_table != NULL) free(hash_table);
}

hash_item *hash_search(const char *key) {
    log_write(LOG_ERR, "hash_search", __FILE__, __LINE__);
    log_write(LOG_ERR, key, __FILE__, __LINE__);
    if (hash_size > 0) {
        log_write(LOG_ERR, "hash_search2", __FILE__, __LINE__);
        unsigned int index = BKDRHash(key) % hash_size;
        log_write(LOG_ERR, "hash_search3", __FILE__, __LINE__);
        hash_item *item = hash_table[index];
        while (item != NULL) {
            log_write(LOG_ERR, "hash_search4", __FILE__, __LINE__);
            if (strcmp(item->key, key) == 0) {
                log_write(LOG_ERR, "hash_search found", __FILE__, __LINE__);
                return item;
            }
            item = item->next;
        }
    }
    log_write(LOG_ERR, "hash_search end", __FILE__, __LINE__);
    return NULL;
}

int hash_add(hash_item *item) {
    int ret = 0;
    log_write(LOG_ERR, "hashlock", __FILE__, __LINE__);
    if (item != NULL) {
         pthread_mutex_lock(&t_mutex_hash);
        log_write(LOG_ERR, "hash_add", __FILE__, __LINE__);
        unsigned int index = BKDRHash(item->key) % hash_size;
        hash_item *found_item = hash_table[index];
        hash_item *prev = NULL;

        log_write(LOG_ERR, "hash_add1", __FILE__, __LINE__);
        while (found_item != NULL) {
            log_write(LOG_ERR, "hash_add---", __FILE__, __LINE__);
            prev = found_item;
            log_write(LOG_ERR, "hash_add---1", __FILE__, __LINE__);
            found_item = found_item->next;
            log_write(LOG_ERR, "hash_add---2", __FILE__, __LINE__);
        }

        log_write(LOG_ERR, "hash_add2", __FILE__, __LINE__);
        if (prev != NULL) {
            prev->next = item;
        } else {
            hash_table[index] = item;
        }
        ret = 1;
        pthread_mutex_unlock(&t_mutex_hash);
    }    
    return ret;
}

int hash_del(const char *key, int fd) {
    
    int ret = 0;
    log_write(LOG_ERR, "hash_del", __FILE__, __LINE__);
    if (key != NULL) {
        pthread_mutex_lock(&t_mutex_hash);
        log_write(LOG_ERR, "hash_del1", __FILE__, __LINE__);
        unsigned int index = BKDRHash(key) % hash_size;
        hash_item *item = hash_table[index];
        hash_item *prev = NULL;
        while (item != NULL) {
            if (strcmp(item->key, key) == 0 && (int)item->data==fd) {
                if (prev != NULL) {
                    prev->next = item->next;
                    log_write(LOG_DEBUG, "hash_del", __FILE__, __LINE__);
                } else {
                    hash_table[index] = item->next;
                }
                free(item);
                ret = 1;
                break;
            }

            prev = item;
            item = item->next;
        }
        pthread_mutex_unlock(&t_mutex_hash);
    }
    log_write(LOG_ERR, "hash_del end", __FILE__, __LINE__);
    return ret;
}
