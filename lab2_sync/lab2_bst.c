/*
*	Operating System Lab
*	    Lab2 (Synchronization)
*	    Student id : 
*	    Student name : 
*
*   lab2_bst.c :
*       - thread-safe bst code.
*       - coarse-grained, fine-grained lock code
*
*   Implement thread-safe bst for coarse-grained version and fine-grained version.
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <string.h>

#include "lab2_sync_types.h"

/*
 * TODO
 *  Implement funtction which traverse BST in in-order
 *  
 *  @param lab2_tree *tree  : bst to print in-order. 
 *  @return                 : status (success or fail)
 */
int lab2_node_print_inorder(lab2_tree *tree) {
    lab2_node *p = tree->root;

    if(tree->root == NULL) {
        return 0;
    }
 
    if(p != NULL) {
        p=p->left;
        if(!p) {
            return 0;
        }
        tree->root = p;
        lab2_node_print_inorder(tree);

        p=p->right;
        if(!p) {
            return 0;
        }
        tree->root = p;
        lab2_node_print_inorder(tree);

    }
    return 0;
}

/*
 * TODO
 *  Implement function which creates struct lab2_tree
 *  ( refer to the ./include/lab2_sync_types.h for structure lab2_tree )
 * 
 *  @return                 : bst which you created in this function.
 */
lab2_tree *lab2_tree_create() {
    lab2_tree *tree = (lab2_tree *)malloc(sizeof(lab2_tree));
    tree->root = NULL;
    return tree;
}

/*
 * TODO
 *  Implement function which creates struct lab2_node
 *  ( refer to the ./include/lab2_sync_types.h for structure lab2_node )
 *
 *  @param int key          : bst node's key to creates
 *  @return                 : bst node which you created in this function.
 */
lab2_node * lab2_node_create(int key) {
    lab2_node *node = (lab2_node *)malloc(sizeof(lab2_node));
    pthread_mutex_init(&(node->mutex), NULL);
    node->key = key;
    node->left = 0;
    node->right = 0;
    return node; 
}

/* 
 * TODO
 *  Implement a function which insert nodes from the BST. 
 *  
 *  @param lab2_tree *tree      : bst which you need to insert new node.
 *  @param lab2_node *new_node  : bst node which you need to insert. 
 *  @return                 : satus (success or fail)
 */
int lab2_node_insert(lab2_tree *tree, lab2_node *new_node){
    lab2_node *p = tree->root;
    lab2_node *q = NULL;

    while(p != NULL) {
        q = p;
        if(new_node->key == p->key) {
            return 0;
        }
        if(new_node->key < p->key) {
            p = p->left;
        } 
        else {
            p = p->right;
        }
    }
    if (q == NULL) {
        tree->root = new_node;
        return 0;
    }

    if(tree->root == NULL) {
        tree->root = new_node;
    } else if(new_node->key < q->key) {
        q->left = new_node;
    } else {
        q->right = new_node;
    }
    return 0;
}

/* 
 * TODO
 *  Implement a function which insert nodes from the BST in fine-garined manner.
 *
 *  @param lab2_tree *tree      : bst which you need to insert new node in fine-grained manner.
 *  @param lab2_node *new_node  : bst node which you need to insert. 
 *  @return                     : status (success or fail)
 */
int lab2_node_insert_fg(lab2_tree *tree, lab2_node *new_node){
    lab2_node *p = tree->root;
    lab2_node *q = NULL;

    while(p != NULL) {
        q = p;
        if(new_node->key == p->key) {
            return 0;
        }
        if(new_node->key < p->key) {
            pthread_mutex_lock(&p->mutex);
            p = p->left;
            pthread_mutex_unlock(&p->mutex);
        } 
        else {
            prhread_mutex_lock(&p->mutex);
            p = p->right;
            pthread_mutex_unlock(&p->mutex);
        }
    }
    if (q == NULL) {
        pthread_mutex_lock(&q->mutex);
        tree->root = new_node;
        pthred_mutex_unlock(&q->mutex);
        return 0;
    }
    pthread_mutex_lock(&tree->root->mutex);
    if(tree->root == NULL) {
        tree->root = new_node;
    } else if(new_node->key < q->key) {
        q->left = new_node;
    } else {
        q->right = new_node;
    }
    pthred_mutex_unlock(&tree->root->mutex);
    return 0;
}

/* 
 * TODO
 *  Implement a function which insert nodes from the BST in coarse-garined manner.
 *
 *  @param lab2_tree *tree      : bst which you need to insert new node in coarse-grained manner.
 *  @param lab2_node *new_node  : bst node which you need to insert. 
 *  @return                     : status (success or fail)
 */
int lab2_node_insert_cg(lab2_tree *tree, lab2_node *new_node){
    lab2_node *p = tree->root;
    lab2_node *q = NULL;

    pthread_mutex_lock(&tree->root->mutex);
    while(p != NULL) {
        q = p;
        if(new_node->key == p->key) {
            return 0;
        }
        if(new_node->key < p->key) {
            p = p->left;
        } 
        else {
            p = p->right;
        }
    }
    if (q == NULL) {
        tree->root = new_node;
        return 0;
    }

    if(tree->root == NULL) {
        tree->root = new_node;
    } else if(new_node->key < q->key) {
        q->left = new_node;
    } else {
        q->right = new_node;
    }
    return 0;
    pthread_mutex_unlock(&tree->root->mutex);
}

/* 
 * TODO
 *  Implement a function which remove nodes from the BST.
 *
 *  @param lab2_tree *tree  : bst tha you need to remove node from bst which contains key.
 *  @param int key          : key value that you want to delete. 
 *  @return                 : status (success or fail)
 */
int lab2_node_remove(lab2_tree *tree, int key) {
    // You need to implement lab2_node_remove function.
}

/* 
 * TODO
 *  Implement a function which remove nodes from the BST in fine-grained manner.
 *
 *  @param lab2_tree *tree  : bst tha you need to remove node in fine-grained manner from bst which contains key.
 *  @param int key          : key value that you want to delete. 
 *  @return                 : status (success or fail)
 */
int lab2_node_remove_fg(lab2_tree *tree, int key) {
    // You need to implement lab2_node_remove_fg function.
}


/* 
 * TODO
 *  Implement a function which remove nodes from the BST in coarse-grained manner.
 *
 *  @param lab2_tree *tree  : bst tha you need to remove node in coarse-grained manner from bst which contains key.
 *  @param int key          : key value that you want to delete. 
 *  @return                 : status (success or fail)
 */
int lab2_node_remove_cg(lab2_tree *tree, int key) {
    // You need to implement lab2_node_remove_cg function.
}


/*
 * TODO
 *  Implement function which delete struct lab2_tree
 *  ( refer to the ./include/lab2_sync_types.h for structure lab2_node )
 *
 *  @param lab2_tree *tree  : bst which you want to delete. 
 *  @return                 : status(success or fail)
 */
void lab2_tree_delete(lab2_tree *tree) {
    // You need to implement lab2_tree_delete function.
}

/*
 * TODO
 *  Implement function which delete struct lab2_node
 *  ( refer to the ./include/lab2_sync_types.h for structure lab2_node )
 *
 *  @param lab2_tree *tree  : bst node which you want to remove. 
 *  @return                 : status(success or fail)
 */
void lab2_node_delete(lab2_node *node) {
    // You need to implement lab2_node_delete function.
}

