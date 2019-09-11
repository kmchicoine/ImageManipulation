#ifndef PNMW_H
#define PNMW_H

#include <sink.h>

class PNMwriter : public Sink {
    public:
        void Write(char *filename);
        virtual const char * SinkName();
};


#endif
