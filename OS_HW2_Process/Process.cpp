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
            int threshold = totalProcesses / totalNodes; // 전체 프로세스 개수 / 스택 노드 수
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
            // 스택이 비어 있는 경우 새로운 노드를 top과 bottom으로 설정
            top = make_shared<Node>();
            bottom = top;
            totalNodes++;
        }
        if (process.type == FOREGROUND) {
            // foreground 프로세스를 top 리스트 끝에 삽입
            top->processList.push_back(process);
        }
        else {
            // background 프로세스를 bottom 리스트 끝에 삽입
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

        // stack top 리스트의 첫 번째 프로세스 가져오기
        Process process = top->processList.front();
        top->processList.pop_front();
        totalProcesses--;

        // 프로세스가 종료된 경우 리스트 노드 삭제
        if (top->processList.empty()) {
            // top 리스트가 비어 있으면 스택에서 해당 노드를 제거
            shared_ptr<Node> oldTop = top;
            top = top->next;
            totalNodes--;
            if (!top) {
                bottom = nullptr; // 스택이 완전히 비어 있는 경우
            }
        }

        return process;
    }

    void promote() {
        if (!top) {
            // 스택이 비어 있는 경우
            return;
        }

        shared_ptr<Node> current = top;
        shared_ptr<Node> prev = nullptr;

        while (current) {
            shared_ptr<Node> next = current->next;

            if (!next) {
                // 다음 스택 노드가 없으면 상위 리스트에 추가
                if (prev) {
                    // 상위 리스트의 꼬리에 붙임
                    prev->next = current;
                    bottom = current; // bottom 갱신
                }
                else {
                    // top에서 promotion을 하면 새로운 스택 노드 생성
                    shared_ptr<Node> newTop = make_shared<Node>();
                    newTop->next = top;
                    top = newTop;
                    totalNodes++;
                }

                // 현재 리스트가 비어 있으면 해당 스택 노드 제거
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

                // 다음 스택 노드로 이동
                prev = current;
                current = next;
            }
            else {
                // 다음 스택 노드가 있는 경우 다음 노드로 이동
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
