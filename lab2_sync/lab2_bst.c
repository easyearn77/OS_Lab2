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
    node->left = NULL;
    node->right = NULL;
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

    if(tree->root == NULL) {
        tree->root = new_node;
        return 0;
    }

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

    //p가 null이 되고나서 빠져 나옴
    //q가 가리키는 인덱스에 새로운 노드를 삽입
    if (q == NULL) {
        tree->root = new_node;
        return 0;
    }

    if(new_node->key < q->key) {
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

    if(tree->root == NULL) {
        pthread_mutex_lock(&tree->root->mutex);
        tree->root = new_node;
        pthread_mutex_unlock(&tree->root->mutex);
        return 0;
    }

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
    if(new_node->key < q->key) {
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

    if(tree->root == NULL) {
        tree->root = new_node;
        return 0;
    }

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

    if(new_node->key < q->key) {
        q->left = new_node;
    } else {
        q->right = new_node;
    }

    pthread_mutex_unlock(&tree->root->mutex);

    return 0;
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
    lab2_node *p = tree->root;
    lab2_node *q = NULL;
    lab2_node *k = NULL;

    while((p != NULL) && (p->key != key)) {
        q = p;
        if(key < p->key) {
            p = p->left;
        } else {
            p = p->right;
        }
    }

    if((p->left == NULL) && (p->right == NULL)) {
        if(q->left == p) {
            q->left = NULL;
        } else {
            q->right = NULL;    
        }
        lab2_node_delete(p);
    }

    if((p->left != 0) && (p->right == 0)) {
        k = p;
        if(q->left == p) {
            q->left = p->left;
        } else if(q->right == p){
            q->right = p->left;
        }
        p = p->left;
        lab2_node_delete(k);
        p->left = NULL;
        p->right = NULL;
    }

    if((p->left == 0) && (p->right != 0)) {
        k = p;
        if(q->left == p) {
            q->left = p->right;
        } else if(q->right == p){
            q->right = p->right;
        }
        p = p->right;
        lab2_node_delete(k);
        p->left = NULL;
        p->right =NULL;
    } else if ((p->left != 0) && (p->right != 0)) {
        q = p;
        k = p->left;
        while(k->right != 0) {
            q = k;
            k = k->right;
        }

        if(q == p) {
            p->key = k->key;
            if(k->left != 0) {
                p->left = k->left;
            } else {
                p->left = NULL;
            }
            lab2_node_delete(k);
        } else {
                p->key = k->key;
                if(k->left != 0) {
                    q->right = k->left;
                } else {
                    q->right = NULL;
                }
                lab2_node_delete(k);
        }
            
    }
    return 0;
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
    lab2_node *p = tree->root;
    lab2_node *q = NULL;
    lab2_node *k = NULL;    

    while((p != NULL) && (p->key != key)) {
        q = p;
        if(key < p->key) {
            p = p->left;
        } else {
            p = p->right;
        }
    }
    
    if((p->left == NULL) && (p->right == NULL)) {
        pthread_mutex_lock(&p->mutex);
        if(q->left == p) {
            q->left = NULL;
        } else {
            q->right = NULL;    
        }
        lab2_node_delete(p);
        pthread_mutex_unlock(&p->mutex);
    }

    if((p->left != 0) && (p->right == 0)) {
        pthread_mutex_lock(&p->mutex);
        k = p;
        if(q->left == p) {
            q->left = p->left;
        } else if(q->right == p) {
            q->right = p->left;
        }

        p = p->left;
        lab2_node_delete(k);
        p->left = NULL;
        p->right = NULL;
        pthread_mutex_unlock(&p->mutex);
    }

    if((p->left == 0) && (p->right != 0)) {
        pthread_mutex_lock(&p->mutex);
        k = p;
        if(q->left == p) {
            q->left = p->right;
        } else {
            q->right = p->right;
        }
        p = p->right;
        lab2_node_delete(k);
        p->left = NULL;
        p->right =NULL;
        pthread_mutex_unlock(&p->mutex);
    } else if ((p->left != 0) && (p->right != 0)) {
        pthread_mutex_lock(&p->mutex);
        q = p;
        k = p->left;
        while(k->right != 0) {
            q = k;
            k = k->right;
        }

        if(q == p) {
            p->key = k->key;
            if(k->left != 0) {
                p->left = k->left;
            } else {
                p->left = NULL;
            }
            lab2_node_delete(k);
        } else {
            p->key = k->key;
            if(k->left != 0) {
                q->right = k->left;
            } else {
                q->right = NULL;
            }
            lab2_node_delete(k);
        }
        pthread_mutex_unlock(&p->mutex);
    }
    return 0;
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
    lab2_node *p = tree->root;
    lab2_node *q = NULL;
    lab2_node *k = NULL;
    
    pthread_mutex_lock(&tree->root->mutex);
    while((p != NULL) && (p->key != key)) {
        q = p;
        if(key < p->key) {
            p = p->left;
        } else {
            p = p->right;
        }
    }

    if((p->left == NULL) && (p->right == NULL)) {
        if(q->left == p) {
            q->left = NULL;
        } else {
            q->right = NULL;    
        }
        lab2_node_delete(p);
    }

    if((p->left != 0) && (p->right == 0)) {
        k = p;
        if(q->left == p) {
            q->left = p->left;
        } else if(q->right == p){
            q->right = p->left;
        }
        p = p->left;
        lab2_node_delete(k);
        p->left = NULL;
        p->right = NULL;
    }

    if((p->left == 0) && (p->right != 0)) {
        k = p;
        if(q->left == p) {
            q->left = p->right;
        } else if(q->right == p){
            q->right = p->right;
        }
        p = p->right;
        lab2_node_delete(k);
        p->left = NULL;
        p->right =NULL;
    } else if ((p->left != 0) && (p->right != 0)) {
        q = p;
        k = p->left;
        while(k->right != 0) {
            q = k;
            k = k->right;
        }

        if(q == p) {
            p->key = k->key;
            if(k->left != 0) {
                p->left = k->left;
            } else {
                p->left = NULL;
            }
            lab2_node_delete(k);
        } else {
                p->key = k->key;
                if(k->left != 0) {
                    q->right = k->left;
                } else {
                    q->right = NULL;
                }
                lab2_node_delete(k);
        }
            
    }
    pthread_mutex_unlock(&tree->root->mutex);
    return 0;
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
   
    if(tree->root != NULL) {
        lab2_node_delete(tree->root);
    }

    tree->root = NULL;
    free(tree);
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

   if (node->left != NULL) {
       lab2_node_delete(node->left);
   }

   if (node->right != NULL) {
       lab2_node_delete(node->right);
   }

   node->key = NULL;
   node->left = NULL;
   node->right = NULL;
   free(node);
}

