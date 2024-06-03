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
        if (!top) { // 스택이 비어 있는 경우 새로운 노드를 top과 bottom으로 설정
            top = make_shared<Node>();
            bottom = top;
            top->processList.push_back(process);
        }
        else if (process.type == FOREGROUND) { // foreground 프로세스를 top 리스트 끝에 삽입
            top->processList.push_back(process);
        }
        else { // background 프로세스를 bottom 리스트 끝에 삽입
            shared_ptr<Node> newNode = make_shared<Node>();
            newNode->processList.push_back(process);
            bottom->next = newNode;
            bottom = newNode;
        }
    }

    Process dequeue() {
        if (!top || top->processList.empty()) {
            throw runtime_error("Stack is empty");
        }

        // stack top 리스트의 첫 번째 프로세스 가져오기
        Process process = top->processList.front();
        top->processList.pop_front();

        // 프로세스가 종료된 경우 리스트 노드 삭제
        if (top->processList.empty()) { // top 리스트가 비어 있으면 스택에서 해당 노드를 제거
            shared_ptr<Node> oldTop = top;
            top = top->next;
            if (!top) { // 스택이 완전히 비어 있는 경우
                bottom = nullptr;
            }
        }

        return process;
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

    stack.printStack();

    cout << "Dequeued process ID: " << stack.dequeue().id << endl;
    stack.printStack();

    return 0;
}