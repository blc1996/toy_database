#pragma once

#include <vector>
#include <iostream>
#include <string>
#include <sstream>

/**
 * b+ tree implementation for indexing
 * b_plus_tree.h file provide interface
 * modified based on CS225 b-tree lab
 */
template <class K, class V>
class BPlusTree
{
  private:
    /**
     * A class for the basic node structure of the BPlusTree. A node contains
     * two vectors, one with DataPairs representing the data, and one of
     * BPlusTreeNode*s, representing the node's children.
     */
    struct BPlusTreeNode {
        bool is_leaf;
        BPlusTreeNode* next;
        std::vector<K> elements;
        std::vector<K> values;
        std::vector<BPlusTreeNode*> children;

        /**
         * Constructs a BPlusTreeNode. The vectors will reserve to avoid
         * reallocations.
         */
        BPlusTreeNode(bool is_leaf, unsigned int order) : is_leaf(is_leaf), next(NULL)
        {
            if(is_leaf){
                elements.reserve(order + 1);
            }
            values.reserve(order + 1);
            children.reserve(order + 2);
        }

        /**
         * Constructs a BPlusTreeNode based on another. Only copies over
         * the elements and is_leaf information.
         */
        BPlusTreeNode(const BPlusTreeNode& other)
            : is_leaf(other.is_leaf), elements(other.elements), values(other.values), next(other.next)
        {
        }

        /**
         * Printing operator for a BPlusTreeNode. E.g. a node containing 4, 5, 6
         * would look like:
         * <pre>
         * | 4 | 5 | 6 |
         * *   *   *   *
         * </pre>
         * The stars below the bars represent non-null child pointers. Null
         * child pointers are represented by an "N". If there are no children
         * then "no children" is displayed instead.
         * @param out The ostream to be written to.
         * @param n The node to be printed.
         * @return The modified ostream.
         */
        inline friend std::ostream& operator<<(std::ostream& out,
                                               const BPlusTreeNode& n)
        {
            std::string node_str;
            node_str.reserve(2 * (4 * n.elements.size() + 1));
            for (int i = 0; i < n.elements.size(); i++) {
                std::stringstream temp;
                temp << n.elements[i];
                node_str += "| ";
                node_str += temp.str();
                node_str += " and ";
                std::stringstream temp2;
                temp2 << n.values[i];
                node_str += temp2.str();
            }
            if (!n.elements.empty()) {
                node_str += "|";
            }
            // node_str += "\n";
            for (auto& child : n.children) {
                if (child == nullptr) {
                    node_str += "N   ";
                } else {
                    node_str += "*   ";
                }
            }
            // if (n.children.empty()) {
            //     node_str += "no children";
            // }

            out << node_str;
            return out;
        }
    };

    unsigned int order;
    BPlusTreeNode* root;

    void DFS_print(BPlusTreeNode* subnode){
        if(subnode->is_leaf){
            std::cout<<*subnode;
            return;
        }
        for(auto i : subnode->children){
            DFS_print(i);
        }
    }

  public:
    /**
     * Constructs a default, order 64 BPlusTree.
     */
    BPlusTree();

    void print()
    {
        // std::cout<<*root<<std::endl;
        // for(auto i : root->children){
        //     std::cout<<*i<<std::endl;
        // }
        std::cout<<"Leaf nodes: ";
        DFS_print(root);
        std::cout<<std::endl;
    }

    /**
     * Constructs a BPlusTree with the specified order. The minimum order allowed
     * is order 3.
     * @param order The order of the constructed BPlusTree.
     */
    BPlusTree(unsigned int order);

    /**
     * Constructs a BPlusTree as a deep copy of another.
     * @param other The BPlusTree to copy.
     */
    BPlusTree(const BPlusTree& other);

    /**
     * Performs checks to make sure the BPlusTree is valid. Specifically
     * it will check to make sure that an in-order traversal of the tree
     * will result in a sorted sequence of keys. Also verifies that each
     * BPlusTree node doesn't have more nodes than its order.
     * @return true if it satisfies the conditions, false otherwise.
     */
    bool is_valid(unsigned int order = 64) const;

    /**
     * Destroys a BPlusTree.
     */
    ~BPlusTree();

    /**
     * Assignment operator for a BPlusTree.
     * @param rhs The BPlusTree to assign into this one.
     * @return The copied BPlusTree.
     */
    const BPlusTree& operator=(const BPlusTree& rhs);

    /**
     * Clears the BPlusTree of all data.
     */
    void clear();

    /**
     * Inserts a key and value into the BPlusTree. If the key is already in the
     * tree do nothing.
     * @param key The key to insert.
     * @param value The value to insert.
     */
    void insert(const K& key, const V& value);

    /**
     * Finds the value associated with a given key.
     * @param key The key to look up.
     * @return The value (if found), the default V if not.
     */
    V find(const K& key) const;

  private:
    /**
     * Private recursive version of the insert function.
     * @param subroot A reference of a pointer to the current BPlusTreeNode.
     * @param pair The DataPair to be inserted.
     */
    void insert(BPlusTreeNode* subroot, const K& key, const V& value);

    /**
     * Private recursive version of the find function.
     * @param subroot A reference of a pointer to the current BPlusTreeNode.
     * @param key The key we are looking up.
     * @return The value (if found), the default V if not.
     */
    V find(const BPlusTreeNode* subroot, const K& key) const;

    /**
     * Splits a child node of a BPlusTreeNode. Called if the child became too
     * large. Modifies the parent such that children[child_idx] contains
     * half as many elements as before, and similarly for
     * children[child_idx + 1] (which is a new BPlusTreeNode*).
     * @param parent The parent whose child we are trying to split.
     * @param child_idx The index of the child in its parent's children
     * vector.
     */
    void split_child(BPlusTreeNode* parent, size_t child_idx);

    /**
     * Private recursive version of the clear function.
     * @param subroot A pointer to the current node being cleared.
     */
    void clear(BPlusTreeNode* subroot);

    /**
     * Private recursive version of the copy function.
     * @param subroot A pointer to the current node being copied.
     */
    BPlusTreeNode* copy(const BPlusTreeNode* subroot);

    /**
     * Private recursive version of the is_valid function.
     * @param subroot A pointer to the current node being checked for
     * validity.
     * @return true if the node's suBPlusTree is valid, false otherwise.
     */
    bool is_valid(const BPlusTreeNode* subroot, std::vector<K>& data,
                  unsigned int order) const;
};

/**
 * Generalized function for finding the insertion index of a given element
 * into a given sorted vector.
 * @param elements A sorted vector of some type.
 * @param val A value which represents something to be inserted into the vector.
 * Must either be the same type as T, or one that can compare to it. E.g. for
 * the elements of a BPlusTreeNode we might pass in either a DataPair value or a
 * K value (the key).
 * @return The index at which val could be inserted into elements to maintain
 * the sorted order of elements. If val occurs in elements, then this returns
 * the index of val in elements.
 */
template <class T, class C>
size_t insertion_idx(const std::vector<T>& elements, const C& val)
{
    /* TODO Your code goes here! */
    if(elements.size() == 0)
        return 0;

    int start = 0, end = elements.size() - 1;
    while(start < end){
        int middleIdx = (start + end) / 2;
        if(val > elements[middleIdx]){
            start = 1 + middleIdx;
        }else if(val == elements[middleIdx]){
            return middleIdx;
        }else{
            end = middleIdx - 1;
        }
    }

    if(start >= (int)elements.size() || val < elements[start] || val == elements[start])
        return start;
    else
        return start + 1;
}
