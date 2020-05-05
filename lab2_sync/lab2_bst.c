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

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/*
 * TODO
 *  Implement funtction which traverse BST in in-order
 *  
 *  @param lab2_tree *tree  : bst to print in-order. 
 *  @return                 : status (success or fail)
 */
int lab2_node_print_inorder(lab2_tree *tree) {
    lab2_node *p = tree->root;
    int count = 0;

    if(tree->root == NULL) {
        return -1;
    }
 
    if(p != NULL) {
        p = p->left;
        if(!p) {
            return 1;
        }
        tree->root = p;
        count=count+lab2_node_print_inorder(tree);

        p = p->right;
        if(!p) {
            return 1;
        }
        tree->root = p;
        count=count+lab2_node_print_inorder(tree);
    }
    return count;
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
    lab2_node *k = lab2_node_create(new_node->key);
    
    if(!tree->root) {
        tree->root = k;
        return 0;
    }
    
    lab2_node *p = tree->root;
    lab2_node *q = NULL;

    while(p != NULL) {
        q = p;
        if(new_node->key == p->key) {
            return -1;
        }
        if(new_node->key < p->key) {
            p = p->left;
        } 
        else {
            p = p->right;
        }
    }

    if(new_node->key < q->key) {
        q->left = k;
    } else {
        q->right = k;
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
    lab2_node *k = lab2_node_create(new_node->key);
    
    if(!tree->root) {
        tree->root = k;
        return 0;
    }

    lab2_node *p = tree->root;
    lab2_node *q = NULL;

    while(p != NULL) {
        q = p;
        if(new_node->key == p->key) {
            return -1;
        }
        if(new_node->key < p->key) {
            p = p->left;
        } 
        else {
            p = p->right;
        }
    }

    if(new_node->key < q->key) {
        pthread_mutex_lock(&q->mutex);
        q->left = k;
        pthread_mutex_unlock(&q->mutex);
    } else {
        pthread_mutex_lock(&q->mutex);
        q->right = k;
        pthread_mutex_unlock(&q->mutex);
    }
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
    lab2_node *k = lab2_node_create(new_node->key);
    
    if(!tree->root) {
        tree->root = k;
        return 0;
    }   
    
    pthread_mutex_lock(&tree->root->mutex);
    lab2_node *p = tree->root;
    lab2_node *q = NULL;

    while(p != NULL) {
        q = p;
        if(new_node->key == p->key) {
            pthread_mutex_unlock(&tree->root->mutex);
            return -1;
        }
        if(new_node->key < p->key) {
            p = p->left;
        } 
        else {
            p = p->right;
        }
    }

    if(new_node->key < q->key) {
        q->left = k;
    } else {
        q->right = k;
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
    
    if(!tree->root) {
        return -1;
    }

    lab2_node *p = tree->root;
    lab2_node *q = NULL;
    lab2_node *k = NULL;
    
    while((p!=NULL) && (p->key != key)) {
        q = p;
        if((p->left == NULL) && (p->right == NULL)) {
            return -1;
        }

        if(key < p->key) {
            if(p->left) {
                p = p->left;
            }
            else{
                return -1;
            }
        }
        else if(key > p->key) {
            if(p->right) {
                p = p->right;
            }
            else {
                return -1;
            }
        }
    }
    
    if((p->left == NULL) && (p->right == NULL)) {
        if(q->left == p) {
            q->left = NULL;
        }
        else if(q->right == p) {
            q->right = NULL;
        }
    }
    else if((p->left != NULL) && (p->right == NULL)) {
        if(q->left == p) {
            q->left = p->left;
        }
        else if(q->right == p) {
            q->right = p->left;
        }
    }
    else if((p->left == NULL) && (p->right != NULL)) {
        if(q->left == p) {
            q->left = p->right;
        }
        else if(q->right == p) {
            q->right = p->right;
        }
    }

    else if((p->left != NULL) && (p->right != NULL)) {
        k = p->left;
        while(k->right != NULL) {
            q = k;
            k = k->right;
        }
        if(k == p->left) {
            p->key = k->key;
            if(k->left != NULL) {
                p->left = k->left;
            }
            else {
                p->left = NULL;
            }
        }
        else {
            p->key = k->key;
            if(k->left != NULL) {
                q->right = k->left;
            }
            else {
                q->right = NULL;
            }
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
   
    if(!tree->root) {
		return -1;
    }

    lab2_node *p = tree->root;
    lab2_node *q = NULL;
    lab2_node *k = NULL; 

    while((p != NULL) && (p->key != key)) {
        q = p;
        if((p->left == NULL) && (p->right == NULL)) {
            return -1;
        }

        if(key < p->key) {
            if(p->left) {
                p = p->left;
            }
            else {
                return -1;
            }
        }
        else if(key > p->key) {
            if(p->right) {
                p = p->right;
            }
            else {
                return -1;
            }
        }
	}


    if((p->left == NULL) && (p->right == NULL)) {
        pthread_mutex_lock(&p->mutex);
        if(q->left==p) {
            q->left = NULL;
		}
		else if(q->right == p) {
			q->right = NULL;
		}
 		pthread_mutex_unlock(&p->mutex);
	}

    if((p->left != NULL) && (p->right == NULL)) {
		pthread_mutex_lock(&p->mutex);
		if(q->left == p) {
            q->left = p->left;
        }
        else if(q->right == p) {
            q->right = p->left;
        }
 		pthread_mutex_unlock(&p->mutex);
	}
       
    if((p->left == NULL) && (p->right != NULL)) {
		pthread_mutex_lock(&p->mutex);
        k = p;
        if(q->left == p){
            q->left = p->right;
        }
        else if(q->right == p) {
            q->right = p->right;
        }
		pthread_mutex_unlock(&p->mutex);
    }
    else if((p->left != NULL) && (p->right != NULL)) {
        k = p->left;
        while(k->right != NULL) {
            q = k;
            k = k->right;
        }
		pthread_mutex_lock(&p->mutex);
        if(k == p->left) {
            p->key = k->key;
            if(k->left != NULL) {
                p->left = k->left;
            } 
            else {
                p->left = NULL;
            }
        }
        else {
            p->key = k->key;
            if(k->left != NULL) {
                q->right = k->left;
            }
            else {
                q->right = NULL;
            }
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
   
    if(!tree->root) {
		return -1;
    }
    pthread_mutex_lock(&tree->root->mutex);
    lab2_node *p = tree->root;
    lab2_node *q = NULL;
    lab2_node *k = NULL; 

    while((p != NULL) && (p->key != key)) {
        q = p;
        if((p->left == NULL) && (p->right == NULL)) {
            pthread_mutex_unlock(&tree->root->mutex);
            return -1;
        }

        if(key < p->key) {
            if(p->left) {
                p = p->left;
            }
            else {
                pthread_mutex_unlock(&tree->root->mutex);
                return -1;
            }
        }
        else if(key > p->key) {
            if(p->right) {
                p = p->right;
            }
            else {
                pthread_mutex_unlock(&tree->root->mutex);
                return -1;
            }
        }
	}


    if((p->left == NULL) && (p->right == NULL)) {
        if(q->left == p) {
            q->left = NULL;
		}
		else if(q->right == p) {
			q->right = NULL;
		}
	}

    if((p->left != NULL) && (p->right == NULL)) {
		if(q->left == p) {
            q->left = p->left;
        }
        else if(q->right == p) {
            q->right = p->left;
        }
	}
       
    if((p->left == NULL) && (p->right != NULL)) {
        if(q->left == p){
            q->left = p->right;
        }
        else if(q->right == p) {
            q->right = p->right;
        }
    }
    else if((p->left != NULL) && (p->right != NULL)) {
        k = p->left;
        while(k->right != NULL) {
            q = k;
            k = k->right;
        }
        if(k == p->left) {
            p->key = k->key;
            if(k->left != NULL) {
                p->left = k->left;
            } 
            else {
                p->left = NULL;
            }
        }
        else {
            p->key = k->key;
            if(k->left != NULL) {
                q->right = k->left;
            }
            else {
                q->right = NULL;
            }
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

   node->key = 0;
   node->left = NULL;
   node->right = NULL;
   free(node);
}

