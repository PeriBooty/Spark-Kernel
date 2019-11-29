#pragma once
#include <hardware/mm/mm.hpp>

template <class T>
struct NodeLink {
    T data;
    NodeLink* next;
    int index;
};

template <class T>
class LinkedList {
    using Type = T;
    using Link = NodeLink<T>;

public:
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

        if (list) {
            while (last->next)
                last = last->next;

            last->next = node;
        } else
            list = node;

        update();
    }

    bool insert_before(int index, Type value) {
        Link *node = list, *last, *result = (Link*)calloc(sizeof(Link));
        bool found = false;

        while (node) {
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

        while (node) {
            if (node->index == index) {
                found = true;
                break;
            }
            node = node->next;
        }

        if (found) {
            if (!node->next) {
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

        while (node) {
            if (node->index == index) {
                found = true;
                break;
            }

            last = node;
            node = node->next;
        }

        if (found) {
            if (node == list) return false;

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

        Link *node = list, *last;

        if (!node->next) {
            list = nullptr;
            free(node);
        } else {
            while (node->next) {
                last = node;
                node = node->next;
            }
            last->next = nullptr;
            free(node);
        }
        return true;
    }

    void clear() {
        if (!list)
            return;

        while (list)
            pop_back();
    }

private:
    void update() {
        if (!list)
            return;

        Link* last = list;
        int idx = 0;

        while (last) {
            last->index = idx++;
            last = last->next;
        }
    }
    Link* list;
};