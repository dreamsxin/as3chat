/* 
 * File:   hash.h
 * Author: Administrator
 *
 * Created on 2009��12��5��, ����11:05
 */

#ifndef _HASH_H
#define	_HASH_H

#ifdef	__cplusplus
extern "C" {
#endif

    typedef struct hash_item {
        char *key;
        void *data;
        struct hash_item *next;
    } hash_item;

    hash_item **hash_table;
    unsigned int BKDRHash(char *str);
    int hash_create(int size);
    void hash_destroy();
    hash_item *hash_search(char *key);
    int hash_add(hash_item *item);
    int hash_del(char *key);
#ifdef	__cplusplus
}
#endif

#endif	/* _HASH_H */

