#include <xinclude.h>
#include <fstream>

class PatchEncoder
        : public mmfile_t {
public:
    PatchEncoder(long bsize, unsigned long flags) {
        if (xdl_init_mmfile(this, bsize, flags) == -1)
            throw std::runtime_error("xdl_init_mmfile");
    }

    ~PatchEncoder() {
        xdl_free_mmfile(this);
    }

    void load(std::string const &filename) {
        std::ifstream f(filename, std::ios::binary);
        if (!f.is_open())
            throw std::runtime_error("std::ifstream::open");

        f.seekg(0, std::ios::end);
        long size = static_cast<long>(f.tellg());
        f.seekg(0);

        char *buffer = static_cast<char *>(xdl_mmfile_writeallocate(this, size));
        f.read(buffer, size);
    }

    void diff(PatchEncoder &fold, PatchEncoder &fnew) {
        bdiffparam_t params;
        params.bsize = 32;

        xdemitcb_t cb;
        cb.priv = this;
        cb.outf = &PatchEncoder::append_buffers;

        if (xdl_bdiff(&fold, &fnew, &params, &cb) == -1)
            throw std::runtime_error("xdl_bdfiff");
    }

    void store(std::string const &filename) {
        std::ofstream f(filename, std::ios::binary);
        if (!f.is_open())
            throw std::runtime_error("std::ofstream::open");

        long size;
        char *buffer = static_cast<char *>(xdl_mmfile_first(this, &size));
        while (buffer) {
            f.write(buffer, size);
            buffer = static_cast<char *>(xdl_mmfile_next(this, &size));
        }
    }

    int append_buffers(mmbuffer_t *buffers, int count) {
        xdl_writem_mmfile(this, buffers, count);
        return 0;
    }

private:
static int append_buffers(void *_this, mmbuffer_t *buffers, int count)
{
    return static_cast<PatchEncoder *>(_this)->append_buffers(buffers, count);
}
};