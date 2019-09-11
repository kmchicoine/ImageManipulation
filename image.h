#ifndef IMAGE_H
#define IMAGE_H

class Source;

struct Pixel {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
};

class Image {
    public:
        Image(void);
        Image(int w, int h);
        Image(Image *cpImg); 
        void ResetSize(int w, int h);
        Pixel GetPixel(int w, int h);
        void SetPixel(Pixel p, int w, int h);
        int GetWidth();
        int GetHeight();
        void Update();
        void SetSource(Source *s);
        ~Image();
    private: 
        int width;
        int height;
        Pixel *pixels = new Pixel[1];
        Source *src;
};

#endif
