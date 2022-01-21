#include <cstdlib>

#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <xinclude.h>

// an xdiff allocator using the C memory functions
class MallocAllocator
        : public memallocator_t
{
public:
    MallocAllocator()
    {
        this->priv    = this;
        this->malloc  = &allocate;
        this->free    = &release;
        this->realloc = &reallocate;
    }

private:
    static void *allocate(void *_this, unsigned size)
    {
        (void) _this;
        return std::malloc(size);
    }

    static void release(void *_this, void *ptr)
    {
        (void) _this;
        std::free(ptr);
    }

    static void *reallocate(void *_this, void *ptr, unsigned size)
    {
        (void) _this;
        return std::realloc(ptr, size);
    }
};