#ifndef FILTER_H
#define FILTER_H

#include <image.h>
#include <source.h>
#include <sink.h>

class CheckSum : public Sink {
    public:
        virtual const char * SinkName();
        void OutputCheckSum(char *file);
};

class Color : public Source {
    public:
        Color(int w, int h, unsigned char r, unsigned char g, unsigned char b);
        virtual const char * SourceName();
        virtual void Execute();
        virtual void Update();
    private:
        unsigned char red;
        unsigned char green;
        unsigned char blue;
        int width;
        int height;
        
};

class Filter : public Source, public Sink {
    public:
        virtual void Update(void);
        virtual const char * SourceName();
        virtual const char * FilterName() = 0;    
        virtual const char * SinkName();
};

class Mirror : public Filter {
    public:
        virtual void Execute();
        virtual const char *FilterName();
};

class Rotate : public Filter {
    public:
        virtual void Execute();
        virtual const char *FilterName();
};

class Subtract : public Filter {
    public:
        virtual void Execute();
        virtual const char *FilterName();
};

class Grayscale : public Filter {
    public:
        virtual void Execute();
        virtual const char *FilterName();
};

class Blur : public Filter {
    public:
        virtual void Execute();
        virtual const char *FilterName();
};

class Shrinker : public Filter {
    public:
        virtual void Execute();
        virtual const char *FilterName();
};

class LRCombine : public Filter {
    public:
        virtual void Execute();
        virtual const char *FilterName();
};

class TBCombine : public Filter {
    public:
        virtual void Execute();
        virtual const char *FilterName();
};

class Blender : public Filter {
    public:
        void SetFactor(double f){factor = f;};;
        virtual void Execute();
        virtual const char *FilterName();
    private:
        double factor;
};


#endif
