#include "headerFiles/akinatorHeader.h"

const char open_bracket = '{';
const char close_bracket = '}';
const char* default_filename = "base.txt";
const char* variants_yes[] = {"y", "ye", "yes", "yeah"};
const size_t amount_yes = 4;
const char* variants_no[] = {"n", "no", "nope"};
const size_t amount_no = 3;
const size_t max_string_size = 128;

//Service funcs
inline bool isOpenBracket(char* symbol)
{
    return (*symbol == open_bracket);
}

inline bool isCloseBracket(char* symbol)
{
    return (*symbol == close_bracket);
}

inline bool isQuote(char* symbol)
{
    return (*symbol == '\"');
}

inline bool isSlashN(char* symbol)
{
    return (*symbol == '\n');
}

bool isStringInArray(const char* array[], size_t size, char* string)
{
    for (size_t i = 0; i < size; ++i)
    {
        if (strcmp(array[i], string) == 0)
        {
            return true;
        }
    }
    return false;
}

bool isRight(Node* node)
{
    return (node->parent->right == node);
}

int approxLength(const char* filename)
{
	assert(filename);
	
	struct stat* buff = (struct stat*)calloc(1, sizeof(struct stat));

	stat(filename, buff);

	return buff->st_size;
	
}

char* readFile(const char* filename, size_t* buffer_size)
{
    FILE* file = fopen(filename, "r");

    assert(file);
    
    size_t apr_size = approxLength(filename);

    char* buffer = (char*)calloc(apr_size, sizeof(char));
    *buffer_size = fread(buffer, sizeof(char), apr_size, file);
    buffer[*buffer_size] = '\0';

    return buffer;
}

char** parseBuffer(size_t amount, char* buffer, size_t buffer_size)
{
    char** lexemes = (char**)calloc(amount, sizeof(char*));

    size_t j = 0;
    size_t i = 0;
    while (i < buffer_size)
    {   
        if (isQuote(buffer + i))
        {
            ++i; //Jump over quote
            lexemes[j++] = buffer + i;
            i  = strchr(buffer + i, '\"') - buffer;
            buffer[i] = '\0'; //Split lexemes
            ++i; //Jump over slash zero
        }
        else if (isOpenBracket(buffer + i) || isCloseBracket(buffer + i))
        {
            lexemes[j++] = buffer + i;
            ++i; //Jump over bracket;
            buffer[i] = '\0'; //Split lexeme
        }
        ++i;
    }
    return lexemes;
}

size_t getAmountLines(char* buffer)
{
    size_t amount_lines = 0;
    for (size_t i = 0; buffer[i] != '\0'; ++i)
    {
        if (buffer[i] == '\n')
        {
            ++amount_lines;
        }
    }
    return amount_lines;
}

size_t Min(const size_t size_a, const size_t size_b)
{
    return (size_a > size_b) ? size_b : size_a;
}

bool* getDefinitionSequence(SimpleTree* tree, char* word, size_t* size_sequence)
{
    Node* seacrhable_node = nullptr; 
    lazyFind(tree->root, word, &seacrhable_node);

    if (seacrhable_node == nullptr) 
    {
        printf("I cant find this word: %s.\n", word);
        *size_sequence = 0;
        return nullptr;
    }

    bool* def_sequence = (bool*)calloc(tree->size, sizeof(bool));
    Node* cur_node = seacrhable_node;
    fillSequence(def_sequence, size_sequence, cur_node);

    return def_sequence;
}

void graphvizTree(SimpleTree* tree)
{
    if (tree->root == nullptr) 
    {
        return;
    }
    FILE* file = fopen("graphPlot.txt", "w");

    fprintf(file, "digraph G{\n");
    fprintf(file, "node [shape=\"record\", style=\"solid\", color=\"green\"];\n");

    loadNudes(file, tree->root);

    fprintf(file, "}");

    fclose(file);

    system("dot -Tjpg  graphPlot.txt > Graph.jpg");
}

void fillSequence(bool* sequence, size_t* size, Node* node)
{
    if (node == nullptr)
    {
        *size = 0;
    }
    while (node->parent != nullptr)
    {
        if (isRight(node))
        {
            sequence[*size] = true;
        }
        else
        {
            sequence[*size] = false;
        }
        *size += 1;
        node = node->parent;
    }
}

void showDefSequence(bool* def_sequence, size_t size_sequence)
{
    for (size_t i = 0; i < size_sequence; ++i)
    {
        printf("seq[%zu] = %s\n", i, def_sequence[size_sequence - i - 1] ? "YES" : "NO");
    }
}

char* getWord()
{
    char* word = (char*)calloc(max_string_size, sizeof(char));

    // scanf("%s", word);
    //possibly safe reading all line
    scanf("\n");
    fgets(word, sizeof(char) * max_string_size, stdin);
    word[strlen(word) - 1] = '\0';

    return word;
}

void printFeatureAndGoNext(bool* def_sequence, size_t size_sequence, size_t idx, Node** node)
{
    if (def_sequence[size_sequence - idx - 1] == true)
    {
        printf("%s", (*node)->value);
        *node = (*node)->right;
    }
    else
    {
        printf("not %s", (*node)->value);
        *node = (*node)->left;
    }
    if (size_sequence - idx - 1 != 0)
    {
        printf(", ");
    }

}

void userIdiot()
{
	while ((getchar()) != '\n');

	printf("Uhh, you are fucking idiot, do you think you are smarter than me? No. You can input only integer values!\n");

}

//Node
void constructEmptyNode(Node* node)
{
    node->left = nullptr;
    node->parent = nullptr;
    node->right = nullptr;
    node->value = 0; 
}

void constructNode(Node* node, elem_t value)
{
    constructEmptyNode(node);
    node->value = value; 
}

void constructChild(Node* node, Node* parent, elem_t value)
{
    constructNode(node, value);
    node->parent = parent;
}

void destructNode(Node* node)
{
    free(node);
}

void destructNodes(Node* node)
{
    if (node == nullptr)
    {
        return;
    }
    if (node->left != nullptr)
    {
        destructNodes(node->left);
    }
    if (node->right != nullptr)
    {
        destructNodes(node->right);
    }
    destructNode(node);
}

void writeNodes(FILE* file, Node* node)
{   
    if (node == nullptr)
    {
        return;
    }

    fprintf(file, "{\n");
    fprintf(file, "\"%s\"\n", node->value);

    if (node->left != nullptr)
    {
        writeNodes(file, node->right);
    }
    if (node->right != nullptr)
    {
        writeNodes(file, node->left);
    }
    fprintf(file, "}\n");
}

void lazyFind(Node* node, elem_t value, Node** searchable_node)
{
    if (node == nullptr || strcmp(node->value, value) == 0)
    {
        *searchable_node = node;
        return;
    }

    lazyFind(node->left, value, searchable_node);
    if (*searchable_node != nullptr)
    {
        return;
    }
    lazyFind(node->right, value, searchable_node);
}

Node* newNode(elem_t value)
{
    Node* new_node = (Node*)calloc(1, sizeof(Node));
    
    constructNode(new_node, value);

    return new_node;
}

Node* newEmptyNode()
{
    Node* new_node = (Node*)calloc(1, sizeof(Node));

    constructEmptyNode(new_node);

    return new_node;
}

Node* newEmptyChild(Node* parent)
{
    Node* new_child = newEmptyNode();

    new_child->parent = parent;

    return new_child;
}

Node* newChild(Node* parent, elem_t value)
{
    Node* new_child = (Node*)calloc(1, sizeof(Node));
    
    constructChild(new_child, parent, value);

    return new_child;
}

Node* loadNudes(FILE* file, Node* node)
{
    if (node == nullptr)
    {
        return nullptr;
    }
    if (node->left != nullptr)
    {
        fprintf(file, "edge[color=orange]\n\"%p\"->\"%p\";\n", loadNudes(file, node->left)->parent, node->left);  // link parent and left child
    }
    if (node->right != nullptr)
    {
        fprintf(file, "edge[color=red]\n\"%p\"->\"%p\";\n", loadNudes(file, node->right)->parent, node->right); //link parent and right child
    }
    if (node->parent != nullptr)
    {
        fprintf(file, "\"%p\" [label=\"{{par: %s}|{str: %s}}\"]\n ", node, node->parent->value,
            node->value); // <-- label printing
    }
    else
    {
        fprintf(file, "\"%p\" [label=\"{{par: %s}|{str: %s}}\"]\n ", node, "root",
            node->value); // <-- label printing
    }
    return node;
}

//SimpleTree
void constructSimpleTree(SimpleTree* tree)
{
    tree->root = newEmptyNode();
    tree->size = 0;
}

void destructSimpleTree(SimpleTree* tree)
{
    free(tree->root->value - 1);
    destructNodes(tree->root);
}

SimpleTree* newSimpleTree()
{
    SimpleTree* tree = (SimpleTree*)calloc(1, sizeof(SimpleTree));

    constructSimpleTree(tree);

    return tree;
}

SimpleTree* loadTree(Lexemes* lexemes)
{
    SimpleTree* tree = newSimpleTree();
    tree->root->value = lexemes->buffer[0];
    
    Node* cur_node = tree->root;
    size_t amount_lexemes = lexemes->size;
    for (size_t i = 0; i < amount_lexemes; ++i)
    {
        if (isOpenBracket(lexemes->buffer[i]))
        {   
            if (cur_node->right == nullptr)
            {   
                cur_node->right = newEmptyChild(cur_node);
                cur_node = cur_node->right;
            }
            else if (cur_node->left == nullptr)
            {
                cur_node->left = newEmptyChild(cur_node);
                cur_node = cur_node->left;   
            }
            ++tree->size;
        }
        else if (isCloseBracket(lexemes->buffer[i]))
        {
            //Он точно должен существовать, так как файл с закрывающейся скобки начинаться не может
            cur_node = cur_node->parent;
        }
        else
        {
            cur_node->value = lexemes->buffer[i];
        }
    }

    return tree;
}

SimpleTree* loadTree(const char* filename)
{
    Lexemes* lexemes = newLexemes(filename);

    SimpleTree* tree = loadTree(lexemes);

    return tree;
}

//Lexemes
void constructLexemes(Lexemes* lexemes, const char* filename)
{
    size_t buffer_size = 0;
    char* main_buffer = readFile(filename, &buffer_size);
    lexemes->size = getAmountLines(main_buffer);
    lexemes->buffer = parseBuffer(lexemes->size, main_buffer, buffer_size);
}

void destructLexemes(Lexemes* lexemes)
{
    free(*lexemes->buffer); //free main buffer
    free(lexemes->buffer);
}

Lexemes* newLexemes(const char* filename)
{
    Lexemes* lexemes = (Lexemes*)calloc(1, sizeof(Lexemes));

    constructLexemes(lexemes, filename);

    return lexemes;
}


//Processing./
void Processing()
{
    SimpleTree* tree = loadTree(default_filename);

    printf("   Welcome to the Akinator!  \n");

    CHOICE choice = GUESS;
    bool tree_changed = false;

    while(choice)
    {   
        printf("=============================\n"
               "Game:                  type 1\n"
               "Difference:            type 2\n"
               "Definition:            type 3\n"
               "Exit:                  type 0\n"
               "-----------------------------\n"
               "Save changes:          type 7\n"
               "Change data base:      type 8\n"
               "Load graph:            type 9\n" 
               "=============================\n"
              );
          
        while (scanf("%d", &choice) != 1)
        {
            userIdiot();
        }
        switch (choice)
        {
            case GUESS:
                Guess(tree, &tree_changed);
                break;

            case DIFFERENCE:
                Difference(tree);
                break;

            case DEFINITION:
                Definition(tree);
                break;

            case SAVE:
                Save(tree);
                break;

            case CHANGE:
                Change(&tree);
                break;
            
            case GRAPH:
                Graph(tree);
                break;

            case EXIT:
                break;

            default:
                printf("Unknown command.\nTry again.\n");
                break;
        }
    }
    if (tree_changed)
    {
        Save(tree);
    }
    printf("Exit...\n");
    destructSimpleTree(tree);
}

void Guess(SimpleTree* tree, bool* tree_changed)
{
    Node* cur_node = tree->root;
    while (cur_node != nullptr)
    {
        if (cur_node->left == nullptr && cur_node->right == nullptr)
        {
            printf("I think, it is %s!\n", cur_node->value);
            if (Yes())
            {
                printf("Yeah boyyy!\nRASKATAL NA IZI\n");
                return;
            }
            else
            {
                printf(":'( Uhh, I'm sorry, I don't know what did you guess...\n");
                addNewFeature(cur_node);
                ++tree->size;
                *tree_changed = true;
                return;
            }
        }
        else
        {
            printf("Is it %s?\n", cur_node->value);
            if (Yes())
            {
                cur_node = cur_node->right;
            }
            else
            {
                cur_node = cur_node->left;
            }
        }
    }
}

void Definition(SimpleTree* tree)
{
    printf("What definition word do you want to know?\n");
    char* word = getWord();

    size_t size_sequence = 0;
    bool* def_sequence = getDefinitionSequence(tree, word, &size_sequence);

    if (def_sequence == nullptr)
    {
        return;
    }

    Node* cur_node = tree->root;
    printf("%s is ", word);
    for (size_t i = 0; i < size_sequence; ++i)
    {
        printFeatureAndGoNext(def_sequence, size_sequence, i, &cur_node);
    }
    printf("\n");
    free(word);
}

void printDiff(Node** node, bool* def_sequence, char* word, size_t begin, size_t end)
{
    printf("but %s is ", word);
    for (size_t i = begin; i < end; ++i)
    {
        printFeatureAndGoNext(def_sequence, end, i, node);
    }
}

void Difference(SimpleTree* tree)
{
    printf("Type 2 word which difference between you want to know\n");
    char* word_a = getWord();
    char* word_b = getWord();

    size_t size_sequence_a = 0;
    bool* def_sequence_a = getDefinitionSequence(tree, word_a, &size_sequence_a);

    size_t size_sequence_b = 0;
    bool* def_sequence_b = getDefinitionSequence(tree, word_b, &size_sequence_b);

    if (def_sequence_a == nullptr || def_sequence_b == nullptr)
    {
        return;
    }

    bool is_first_iter = true;;
    Node* cur_node_a = tree->root;
    Node* cur_node_b = tree->root;
    size_t i = 0;
    size_t j = 0;
    while (i < size_sequence_a && j < size_sequence_b && 
            def_sequence_a[size_sequence_a - i - 1] == def_sequence_b[size_sequence_b - j - 1])
    {   
        if (is_first_iter) printf ("They are both ");
        printFeatureAndGoNext(def_sequence_a, size_sequence_a, i, &cur_node_a);
        cur_node_b = cur_node_a;
        is_first_iter = false;
        ++i;
        ++j;
    }

    printDiff(&cur_node_a, def_sequence_a, word_a, i, size_sequence_a);
    printf(", ");
    printDiff(&cur_node_b, def_sequence_b, word_b, j, size_sequence_b);
    printf("\n");
    free(word_a);
    free(word_b);
}

void Change(SimpleTree** tree)
{
    printf("Type name of file\n");

    char* filename = getWord();

    destructSimpleTree(*tree);

    *tree = loadTree(filename);
}

void Save(SimpleTree* tree)
{
    printf("Save in default file?\n");
    if (Yes())
    {
        updDataBase(tree, default_filename);
    }
    else
    {
        printf("Type name of file.\n");
        char* filename = getWord();
        updDataBase(tree, filename);

        free(filename);
    }
}

void Graph(SimpleTree* tree)
{
    printf("Loading graph...\n");

    graphvizTree(tree);

    printf("Graph was loaded successful!\n");
}

bool Yes()
{

    char* answer = (char*)calloc(16, sizeof(char));
    scanf("%s", answer);
    
    while(answer)
    {
        if (isStringInArray(variants_yes, amount_yes, answer))
        {
            return true;
        }
        else if (isStringInArray(variants_no, amount_no, answer))
        {
            return false;
        }
        else
        {
            printf("PISHI KAK CHELOVEK, CHO VIPENDRIVAYESHSA!\n");
        }
        scanf("%s", answer);
    }
    
    return false;
}

void addNewFeature(Node* node)
{
    Node* new_node = newEmptyNode();
    printf("What did you guess?\n");
    new_node->value = getWord();

    Node* their_parent = newEmptyNode();
    printf("How is %s different from %s?\n", node->value, new_node->value);
    their_parent->value = getWord();

    if (isRight(node))
    {
        node->parent->right = their_parent;
    }
    else
    {
        node->parent->left = their_parent;
    }
    

    their_parent->parent = node->parent;
    new_node->parent = their_parent;
    node->parent = their_parent;

    their_parent->right = node;
    their_parent->left = new_node;
}

void updDataBase(SimpleTree* tree, const char* filename)
{
    FILE* file = fopen(filename, "w");

    fprintf(file, "\"%s\"\n", tree->root->value);

    writeNodes(file, tree->root->right);
    writeNodes(file, tree->root->left);

    fclose(file);   
}