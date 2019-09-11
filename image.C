#include <image.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <source.h>
#include <logging.h>

int Image::GetWidth() {return width;}
int Image::GetHeight() {return height;}

Image::Image(void) {
    width = 0;
    height = 0;
}

Image::Image(Image *cpImg) {
    width = cpImg->GetWidth();
    height = cpImg->GetHeight();
    delete[] pixels;
    pixels = new Pixel[width*height];
    for (int i = 0; i < width*height; i++) {
        memcpy((Pixel *)&pixels[i], (Pixel *)&cpImg->pixels[i], sizeof(Pixel));
    }
}

Image::Image(int w, int h) {
    if (w  < 0 || h < 0) {
        char msg[1024];
        sprintf(msg, "%s: invalid image dimensions: %i, %i", "Image::Image(w,h)", w, h); 
        DataFlowException e("Image::Image(w,h)", msg);
        throw e;
    }
    width = w;
    height = h;
    delete[] pixels;
    pixels = new Pixel[w*h];
}

void Image::ResetSize(int w, int h) {
    if (w  < 0 || h < 0) {
        char msg[1024];
        sprintf(msg, "%s: invalid image dimensions: %i, %i", "Image::ResetSize", w, h); 
        DataFlowException e("Image::ResetSize", msg);
        throw e;
    }
    width = w;
    height = h;
    delete[] pixels;
    pixels = new Pixel[w*h];
}

Pixel Image::GetPixel(int w, int h) {
    if (w < 0 || h < 0 || w >= width || h >= height) {
        char msg[1024];
        sprintf(msg, "%s: invalid image location: %i, %i", "Image::GetPixel", w, h); 
        DataFlowException e("Image::GetPixel", msg);
        throw e;
    }
    Pixel returnVal = pixels[width*h+w];
    return returnVal;
}

void Image::SetPixel(Pixel p, int w, int h) {
    if (w  < 0 || h < 0 || w >= width || h >= height) {
        char msg[1024];
        sprintf(msg, "%s: invalid image location: %i, %i", "Image::SetPixel", w, h); 
        DataFlowException e("Image::SetPixel", msg);
        throw e;
    }
    pixels[width*h+w] = p;
}
void Image::Update() {
    if (src != NULL) {
        src->Update();
    }
}

void Image::SetSource(Source *s) {
    src = s;
    return;
}

Image::~Image() {
    delete[] pixels;
}
