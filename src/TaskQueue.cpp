#include "TaskQueue.h"
#include "Utils.h"
#include "Localizer.h"

void TaskQueue::addTask(std::string taskName, task tsk) {
    TaskMap[taskName]=tsk;
}

void TaskQueue::executeTask(std::string taskName, void *data) {
    task_info *info=new task_info;
    info->name=taskName;
    info->tsk=TaskMap[taskName];
    info->data=data;
    Queue.push_back(info);
    LOG_DEBUG("Task %s has been add into pre-execute list",taskName.c_str());
}

void *syncExecutor(void *data){
    task_info *info=(task_info*)data;
    int ret=info->tsk(info->data);
    if (ret!=0){
        LOG_WARNING(Localizer::getInstance()->getString("TASK_QUEUE_RETVALUE_NOT_RIGHT").c_str(),info->name.c_str(),ret);
    }
    return nullptr;
}

pthread_t TaskQueue::executeTaskAsync(std::string taskName, void *data) {
    task_info *info=new task_info;
    info->name=taskName;
    info->tsk=TaskMap[taskName];
    info->data=data;
    pthread_t tid;
    pthread_create(&tid, nullptr,syncExecutor,(void*)info);
    return tid;
}

bool TaskQueue::hasRemain() {
    if (!Queue.empty()){
        LOG_DEBUG("Task Remain:%d",Queue.size());
        return true;
    }
    return false;
}

void TaskQueue::executeOneFromQueue() {
    task_info *info=Queue.front();
    int ret=info->tsk(info->data);
    if (ret!=0){
        LOG_WARNING(Localizer::getInstance()->getString("TASK_QUEUE_RETVALUE_NOT_RIGHT").c_str(),info->name.c_str(),ret);
    }
    Queue.pop_front();
    LOG_DEBUG("Task %s has been executed done",info->name.c_str());
}

std::map<std::string,task> TaskQueue::TaskMap;
std::list<task_info*> TaskQueue::Queue;