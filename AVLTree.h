//
// Created by Robert on 2017-12-20.
//
#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;
#ifndef LAB3_AVLTREE_H
#define LAB3_AVLTREE_H

template <typename Key, typename Info>
class AVLTree {
private:
    struct Node{
        Key key;
        Info info;
        Node *lnext;
        Node *rnext;
        int bfactor;
        Node(Key key, Info info){this->key=key,this->info=info,lnext=NULL,rnext=NULL,bfactor=0;};
    };
    void destroy(Node* &p);
    bool deleteNode(Node *toDel, Node *toDelTrail);
    void inorder(Node *p,Node* first) const ;
    void graph(AVLTree<Key, Info>::Node* const & subtreeRoot, int x) const;
    void insertIntoAVL(Node* &root,Node* newNode, bool& taller);

    int size(Node *n) const;
    template<typename K, typename I>
    friend std::ostream& operator<<(std::ostream& os, const AVLTree<K, I>& rhs);
    int max(int x, int y) const;

    void rotateToLeft(Node* &root);
    void rotateToRight(Node* &root);
    void balanceFromLeft(Node* &root);
    void balanceFromRight(Node* &root);

protected:
    Node *root;
public:

    AVLTree();
    ~AVLTree();
    bool isEmpty();

    bool remove(const Key& key);
    int rebalance(Node*& subtree);
    Node* getRoot(){ return root;};


    void insert(const Key& key, const Info& info);
    void printInorder(std::stringstream& st, Node *n) const;


    const Info&operator[](const Key& key) const ;
    Info&operator[](const Key& key);

    bool keyExists(const Key& key) const;
    void clear();
    void graph() const {graph(root,0);};
    void inorderTraversal()const;

    int size() const;

};


template<typename Key, typename Info>
std::ostream& operator<<(std::ostream& os, const AVLTree<Key, Info> &x)
{
    std::stringstream str;
    str<<"{";
    x.printInorder(str, x.root);
    std::string s(str.str());
    if(x.root!=NULL){
        s.pop_back(); s.pop_back();
    }
    s.append("}");
    os<<s;
    return os;
}

template<typename Key, typename Info>
void AVLTree<Key, Info>::printInorder(std::stringstream& str, Node *node) const
{
    if(node!=NULL){
        printInorder(str, node->lnext);
        str<<node->key<<": "<<node->info<<", ";
        printInorder(str, node->rnext);
    }
}

template<typename Key, typename Info>
int AVLTree<Key, Info>::rebalance(Node* &sub) {
    if(sub==NULL) {
        return 0;
    }
    int lheight = rebalance(sub->lnext);
    int rheight = rebalance(sub->rnext);

    sub->bfactor = rheight - lheight;
    if(sub->bfactor == 2 && sub->rnext->bfactor == 0) {
        rotateToLeft(sub);
        return rebalance(sub);
    }
    if(sub->bfactor == -2 && sub->lnext->bfactor == 0) {
        rotateToRight(sub);
        return rebalance(sub);
    }
    if(sub->bfactor == 2 && sub->rnext->bfactor == 1) {
        rotateToLeft(sub);
        return rebalance(sub);
    }
    if(sub->bfactor == 2 && sub->rnext->bfactor == -1) {
        rotateToRight(sub->rnext);
        rotateToLeft(sub);
        return rebalance(sub);
    }
    if(sub->bfactor == -2 && sub->lnext->bfactor == -1) {
        rotateToRight(sub);
        return rebalance(sub);
    }
    if(sub->bfactor == -2 && sub->lnext->bfactor == 1) {
        rotateToLeft(sub->lnext);
        rotateToRight(sub);
        return rebalance(sub);
    }
    return std::max(lheight, rheight) + 1;
}

template<typename Key, typename Info>
bool AVLTree<Key, Info>::deleteNode(Node *toDel, Node *toDelTrail)
{
    if(toDel->lnext && toDel->rnext) {
        Node* tmp = toDel->rnext;
        toDelTrail = toDel;
        while (tmp->lnext) {
            toDelTrail = tmp;
            tmp = tmp->lnext;
        }
        toDel->key = tmp->key;
        toDel->info = tmp->info;
        toDel = tmp;
    }

    Node* subtree = toDel->lnext;
    if(!subtree)
        subtree = toDel->rnext;
    if(!toDelTrail)
        root = subtree;
    else if(toDelTrail->lnext == toDel)
        toDelTrail->lnext = subtree;
    else
        toDelTrail->rnext = subtree;

    delete toDel;
    rebalance(root);
    return true;
}

template<typename Key, typename Info>
bool AVLTree<Key, Info>::remove(const Key &key)
{
    Node* toDel = root;
    Node* toDelTrail = nullptr;

    while(toDel) {
        if(key < toDel->key) {
            toDelTrail = toDel;
            toDel = toDel->lnext;
        } else if(key > toDel->key) {
            toDelTrail = toDel;
            toDel = toDel->rnext;
        } else if(toDel->key == key) {
            return deleteNode(toDel, toDelTrail);
        }
    }
    return false;
}
/*
template<typename Key, typename Info>
void AVLTree<Key, Info>::copyTree(AVLTree::Node *&r, AVLTree::Node *o)
{
    if(!o){ r = nullptr;}
    else{
        r = new Node(o->key,o->info);
        r->bfactor=o->bfactor;
        copyTree(r->lnext, o->lnext);
        copyTree(r->rnext, o->rnext);
    }
}

template<typename Key, typename Info>
AVLTree<Key,Info> &AVLTree<Key, Info>::operator=(const AVLTree &rhs)
{
    if(&rhs == this){ return *this; }
    if(root){clear();}
    if(!rhs.root){ root = nullptr; }
    else{ copyTree(root, rhs.root); }
    return *this;
}*/

template<typename Key, typename Info>
int AVLTree<Key, Info>::size() const
{
    return size(root);
}

template<typename Key, typename Info>
int AVLTree<Key, Info>::size(Node *node) const
{
    int x=0;
    if(node!=NULL){
        x=size(node->lnext);
        x++;
        x=x+size(node->rnext);
    }
    return x;
}

template <typename Key, typename Info>
void AVLTree<Key,Info>::inorder(Node *p,Node* first) const {
    if (p != NULL)
    {
        inorder(p->lnext,first);
        if(p->key==first->key){
            cout << p->key << ": " << p->info;
        } else {
            cout << ", " << p->key << ": " << p->info;
        }
        inorder(p->rnext,first);
    }
}
template <typename Key, typename Info>
void AVLTree<Key,Info>::inorderTraversal() const {
    Node *curr;
    curr=root;
    if(curr==NULL){
        cout<<"{}"<<endl;
    }
    else{
        Node* first;
        first=firstNodeToPrint(root);
        cout<<"{";
        inorder(curr,first);
        cout<<"}";
    }
}
template <typename Key, typename Info>
void AVLTree<Key,Info>::rotateToLeft(Node *&root) {
    Node *curr;

    if(root==NULL){
        cerr<<"Error in tree" <<endl;
    } else if (root->rnext==NULL){
        cerr<<"No right subtree to rotate." <<endl;
    } else{
        curr=root->rnext;
        root->rnext=curr->lnext;

        curr->lnext=root;
        root=curr;
    }
}
template <typename Key, typename Info>
void AVLTree<Key,Info>::rotateToRight(Node *&root) {
    Node *curr;

    if (root==NULL){
        cerr<<"Tree is empty"<<endl;
    } else if (root->lnext==NULL){
        cerr<<"no left subtree to rotate"<<endl;
    } else{
        curr=root->lnext;
        root->lnext=curr->rnext;

        curr->rnext=root;
        root=curr;
    }
}
template <typename Key, typename Info>
void AVLTree<Key,Info>::balanceFromLeft(Node *&root) {
    Node *curr;
    Node *tmp;

    curr=root->lnext;

    switch (curr->bfactor){
        case -1:
            root->bfactor =0;
            curr->bfactor=0;
            rotateToRight(root);
            break;
        case 0:
            cerr<<"Cannot balance from the left."<<endl;
            break;
        case 1:
            tmp=curr->rnext;
            switch (tmp->bfactor){
                case -1:
                    root->bfactor=1;
                    curr->bfactor=0;
                    break;
                case 0:
                    root->bfactor=0;
                    curr->bfactor=0;
                    break;
                case 1:
                    root->bfactor=0;
                    curr->bfactor=-1;
            }
            tmp->bfactor=0;
            rotateToLeft(curr);
            root->lnext=curr;
            rotateToRight(root);
            break;
    }
}
template <typename Key, typename Info>
void AVLTree<Key,Info>::balanceFromRight(Node *&root) {
    Node *curr;
    Node *tmp;

    curr=root->rnext;

    switch (curr->bfactor){
        case 1:
            root->bfactor =0;
            curr->bfactor=0;
            rotateToLeft(root);
            break;
        case 0:
            cerr<<"Cannot balance from the left."<<endl;
            break;
        case -1:
            tmp=curr->lnext;
            switch (tmp->bfactor){
                case -1:
                    root->bfactor=0;
                    curr->bfactor=1;
                    break;
                case 0:
                    root->bfactor=0;
                    curr->bfactor=0;
                    break;
                case 1:
                    root->bfactor=-1;
                    curr->bfactor=0;
            }
            tmp->bfactor=0;
            rotateToRight(curr);
            root->rnext=curr;
            rotateToLeft(root);
            break;
    }
}
template <typename Key, typename Info>
void AVLTree<Key,Info>::insertIntoAVL(Node *&root, Node *newNode, bool &taller) {
    if (root == NULL) {
        root = newNode;
        taller = true;
    } else if (root->key == newNode->key) {
        cerr << "key already in tree can't add" << endl;
    } else if (root->key > newNode->key) {
        insertIntoAVL(root->lnext, newNode, taller);

        if (taller==true) {
            switch (root->bfactor) {
                case -1:
                    balanceFromLeft(root);
                    taller = false;
                    break;
                case 0:
                    root->bfactor = -1;
                    taller = true;
                    break;
                case 1:
                    root->bfactor = 0;
                    taller = false;
            }
        }
    } else {
            insertIntoAVL(root->rnext,newNode,taller);
            if (taller== true){
                switch (root->bfactor){
                    case -1:
                        root->bfactor=0;
                        taller= false;
                        break;
                    case 0:
                        root->bfactor=1;
                        taller= true;
                        break;
                    case 1:
                        balanceFromRight(root);
                        taller= false;
                }
            }
        }
}

template <typename Key, typename Info>
void AVLTree<Key,Info>::insert(const Key &key, const Info &info) {
    bool isTaller= false;
    Node *newNode= new Node(key,info);
    insertIntoAVL(root,newNode,isTaller);
}

template <typename Key, typename Info>
AVLTree<Key,Info>::AVLTree() {
    root=NULL;
}
template <typename Key, typename Info>
AVLTree<Key,Info>::~AVLTree() {
destroy(root);
}

template <typename Key, typename Info>
int AVLTree<Key,Info>::max(int x, int y) const {
    if(x>=y)
        return x;
    else
        return y;
}

template <typename Key, typename Info>
void AVLTree<Key,Info>::destroy(Node *&p) {
    if(p!=NULL){
        destroy(p->lnext);
        destroy(p->rnext);
        delete p;
        p=NULL;
    }
}

template <typename Key, typename Info>
void AVLTree<Key,Info>::clear() {
    destroy(root);
}

template <typename Key, typename Info>
bool AVLTree<Key,Info>::keyExists(const Key &key) const {
    Node*curr;
    bool found= false;

    if(root==NULL){
        cout<<"Tree is empty"<<endl;
    } else{
        curr=root;
        while (curr!=NULL && !found){
            if (curr->key == key)
                found = true;
            else if (curr->key > key)
                curr = curr ->lnext;
            else
                curr = curr ->rnext;
        }
    }
    return found;
}

template <typename Key, typename Info>
void AVLTree<Key,Info>::graph(AVLTree<Key, Info>::Node *const &subtreeRoot, int indent) const {
    if (subtreeRoot != NULL) {
        graph(subtreeRoot->rnext, indent + 8);
        std::cout << std::setw(indent) << " " << subtreeRoot->key << endl;
        graph(subtreeRoot->lnext, indent + 8);
    }
    else
        std::cout << std::setw(indent) << " " <<  std::endl;
}
template <typename Key, typename Info>
const Info& AVLTree<Key,Info>::operator[](const Key &key) const {
    bool found = keyExists(key);
    if (found) {
        Node *curr = root;
        while (curr != NULL) {
            if (curr->key == key)
                return curr->info;
            else if (curr->key > key)
                curr = curr->lnext;
            else
                curr = curr->rnext;
        }
    }
    throw std::range_error("no key");
}
template <typename Key, typename Info>
Info& AVLTree<Key,Info>::operator[](const Key &key) {
    bool found = keyExists(key);
    if (found) {
        Node *curr = root;
        while (curr != NULL) {
            if (curr->key == key)
                return curr->info;
            else if (curr->key > key)
                curr = curr->lnext;
            else
                curr = curr->rnext;
        }
    }
    throw std::range_error("no key");
}

#endif //LAB3_AVLTREE_H

