#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "AVLTree.h"
using namespace std;
using namespace std::chrono;

class error_messenger
{
    int err_cnt;
    high_resolution_clock::time_point start;
public:
    error_messenger()
    {
        err_cnt = 0;
        start = high_resolution_clock::now();
    }
    void report_error(const std::string& msg)
    {
        std::cerr << msg << std::endl;
        ++err_cnt;
    }
    void report_errors(const std::string& msg)
    {
        std::cerr << msg;
        if (err_cnt == 0)
            std::cerr << "No errors detected!";
        else
            std::cerr << "Number of errors: " << err_cnt;

        high_resolution_clock::time_point now = high_resolution_clock::now();
        duration<double> time_span = duration_cast<duration<double>>(now - start);
        std::cerr << " (in " << time_span.count() << " seconds.)" << std::endl;
    }
    ~error_messenger()
    {
        report_errors("Final result: ");
    }
};

template <typename T> std::string to_string(const T& src)
{
    std::stringstream ss;
    ss << src;
    return ss.str();
}

template <typename K, typename I>
bool assertContent(error_messenger& em, const AVLTree<K, I>& avl, const string& res)
{
    stringstream ss;
    ss << avl;
    if (ss.str() != res)
    {
        em.report_error("Invalid output " + ss.str() + " should be " + res);
        return false;
    }
    return true;
}

template <typename K, typename I>
string to_string(const vector<K>& keys, const vector<I>& values, const vector<int> indices)
{
    vector<int> inorder(indices);
    sort(inorder.begin(), inorder.end());
    stringstream ss;
    ss << '{';
    for (int i = 0; i < (int)inorder.size(); i++)
    {
        ss << keys[inorder[i]] << ": " << values[inorder[i]];
        if ( i + 1 < (int)inorder.size())
            ss << ", ";
    }
    ss << '}';
    return ss.str();
}

ostream& operator <<(ostream& os, const vector<int>& v)
{
    os << '[';
    for (int i = 0; i < (int)v.size(); i++)
    {
        os << v[i];
        if (i < (int)v.size() - 1)
            os << ", ";
    }
    return os << ']';
}

class WordCounter
{
public:
    WordCounter(){ tree = new AVLTree<std::string, int>(); }
    ~WordCounter(){delete tree;}
    int size(){
        return tree->size();
    }
    void add(std::string word){
        if(tree->keyExists(word)){
            (tree->operator[](word))++;
        }  else{
            tree->insert(word,1);
        }

    }

    friend  std::ostream& operator<<(std::ostream& os, const WordCounter &wc)
    {
        std::stringstream st;
        st<<"{";
        //rhs.printInorder(st, rhs.root);
        wc.tree->printInorder(st,wc.tree->getRoot());
        std::string s(st.str());
        if(wc.tree->getRoot()){s.pop_back(); s.pop_back();}
        s.append("}");
        os<<s;
        return os;
    }

private:
    AVLTree<std::string, int>* tree;
};

std::ostream& operator<<(std::ostream& os, WordCounter& wc){
    return os;
}

int main()
{

    error_messenger em;

    {
        AVLTree<int, float> a;
        assertContent(em, a, "{}");
    }
    em.report_errors("Empty avl tree: ");

    {
        vector<int> keys{1, 2, 3, 4, 5, 6, 7, 8};
        vector<float> values{1.1f, 2.2f, 3.3f, 4.4f, 5.5f, 6.6f, 7.7f, 8.8f};

        vector<int> indices{0, 1, 2, 3, 4, 5, 6, 7};
        auto first = indices.begin();
        auto last = indices.end();
        while (next_permutation(first, last))
        {
            AVLTree<int, float> a;
            for (int count = 1; count <= (int)indices.size(); count++)
            {
                vector<int> idx;
                for (int i=0; i < count; i++)
                {
                    a.insert(keys[indices[i]], values[indices[i]]);
                    idx.push_back(indices[i]);
                }
                string res = to_string(keys, values, idx);
                assertContent(em, a, res);
                a.clear();
            }
        }
    }
    em.report_errors("Exhaustive insertions: ");

    {
        vector<float> keys{1.1f, 2.2f, 3.3f, 4.4f, 5.5f, 6.6f, 7.7f, 8.8f};
        vector<string> values{"1.1f", "2.2f", "3.3f", "4.4f", "5.5f", "6.6f", "7.7f", "8.8f"};

        vector<int> indices{0, 1, 2, 3, 4, 5, 6, 7};
        auto first = indices.begin();
        auto last = indices.end();
        while (next_permutation(first, last))
        {
            AVLTree<float, string> a;
            for (int i = 0; i < (int)keys.size(); i++)
                a.insert(keys[i], values[i]);
            for (int count = 1; count <= (int)indices.size(); count++)
            {
                AVLTree<float, string> b(a);
                vector<int> idx;
                for (int i=0; i < count; i++)
                    idx.push_back(indices[i]);
                for (int i=count; i < (int)indices.size(); i++)
                    b.remove(keys[indices[i]]);
                string res = to_string(keys, values, idx);
                assertContent(em, b, res);
            }
        }
    }
    em.report_errors("Exhaustive deletions: ");

    // This part is the new task that require defining a new class: WordCounter
    // This class is based on the AVLTree and its purpose is to count words
    // The class interface should be derived from this code

    cout << "\n\nWordCounter tests\n\n";

    vector<string> simplistic {
            "alina ",
            "helena halina helena ",
            "helena halina hanna halina helena ",
            "helena halina hanna halina helena halina " };

    for (int i = 0; i < (int)simplistic.size(); i++)
    {
        WordCounter wc;
        stringstream ss(simplistic[i]);
        string word;
        while (ss >> word)
            wc.add(word);
        cout << wc << endl;
    }

    WordCounter wc;
    ifstream is("C:\\Users\\Robert\\Desktop\\STUDIA\\EADS\\LAB3\\TheVoyageofBeagle.txt");
    if (!is.good())
    {
        return -1;
    }
    string word;
    while (is >> word)
        wc.add(word);
    cout << "\n\nThe Voyage of Beagle by Charles Darwin contains: " << wc.size() << " different words.\n";

    wc.add("baba");
    cout<<wc;
    AVLTree<int,string> b1;

    cout<<b1<<endl;
    cout<<"-------"<<endl;

    b1.insert(3,"halina");
    cout<<b1<<endl;
    cout<<endl;
    cout<<"-------"<<endl;

    b1.insert(3,"halina");
    b1.insert(2,"max");
    cout<<b1<<endl;;
    cout<<endl;
    cout<<"-------"<<endl;

    b1.insert(8,"tom");
    b1.insert(2,"adam");
    b1.insert(1,"peter");
    b1.insert(6,"jan");
    b1.insert(6,"michal");
    b1.insert(5,"sam");
    cout<<b1<<endl;
    cout<<endl;
    cout<<"-------"<<endl;
    b1.remove(6);
    b1.remove(1);
    cout<<endl;
    cout<<"-------"<<endl;
    b1.graph();
    cout<<endl;
    cout<<"-------"<<endl;
    cout<<b1<<endl;
    cout<<b1[2]<<endl;
    cout<<endl;
    cout<<"-------"<<endl;
    b1.clear();
    cout<<b1;

    return 0;

}

/* Output should be similar to this:

Empty avl tree: No errors detected! (in 0 seconds.)
Exhaustive insertions: No errors detected! (in 16.8675 seconds.)
Exhaustive deletions: No errors detected! (in 35.3183 seconds.)


WordCounter tests

{alina: 1}
{halina: 1, helena: 2}
{halina: 2, hanna: 1, helena: 2}
{halina: 3, hanna: 1, helena: 2}


The Voyage of Beagle by Charles Darwin contains: 14193 different words.
Final result: No errors detected! (in 35.8008 seconds.)

Process returned 0 (0x0)   execution time : 36.618 s
Press any key to continue.

*/
