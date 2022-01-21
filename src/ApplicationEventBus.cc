#include "ApplicationEventBus.h"
#include <vector>
#include <cstdio>
std::vector<eventbus_callback> callbacks;

void ApplicationEventBus::registerBeforeClose(eventbus_callback func) {
    callbacks.push_back(func);
}

void ApplicationEventBus::beforeClose(){
    for (int i = 0; i < callbacks.size(); ++i) {
        callbacks[i]();
    }
}
