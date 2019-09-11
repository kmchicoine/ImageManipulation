#ifndef SINK_H
#define SINK_H
#include <stdlib.h>
#include <image.h>

class Sink {
    public:
        void SetInput(Image *img);
        void SetInput2(Image *img);
        virtual const char * SinkName() = 0;
    protected:
        Image *input = NULL;
        Image *input2 = NULL;

};



#endif
