#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>

enum CHOICE
{
    EXIT = 0,
    GUESS,
    DIFFERENCE, 
    DEFINITION,

    SAVE = 7,
    CHANGE = 8,
    GRAPH = 9,
};

enum YESNO
{
    NO,
    YES,
};

typedef char* elem_t;

struct Node
{
    Node* parent;
    Node* left;
    Node* right;

    elem_t value;
};


struct SimpleTree
{
    Node* root;
    size_t size;
};

struct Lexemes
{
    char** buffer;
    size_t size;
};

//Service funcs
void userIdiot();
void showDefSequence(bool* def_sequence, size_t size_sequence);
void fillSequence(bool* sequence, size_t* size, Node* node);
inline bool isOpenBracket(char* symbol);
inline bool isCloseBracket(char* symbol);
inline bool isQuote(char* symbol);
inline bool isSlashN(char* symbol);
bool inStringInArray(const char* array[], size_t size, char* string);
int approxLength(const char* filename);
size_t getAmountLines(char* buffer);
char* readFile(const char* filename, size_t* buffer_size);
char** parseBuffer(size_t amount, char* buffer, size_t buffer_size);

//Node
void constructEmptyNode(Node* node);
void constructNode(Node* node, elem_t value);
void constructChild(Node* node, Node* parent, elem_t value);
void destructNode(Node* node);
void destructNodes(Node* nodese);
void addNewNode(Node* node);
void updateChildsNode(Node* node);
void writeNodes(FILE* file, Node* node);
void lazyFind(Node* node, elem_t value, Node** searchable_node);
void addNewFeature(Node* node);
Node* newNode(elem_t value);
Node* newChild(Node* parent, elem_t value);
Node* newEmptyNode();
Node* newEmptyChild(Node* parent);
Node* loadNudes(FILE* file, Node* node);

//SimpleTree
void constructSimpleTree(SimpleTree* tree);
void destructSimpleTree(SimpleTree* tree);
void graphvizTree(SimpleTree* tree);
void updDataBase(SimpleTree* tree, const char* filename);
SimpleTree* newSimpleTree();
SimpleTree* loadTree(Lexemes* lexemes);
SimpleTree* loadTree(const char* filename);

//MiniStack


//Lexemes
void constructLexemes(Lexemes* lexemes, const char* filename);
void destructLexemes(Lexemes* lexemes);
Lexemes* newLexemes(const char* filename);

//Processing
void Processing();
void Guess(SimpleTree* tree, bool* tree_changed);
void printDiff(Node** node, bool* def_sequence, char* word, size_t begin, size_t end);
void Difference(SimpleTree* tree);
void Definition(SimpleTree* tree);
void Save(SimpleTree* tree);
void Change(SimpleTree** tree);
void Graph(SimpleTree* tree);
bool Yes();
