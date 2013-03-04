#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdint>
#include <queue>
#include <map>
#include <istream>
#include <ostream>
#include <list>

using namespace std;

typedef uint32_t node_weight;
typedef map<char, node_weight> frequency_table;

struct node
{
    char c;
    node_weight weight;
    node * left;
    node * right;
};

node * init_node()
{
    node * n = new node;
    n->c = 0;
    n->left = 0;
    n->right = 0;
    n->weight = 0;

    return n;
}

node * init_node(node * left, node * right)
{
    node * n = new node;
    n->c = 0;
    n->left = left;
    n->right = right;
    n->weight = left->weight + right->weight;

    return n;
}

void delete_node(node * n)
{
    if(n->left)
    {
        delete_node(n->left);
    }
    if(n->right)
    {
        delete_node(n->right);
    }
    delete n;
}

node * frequency_table_to_huffman(frequency_table &freq)
{
    auto comp = ([](const node * a, const node * b) -> bool {
        if(a->weight == b->weight)
        {
            return a->c > b->c;
        }
        return a->weight > b->weight;
    });
    priority_queue<node *, vector<node *>, decltype(comp)> pq(comp);

    // Loop through all characters and add them to the priority queue.
    // They will be sorted by their weight
    for(auto &character:freq)
    {
        node * n = init_node();
        n->c = character.first;
        n->weight = character.second;
        pq.push(n);
    }

    while(!(pq.size() == 1))
    {
        node * a = pq.top();
        pq.pop();
        node * b = pq.top();
        pq.pop();

        node * n = init_node(a, b);
        pq.push(n);
    }
    return pq.top();
}

void create_table(node * n, list<bool> &current_encoding, map<char, vector<bool> > &m)
{
    if(n->c != 0) // This node is a leaf
    {
        m[n->c] = vector<bool>(current_encoding.begin(), current_encoding.end());
    }
    else
    {
        // Recurse through the left and right subtree
        current_encoding.push_back(false);
        create_table(n->left, current_encoding, m);
        current_encoding.pop_back();

        current_encoding.push_back(true);
        create_table(n->right, current_encoding, m);
        current_encoding.pop_back();
    }
}

map<char, vector<bool> > create_table(node * n)
{
    map<char, vector<bool> > m;
    list<bool> current_encoding;
    create_table(n, current_encoding, m);
    return m;
}

void encode(istream &in, ostream &out)
{
    frequency_table freq;

    in.seekg(0);
    while(!in.eof())
    {
        char c = in.get();
        if(c == -1)
        {
            continue;
        }
        freq[c]++;
    }

    // Add end of text indicator to the tree
    freq[3] = 1;

    node * huffman_tree = frequency_table_to_huffman(freq);
    auto encoding_table = create_table(huffman_tree);

    // File header Bouke hUFFman encoding
    out.write("BUFF", 4);

    for(auto &iter:freq)
    {
        char c;
        out.put(iter.first);

        for(int i = sizeof(node_weight) - 1; i >= 0; i--)
        {
            c = 0xFF & ((iter.second & (0xFF << i * 8)) >> (i * 8));
            out.put(c);
        }
    }

    out.put(0);


    char current_byte = 0;
    int pos = 0;

    in.clear();
    in.seekg(0);

    while(!in.eof())
    {
        char c = in.get();

        for(bool b:encoding_table[c])
        {
            current_byte <<= 1;
            if(b)
            {
                current_byte |= 1;
            }
            pos++;
            if(pos == 8)
            {
                out.put(current_byte);
                current_byte = 0;
                pos = 0;
            }
        }

    }
    if(pos != 0)
    {
        for(bool b:encoding_table[3])
        {
            current_byte <<= 1;
            if(b)
            {
                current_byte |= 1;
            }
            pos++;
            if(pos == 8)
            {
                out.put(current_byte);
                current_byte = 0;
                pos = 0;
            }
        }
        if(pos != 0)
        {
            out.put(current_byte << (8 - pos));
        }
    }
}

void decode(istream &in, ostream &out)
{
    in.seekg(0);
    // Check for correct fileheader BUFF
    if(!(in.get() == 'B' and in.get() == 'U' and in.get() == 'F' and in.get() == 'F'))
    {
        cerr << "Invalid file header" << endl;
        return;
    }
    frequency_table freq;

    // Read frequency table
    while(true)
    {
        if(in.eof())
        {
            cerr << "Incorrect file format" << endl;
            return;
        }
        char c = in.get();
        if(c == 0)
        {
            break;
        }
        node_weight weight = 0;
        for(size_t i = 0; i < sizeof(node_weight); i++)
        {
            weight <<= 8;
            weight |= in.get();
        }
        freq[c] = weight;
    }

    // Convert frequency table to the corresponding Huffman tree
    node * huffman_tree = frequency_table_to_huffman(freq);
    node * tree_position = huffman_tree;

    while(!in.eof())
    {
        char c = in.get();
        for(int i = 7; i >= 0; i--)
        {
            bool set = c & (1 << i);
            tree_position = set ? tree_position->right : tree_position->left;
            if(tree_position->c)
            {
                if(tree_position->c == 3) // ETX end of text reached
                {
                    return;
                }
                else
                {
                    out.put(tree_position->c);
                    tree_position = huffman_tree;
                }
            }
        }
    }
}

int main(int argc, char const *argv[])
{
    if (argc != 4 ||
        strlen(argv[1]) != 1 ||
        (argv[1][0] != 'e' && argv[1][0] != 'd'))
    {
        cerr << argv[0] << " (e|d) <filename> <outfile>" << endl;
        return 1;
    }

    ifstream in_stream;
    in_stream.open(argv[2], ios::in | ios::binary);

    ofstream out_stream;
    out_stream.open(argv[3], ios::out | ios::binary);

    if(argv[1][0] == 'e')
    {

        encode(in_stream, out_stream);
    }
    else
    {
        decode(in_stream, out_stream);
    }
    in_stream.close();
    out_stream.close();

    return 0;
}