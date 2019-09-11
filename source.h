#ifndef SOURCE_H
#define SOURCE_H

#include <image.h>

class Source {
    public:
        virtual void Execute(void) = 0;
        Image * GetOutput();
        Source(void);
        virtual void Update(void) = 0;
        virtual const char * SourceName() = 0;
    protected:
        Image output;
        int updateCount = 0;
};

#endif
