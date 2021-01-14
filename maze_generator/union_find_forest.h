#include <vector>
#include <iostream>

using std::cerr;
using std::vector;
using std::size_t;

template <class T>
class union_find_forest {
    private:
        struct Node {
            T value;
            size_t parent; // index of parent
            size_t rank;
            Node(T& value, size_t parent) : value(value), parent(parent), rank(0) {}
        };
        vector<Node*> nodes;
        size_t num_sets;
    public:
        union_find_forest() : num_sets(0) {}
        union_find_forest(size_t size) : num_sets(0) {
            nodes.reserve(size);
        }
        template <class InputIterator>
        union_find_forest(InputIterator first, InputIterator last) {
            num_sets = 0;
            for (InputIterator i = first; i != last; ++i) insert(*i);
        }
        void insert(T& value) {
            Node* tmp = new Node(value, nodes.size());
            nodes.push_back(tmp);
            num_sets++;
        }
        size_t size() {
            return num_sets;
        }
        /**
         * Get the set in which the element at an index is contained
         * 
         * @param index index of node to find
         */
        size_t find_set(size_t index) {
            if (index >= nodes.size()) {
                cerr << "union_find_forest.find(): index out of bounds\n";
                exit(1);
            }
            size_t parent = nodes[index]->parent; 
            if (index != parent) parent = find_set(parent);
            return parent;
        }
        /**
         * Merge sets containing elements at two given indices
         * 
         * @param a index of node in first set to merge
         * @param b index of node in second set to merge
         */ 
        void union_sets(size_t a, size_t b) {
            if (a >= nodes.size() || b >= nodes.size()) {
                cerr << "union_find_forest.union(): index out of bounds\n";
                exit(1);
            }
            size_t set_a = find_set(a), set_b = find_set(b);
            if (set_a != set_b) {
                if (nodes[a]->rank > nodes[b]->rank) nodes[b]->parent = a ;
                else {
                    nodes[a]-> parent = b;
                    if (nodes[a]->rank == nodes[b]->rank) nodes[b]->rank++;
                }
                num_sets--;
            }
        }
        T& operator[](size_t index) {
            if (index >= nodes.size()) {
                cerr << "union_find_forest.operator[]: index out of bounds\n";
                exit(1);
            }
            return nodes[index]->value;          
        }
        void reserve(size_t size) {
            nodes.reserve(size);
        }
};