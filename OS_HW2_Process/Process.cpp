#include <iostream>
#include <memory>
#include <string>
#include <list>

using namespace std;

enum ProcessType {
    FOREGROUND,
    BACKGROUND
};

struct Process {
    int id;
    ProcessType type;

    Process(int id, ProcessType type) : id(id), type(type) {}
};

struct Node {
    list<Process> processList;
    shared_ptr<Node> next;

    Node() : next(nullptr) {}
};

class Stack {
private:
    shared_ptr<Node> top;
    shared_ptr<Node> bottom;

public:
    Stack() : top(nullptr), bottom(nullptr) {}

    void enqueue(Process process) {
        shared_ptr<Node> newNode = make_shared<Node>();
        newNode->processList.push_back(process);

        if (!top) { // ������ ��� �ִ� ���
            top = newNode;
            bottom = newNode;
        }
        else if (process.type == FOREGROUND) { // foreground process�� top ����Ʈ ���� ����
            top->processList.push_back(process);
        }
        else { // background process�� bottom ����Ʈ ���� ����
            bottom->next = newNode;
            bottom = newNode;
        }
    }

    void printStack() {
        shared_ptr<Node> current = top;
        while (current) {
            for (const auto& process : current->processList) {
                cout << "Process ID: " << process.id << ", Type: " << (process.type == FOREGROUND ? "foreground" : "background") << endl;
            }
            current = current->next;
        }
    }
};

int main() { // test
    Stack stack;
    stack.enqueue(Process(1, FOREGROUND));
    stack.enqueue(Process(2, BACKGROUND));
    stack.enqueue(Process(3, FOREGROUND));
    stack.enqueue(Process(4, BACKGROUND));

    stack.printStack();

    return 0;
}