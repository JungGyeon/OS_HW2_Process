// 2-1 enqueue(), dequeue(), promote(), split_n_merge()

#include <iostream>
#include <memory>
#include <string>
#include <list>
#include <algorithm>

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
    int totalProcesses;
    int totalNodes;

    void split_n_merge(shared_ptr<Node> node) {
        while (node) {
            int threshold = totalProcesses / totalNodes; // ��ü ���μ��� ���� / ���� ��� ��
            if (node->processList.size() > threshold) {
                auto it = node->processList.begin();
                advance(it, node->processList.size() / 2); 
                list<Process> splitList(it, node->processList.end());
                node->processList.erase(it, node->processList.end());

                shared_ptr<Node> newNode = make_shared<Node>();
                newNode->processList = splitList;
                newNode->next = node->next;
                node->next = newNode;
                totalNodes++;

                split_n_merge(newNode);
            }
            node = node->next;
        }
    }

public:
    Stack() : top(nullptr), bottom(nullptr), totalProcesses(0), totalNodes(0) {}

    void enqueue(Process process) {
        if (!top) {
            // ������ ��� �ִ� ��� ���ο� ��带 top�� bottom���� ����
            top = make_shared<Node>();
            bottom = top;
            totalNodes++;
        }
        if (process.type == FOREGROUND) {
            // foreground ���μ����� top ����Ʈ ���� ����
            top->processList.push_back(process);
        }
        else {
            // background ���μ����� bottom ����Ʈ ���� ����
            if (bottom->processList.empty()) {
                bottom->processList.push_back(process);
            }
            else {
                shared_ptr<Node> newNode = make_shared<Node>();
                newNode->processList.push_back(process);
                bottom->next = newNode;
                bottom = newNode;
                totalNodes++;
            }
        }
        totalProcesses++;
        split_n_merge(top);
    }

    Process dequeue() {
        if (!top || top->processList.empty()) {
            throw runtime_error("Stack is empty");
        }

        // stack top ����Ʈ�� ù ��° ���μ��� ��������
        Process process = top->processList.front();
        top->processList.pop_front();
        totalProcesses--;

        // ���μ����� ����� ��� ����Ʈ ��� ����
        if (top->processList.empty()) {
            // top ����Ʈ�� ��� ������ ���ÿ��� �ش� ��带 ����
            shared_ptr<Node> oldTop = top;
            top = top->next;
            totalNodes--;
            if (!top) {
                bottom = nullptr; // ������ ������ ��� �ִ� ���
            }
        }

        return process;
    }

    void promote() {
        if (!top) {
            // ������ ��� �ִ� ���
            return;
        }

        shared_ptr<Node> current = top;
        shared_ptr<Node> prev = nullptr;

        while (current) {
            shared_ptr<Node> next = current->next;

            if (!next) {
                // ���� ���� ��尡 ������ ���� ����Ʈ�� �߰�
                if (prev) {
                    // ���� ����Ʈ�� ������ ����
                    prev->next = current;
                    bottom = current; // bottom ����
                }
                else {
                    // top���� promotion�� �ϸ� ���ο� ���� ��� ����
                    shared_ptr<Node> newTop = make_shared<Node>();
                    newTop->next = top;
                    top = newTop;
                    totalNodes++;
                }

                // ���� ����Ʈ�� ��� ������ �ش� ���� ��� ����
                if (current->processList.empty()) {
                    if (prev) {
                        prev->next = nullptr;
                    }
                    else {
                        top = nullptr;
                        bottom = nullptr;
                        totalNodes--;
                    }
                }

                // ���� ���� ���� �̵�
                prev = current;
                current = next;
            }
            else {
                // ���� ���� ��尡 �ִ� ��� ���� ���� �̵�
                prev = current;
                current = next;
            }
        }
    }

    void printStack() {
        shared_ptr<Node> current = top;
        while (current) {
            for (const auto& process : current->processList) {
                cout << "Process ID: " << process.id << ", Type: " << (process.type == FOREGROUND ? "Foreground" : "Background") << endl;
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

    cout << "Initial stack:" << endl;
    stack.printStack();

    cout << "Dequeued process ID: " << stack.dequeue().id << endl;
    stack.printStack();

    stack.promote();

    cout << "Stack after promotion:" << endl;
    stack.printStack();

    return 0;
}
