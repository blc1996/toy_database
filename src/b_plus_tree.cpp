/**
 * b+ tree implementation for indexing
 * modified based on CS225 b-tree lab
 */

#include <algorithm>
#include "b_plus_tree.h"

using std::vector;

/**
 * Constructs a default, order 64 BPlusTree.
 */
template <class K, class V>
BPlusTree<K, V>::BPlusTree()
{
    root = NULL;
    order = 64; //default order, can be changed
}

/**
 * Constructs a BPlusTree with the specified order. The minimum order allowed
 * is order 3.
 * @param order The order of the constructed BPlusTree.
 */
template <class K, class V>
BPlusTree<K, V>::BPlusTree(unsigned int order)
{
    root = NULL;
    this->order = order < 3 ? 3 : order;
}

/**
 * Constructs a BPlusTree as a deep copy of another.
 * @param other The BPlusTree to copy.
 */
template <class K, class V>
BPlusTree<K, V>::BPlusTree(const BPlusTree& other)
{
    clear(root);
    root = copy(other.root);
}

/**
 * Private recursive version of the copy function.
 * @param subroot A pointer to the current node being copied.
 */
template <class K, class V>
typename BPlusTree<K, V>::BPlusTreeNode* BPlusTree<K, V>::copy(const BPlusTreeNode* subroot)
{
    if (subroot == NULL) {
        return NULL;
    }

    BPlusTreeNode* new_node = new BPlusTreeNode(subroot);
    for (auto& child : subroot->children) {
        new_node->children.push_back(copy(child));
    }
    return new_node;
}

/**
 * Performs checks to make sure the BPlusTree is valid. Specifically
 * it will check to make sure that an in-order traversal of the tree
 * will result in a sorted sequence of keys. Also verifies that each
 * BPlusTree node doesn't have more nodes than its order.
 * @return true if it satisfies the conditions, false otherwise.
 */
template <class K, class V>
bool BPlusTree<K, V>::is_valid(unsigned int order /* = 64 */) const
{
    if (root == NULL)
        return true;
    vector<K> keys;
    return is_valid(root, keys, order)
           && std::is_sorted(keys.begin(), keys.end());
}

/**
 * Private recursive version of the is_valid function.
 * @param subroot A pointer to the current node being checked for
 * validity.
 * @return true if the node is a valid BPlusTreeNode, false otherwise.
 */
template <class K, class V>
bool BPlusTree<K, V>::is_valid(const BPlusTreeNode* subroot, vector<K>& keys,
                           unsigned int order) const
{
    if (subroot->elements.size() >= order) {
        return false;
    }

    auto first = subroot->elements.begin();
    auto last = subroot->elements.end();
    bool ret = subroot->children.size() == subroot->elements.size() + 1;
    if (!subroot->is_leaf) {
        auto curr_child = subroot->children.begin();
        ret &= is_valid(*curr_child, keys, order);
        curr_child++;
        for (auto elem = first; ret && elem != last; elem++) {
            keys.push_back(*elem);
            ret &= is_valid(*curr_child, keys, order);
            curr_child++;
        }
    } else {
        keys.insert(keys.end(), first, last);
        ret = true;
    }
    return ret;
}

/**
 * Private recursive version of the clear function.
 * @param subroot A pointer to the current node being cleared.
 */
template <class K, class V>
void BPlusTree<K, V>::clear(BPlusTreeNode* subroot)
{
    if (!subroot->is_leaf) {
        for (auto child : subroot->children) {
            clear(child);
        }
    }
    delete subroot;
}

/**
 * Destroys a BPlusTree.
 */
template <class K, class V>
BPlusTree<K, V>::~BPlusTree()
{
    clear();
}

/**
 * Assignment operator for a BPlusTree.
 * @param rhs The BPlusTree to assign into this one.
 * @return The copied BPlusTree.
 */
template <class K, class V>
const BPlusTree<K, V>& BPlusTree<K, V>::operator=(const BPlusTree& rhs)
{
    if (this != &rhs) {
        clear(root);
        root = copy(rhs.root);
    }
    return *this;
}

/**
 * Clears the BPlusTree of all data.
 */
template <class K, class V>
void BPlusTree<K, V>::clear()
{
    if (root != NULL) {
        clear(root);
        root = NULL;
    }
}


/**
 * @file BPlusTree.cpp
 * Implementation of a B-tree class which can be used as a generic dictionary
 * (insert-only). Designed to take advantage of caching to be faster than
 * standard balanced binary search trees.
 *
 * @author Matt Joras
 * @date Winter 2013
 */

using std::vector;

/**
 * Finds the value associated with a given key.
 * @param key The key to look up.
 * @return The value (if found), the default V if not.
 */
template <class K, class V>
typename BPlusTree<K, V>::iterator BPlusTree<K, V>::find(const K& key)
{
    // std::cout<<*root;
    return root == NULL ? this->end() : find(root, key);
}

/**
 * Private recursive version of the find function.
 * @param subroot A reference of a pointer to the current BPlusTreeNode.
 * @param key The key we are looking up.
 * @return The value (if found), the default V if not.
 */
template <class K, class V>
typename BPlusTree<K, V>::iterator BPlusTree<K, V>::find(BPlusTreeNode* subroot, const K& key)
{
    // insertion_idx is a helper function that finds the index in the key array for a given key
    size_t first_larger_idx = insertion_idx(subroot->elements, key);

    //recursively find the key
    if(!subroot->is_leaf){
        if(subroot->elements[first_larger_idx] == key){
            first_larger_idx++;
        }
        return find(subroot->children[first_larger_idx], key);
    }else if(subroot->elements.size() > first_larger_idx && subroot->elements[first_larger_idx] == key){
        return this->begin(subroot, first_larger_idx);
    }else{
        return this->end();
    }
}

/**
 * Inserts a key and value into the BPlusTree. If the key is already in the
 * tree do nothing.
 * @param key The key to insert.
 * @param value The value to insert.
 */
template <class K, class V>
void BPlusTree<K, V>::insert(const K& key, const V& value)
{
    /* Make the root node if the tree is empty. */
    if (root == NULL) {
        root = new BPlusTreeNode(true, order);
    }
    insert(root, key, value);
    // std::cout<<*root<<std::endl;
    /* Increase height by one by tossing up one element from the old
     * root node. */
    if (root->elements.size() >= order) {
        BPlusTreeNode* new_root = new BPlusTreeNode(false, order);
        new_root->children.push_back(root);
        split_child(new_root, 0);
        root = new_root;
    }
}

/**
 * Splits a child node of a BPlusTreeNode. Called if the child became too
 * large.
 * @param parent The parent whose child we are trying to split.
 * @param child_idx The index of the child in its parent's children
 * vector.
 */
template <class K, class V>
void BPlusTree<K, V>::split_child(BPlusTreeNode* parent, size_t child_idx)
{
    /* The child we want to split. */
    BPlusTreeNode* child = parent->children[child_idx];
    /* The "left" node is the old child, the right child is a new node. */
    BPlusTreeNode* new_left = child;
    BPlusTreeNode* new_right = new BPlusTreeNode(new_left->is_leaf, order);

    // find the index of the element that is going to be lift up
    size_t mid_elem_idx = (new_left->elements.size() - 1) / 2;
    size_t mid_child_idx = new_left->children.size() / 2;

    /* Iterator for where we want to insert the new child. */
    auto child_itr = parent->children.begin() + child_idx + 1;
    /* Iterator for where we want to insert the new element. */
    auto elem_itr = parent->elements.begin() + child_idx;
    /* Iterator for the middle element. */
    auto mid_elem_itr = new_left->elements.begin() + mid_elem_idx;
    /* Iterator for the middle child. */
    auto mid_child_itr = new_left->children.begin() + mid_child_idx;

    parent->elements.insert(elem_itr, new_left->elements[mid_elem_idx]);

    if(new_left->is_leaf){
        new_right->elements.assign(mid_elem_itr, new_left->elements.end());
        new_right->values.assign(new_left->values.begin() + mid_elem_idx, new_left->values.end());
        //connect to next leaf node
        new_right->next = new_left->next;
        new_left->next = new_right;
    }else{
        new_right->elements.assign(mid_elem_itr + 1, new_left->elements.end());
        new_right->children.assign(mid_child_itr, child->children.end());
    }
    parent->children.insert(child_itr, new_right);
    new_left->elements.erase(mid_elem_itr, new_left->elements.end());
    new_left->children.erase(mid_child_itr, new_left->children.end());
}

/**
 * Private recursive version of the insert function.
 * @param subroot A reference of a pointer to the current BPlusTreeNode.
 * @param pair The DataPair to be inserted.
 * Note: Original solution used std::lower_bound, but making the students
 * write an equivalent seemed more instructive.
 */
template <class K, class V>
void BPlusTree<K, V>::insert(BPlusTreeNode* subroot, const K& key, const V& value)
{
    /* There are two cases to consider.
     * If the subroot is a leaf node and the key doesn't exist subroot, we
     * should simply insert the pair into subroot.
     * Otherwise (subroot is not a leaf and the key doesn't exist in subroot)
     * we need to figure out which child to insert into and call insert on it.
     * After this call returns we need to check if the child became too large
     * and thus needs to be split to maintain order.
     */

    if(subroot->is_leaf && subroot->elements.size() == 0){
        subroot->elements.insert(subroot->elements.begin(), key);
        subroot->values.insert(subroot->values.begin(), value);
        return;
    }

    size_t first_larger_idx = insertion_idx(subroot->elements, key);

    if(first_larger_idx < subroot->elements.size() &&
       subroot->elements[first_larger_idx] == key){
        return;
    }else if(subroot->is_leaf){
        subroot->elements.insert(subroot->elements.begin() + first_larger_idx, key);
        subroot->values.insert(subroot->values.begin() + first_larger_idx, value);
    }else{
        insert(subroot->children[first_larger_idx], key, value);
    }

    if( !subroot->is_leaf &&
        subroot->children[first_larger_idx]->elements.size() >= order){
        split_child(subroot, first_larger_idx);
    }
}

template <class K, class V>
void BPlusTree<K, V>::erase(const K& key){
    if (root == NULL) {
        /* tree is empty. abandon */
        return;
    }

    erase(root, key);

    /* Decrease height by one by using the next child as root */
    if (root->elements.size() == 0) {
        BPlusTreeNode* temp = root->children[0];
        delete root;
        root = temp;
    }
}

template <class K, class V>
void BPlusTree<K, V>::merge_child(BPlusTreeNode* parent, size_t child_idx)
{
    const unsigned int MIN_ENTRY = (order / 2) - 1;
    auto child_iter = parent->chidren.begin() + child_idx;
    // check the left one
    if(child_idx > 0){
        auto left_sibling = child_iter - 1;
        if(left_sibling->elements.size() > MIN_ENTRY){
            K temp_key = left_sibling->elements.back();
            left_sibling->elements.pop_back();
            child_iter->elements.insert(child_iter->elements.begin(), temp_key);
            // update the parent node
            parent->elements[child_idx  - 1] = temp_key;
            // if is leaf node, alse update value
            if(left_sibling->is_leaf){
                V temp_value = left_sibling->values.back();
                left_sibling->values.pop_back();
                child_iter->values.insert(child_iter->values.begin(), temp_value);
            }
            // done!
            return;
        }
    }
    // check the right one
    if(child_idx < parent->elements.size() - 1){
        auto right_sibling = child_iter + 1;
        if(right_sibling->elements.size() > MIN_ENTRY){
            K temp_key = right_sibling->elements.front();
            right_sibling->elements.erase(right_sibling->elements.begin());
            child_iter->elements.push_back(temp_key);
            // update the parent node
            parent->elements[child_idx] = right_sibling->elements.front();
            // if is leaf node, alse update value
            if(right_sibling->is_leaf){
                V temp_value = right_sibling->values.front();
                right_sibling->values.erase(right_sibling->elements.begin());
                child_iter->values.push_back(temp_value);
            }
            // done!
            return;
        }
    }

    // have to merge with siblings now
    if(child_idx > 0){
        // merge with left sibling
        auto left_sibling = child_iter - 1;
        left_sibling->elements.insert(left_sibling->elements.end(), child_iter->elements.begin(), child_iter->elements.end());
        if(child_iter->is_leaf){
            left_sibling->values.insert(left_sibling->values.end(), child_iter->values.begin(), child_iter->values.end());
        }
        delete parent->children[child_idx];
        parent->elements.erase(parent->elements.begin() + child_idx - 1);
        parent->children.erase(parent->elements.begin() + child_idx);
    }else if(child_idx < parent->elements.size() - 1){
        // merge with right sibling
        auto right_sibling = child_iter + 1;
        child_iter->elements.insert(child_iter->elements.end(), right_sibling->elements.begin(), right_sibling->elements.end());
        if(child_iter->is_leaf){
            child_iter->values.insert(child_iter->values.end(), right_sibling->values.begin(), right_sibling->values.end());
        }
        delete parent->children[child_idx + 1];
        parent->elements.erase(parent->elements.begin() + child_idx);
        parent->children.erase(parent->elements.begin() + child_idx + 1);
    }
}


template <class K, class V>
void BPlusTree<K, V>::erase(BPlusTreeNode* subroot, const K& key){
    const unsigned int MIN_ENTRY = (order / 2) - 1;

    // find the index of the entry or next node
    size_t first_larger_idx = insertion_idx(subroot->elements, key);
    if(first_larger_idx >= subroot->elements.size()){
        // out of bound
        return;
    }

    if(subroot->is_leaf && subroot->elements[first_larger_idx] == key){
        subroot->elements.erase(subroot->elements.begin() + first_larger_idx);
        subroot->values.erase(subroot->values.begin() + first_larger_idx);
    }else{
        erase(subroot->children[first_larger_idx], key);
    }

    // check if the number of elements in the node satisfy the rule
    if(!subroot->is_leaf && subroot->children[first_larger_idx]->elements.size() < MIN_ENTRY){
        //oops. Try to merge some entries
        merge_child(subroot, first_larger_idx);
    }
}