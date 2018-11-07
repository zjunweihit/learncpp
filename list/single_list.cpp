#include <iostream>

class Node
{
public:
    Node *next;
    int index;

    Node(int id = -1): next(nullptr), index(id)
    {
        //std::cout << index << "\n";
    }

    void print(void)
    {
        std::cout << index << "-> ";
    }
};

class SingleList
{
private:
    Node *head;

    void list_delete(Node *head, Node *entry);
    void list_add(Node *head, Node *entry);
    bool isEnd(Node *entry);

public:
    SingleList() : head(new Node) { }
    ~SingleList();
    bool isEmpty();
    void insertEntry(Node *entry);
    void insertEntry(int index);
    void removeEntry(int index);
    void traverseList(void);
    void cleanList(void);
    void sort(void);
};

void SingleList::list_add(Node *head, Node *entry)
{
    entry->next = head->next;
    head->next = entry;
}

void SingleList::list_delete(Node *prev, Node *entry)
{
    prev->next = entry->next;
    entry->next = nullptr;
    delete entry;
}

bool SingleList::isEnd(Node *entry)
{
    return (entry == nullptr);
}

void SingleList::cleanList(void)
{
    if (isEmpty())
        return;

    Node *prev = head;
    Node *cur = head->next;

    while (!isEnd(cur)) {
        list_delete(prev, cur);
        prev = head;
        cur = head->next;
    }
}

SingleList::~SingleList()
{
    cleanList();
    head->next = nullptr;
    delete head;
}

bool SingleList::isEmpty()
{
    return (head->next == nullptr);
}

void SingleList::insertEntry(Node *entry)
{
    list_add(head, entry);
}

void SingleList::insertEntry(int index)
{
    Node *e = new Node(index);
    list_add(head, e);
}

void SingleList::removeEntry(int index)
{
    Node *prev = head;
    Node *cur = head->next;

    while (!isEnd(cur)) {
        if (cur->index == index) {
            list_delete(prev, cur);
            break;
        }
        prev = cur;
        cur = cur->next;
    }

    if (isEnd(cur)) {
        std::cout << "Cannot find index " << index << "\n";
    }
}

void SingleList::traverseList(void)
{
    std::cout << "head-> ";

    Node *each = head->next;
    while (!isEnd(each)) {
        each->print();
        each = each->next;
    }
    std::cout << "end" << std::endl;
}

int main()
{
    // initialize the list head
    SingleList slist;

    // insert nodes
    slist.insertEntry(2);
    slist.insertEntry(8);
    slist.insertEntry(5);
    slist.traverseList();

    // remove nodes
    slist.removeEntry(2);
    slist.traverseList();

    // remove a non-existing node
    slist.removeEntry(7);
    slist.traverseList();

    // clean the list
    slist.cleanList();
    slist.traverseList();

    return 0;
}
