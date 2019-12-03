#pragma once
#include <hardware/mm/mm.hpp>

template <typename T>
struct NodeLink {
    T data;
    NodeLink<T>* next;
    int index;
};

template <typename T>
class LinkedList {
    using Type = T;
    using Link = NodeLink<T>;

public:
    template <typename iterator_T>
    class LinkedListIterator {
    public:
        explicit LinkedListIterator(NodeLink<iterator_T>* link)
            : link(link) {
        }

        Type& operator*() {
            return link->data;
        }

        void operator++() {
            if (link == nullptr)
                link = link->next;
        }

        bool operator!=(LinkedListIterator it) {
            return link == it.link;
        }

        NodeLink<iterator_T>* link;
    };

    LinkedList()
        : list(nullptr) {
    }

    ~LinkedList() {
        clear();
    }

    Link* find(int index) {
        Link* temp = list;

        while (temp && temp->index != index)
            temp = temp->next;

        return temp;
    }

    Type* operator[](int index) {
        Link* node = find(index);

        return node ? &node->data : nullptr;
    }

    void push(Type value) {
        Link* node = (Link*)calloc(sizeof(Link));
        node->data = value;
        node->next = list;
        list = node;
        update();
    }

    void push_back(Type value) {
        Link *node = (Link*)calloc(sizeof(Link)), *last = list;
        node->data = value;
        node->next = nullptr;

        if (list != nullptr) {
            while (last->next != nullptr)
                last = last->next;

            last->next = node;
        } else
            list = node;

        update();
    }

    bool insert_before(int index, Type value) {
        Link *node = list, *last, *result = (Link*)calloc(sizeof(Link));
        bool found = false;

        while (node != nullptr) {
            if (node->index == index) {
                found = true;
                break;
            }

            last = node;
            node = node->next;
        }

        if (found) {
            if (node == list) {
                push(value);
                free(result);
            } else {
                result->data = value;
                last->next = result;
                result->next = node;

                update();
            }

            return true;
        }

        free(result);

        return false;
    }

    bool insert_after(int index, Type value) {
        Link *node = list, *last, *result = (Link*)calloc(sizeof(Link));
        bool found = false;

        while (node != nullptr) {
            if (node->index == index) {
                found = true;
                break;
            }

            node = node->next;
        }

        if (found) {
            if (node->next == nullptr) {
                push_back(value);
                free(result);
            } else {
                result->data = value;
                last = node->next;
                node->next = result;
                result->next = last;

                update();

                return true;
            }
        }

        free(result);

        return false;
    }

    bool pop() {
        if (!list)
            return false;

        Link* node = list;
        list = list->next;
        free(node);
        update();

        return true;
    }

    bool pop(int index) {
        if (!list)
            return false;

        Link *node = list, *last;
        bool found = false;

        while (node != nullptr) {
            if (node->index == index) {
                found = true;
                break;
            }

            last = node;
            node = node->next;
        }

        if (found) {
            if (node == list)
                return false;

            if (node == list->next) {
                list = node;
                free(last);
            } else {
                find(last->index - 1)->next = node;
                free(last);
            }

            update();

            return true;
        }

        return false;
    }

    bool pop_back() {
        if (!list)
            return false;

        Link *node = list, *last = nullptr;

        if (node == nullptr || !node->next) {
            list = nullptr;
            free(node);
        } else {
            while (node->next != nullptr) {
                last = node;
                node = node->next;
            }

            if (last == nullptr)
                return false;

            last->next = nullptr;
            free(node);
        }
        return true;
    }

    void clear() {
        if (list == nullptr)
            return;

        while (list != nullptr)
            pop_back();
    }

    LinkedListIterator<Type> begin() {
        return LinkedListIterator<Type>(list);
    }

    LinkedListIterator<Type> end() {
        return LinkedListIterator<Type>(nullptr);
    }

private:
    void update() {
        if (list == nullptr)
            return;

        Link* last = list;
        int idx = 0;

        while (last != nullptr) {
            last->index = idx++;
            last = last->next;
        }
    }
    Link* list;
};