#ifndef BLOCKSTUDIO_APPLICATIONEVENTBUS_H
#define BLOCKSTUDIO_APPLICATIONEVENTBUS_H

typedef void (*eventbus_callback)();

class ApplicationEventBus {
public:
    static void registerBeforeClose(eventbus_callback func);
    static void beforeClose();
};


#endif //BLOCKSTUDIO_APPLICATIONEVENTBUS_H
