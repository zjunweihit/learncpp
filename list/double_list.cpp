#include <iostream>

class Node
{
public:
    Node *prev;
    Node *next;
    int index;

    Node(int id = -1): prev(nullptr), next(nullptr), index(id)
    {
        //std::cout << index << "\n";
    }

    void print(void)
    {
        std::cout << index << "<-> ";
    }
};

class DoubleList
{
private:
    Node *head;

    void list_delete(Node *head, Node *entry);
    void list_add(Node *head, Node *entry);
    bool isEnd(Node *entry);

public:
    DoubleList() : head(new Node)
    {
        head->next = head;
        head->prev = head;
    }
    ~DoubleList();
    bool isEmpty();
    void insertEntry(Node *entry);
    void insertEntry(int index);
    void removeEntry(int index);
    void traverseList(void);
    void cleanList(void);
    void sort(void);
};

void DoubleList::list_add(Node *head, Node *entry)
{
    Node *next = head->next;

    entry->next = next;
    entry->prev = head;

    next->prev = entry;
    head->next = entry;
}

void DoubleList::list_delete(Node *prev, Node *entry)
{
    Node *next = entry->next;

    prev->next = next;
    next->prev = prev;

    entry->next = nullptr;
    entry->prev = nullptr;
    delete entry;
}

void DoubleList::cleanList(void)
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

DoubleList::~DoubleList()
{
    cleanList();
    head->next = nullptr;
    head->prev = nullptr;
    delete head;
}

bool DoubleList::isEmpty()
{
    return (head->next == head);
}

bool DoubleList::isEnd(Node *entry)
{
    return (entry == head);
}

void DoubleList::insertEntry(Node *entry)
{
    list_add(head, entry);
}

void DoubleList::insertEntry(int index)
{
    Node *e = new Node(index);
    list_add(head, e);
}

void DoubleList::removeEntry(int index)
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

void DoubleList::traverseList(void)
{
    std::cout << "head<-> ";

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
    DoubleList slist;

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
