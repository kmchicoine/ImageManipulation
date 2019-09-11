#ifndef PNMR_H
#define PNMR_H

#include <source.h>

class PNMreader : public Source {
    public:
        PNMreader(char *f);
        ~PNMreader();
        virtual void Execute();
        virtual void Update();
        virtual const char *SourceName();
    protected:
        char *filename;
};

#endif
