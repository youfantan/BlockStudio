#ifndef TASKQUEUE_H
#define TASKQUEUE_H
#include <string>
#include <map>
#include <vector>
#include <list>

typedef int(*task)(void* data);
struct task_info{
    std::string name;
    task tsk;
    void* data;
};
class TaskQueue {
public:
    static void addTask(std::string taskName,task tsk);
    static void executeTask(std::string taskName,void* data);
    //following task can only be called by main function
    static bool hasRemain();
    static void executeOneFromQueue();
private:
    static std::map<std::string,task> TaskMap;
    static std::list<task_info*> Queue;
};
#endif