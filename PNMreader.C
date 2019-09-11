#include <PNMreader.h>
#include <image.h>
#include <string.h>
#include <stdio.h>
#include <logging.h>
#include <regex>

void PNMreader::Update() {
    char msg[128];
    sprintf(msg, "%s: about to execute\n", SourceName());
    Logger::LogEvent(msg);
    Execute();
    sprintf(msg, "%s: done executing\n", SourceName());
    Logger::LogEvent(msg);
    
}

const char *PNMreader::SourceName() {
    return "PNMreader";
}

PNMreader::PNMreader(char *f) {
    filename = new char[50];
    strcpy(filename, f);    
}

void PNMreader::Execute() {
    std::regex test(".*\\.pnm$");
    if (! std::regex_match(filename,test)) {
        char msg[1024];
        sprintf(msg, "%s: invalid filename, must be .pnm", "PNMreader"); 
        DataFlowException e("PNMreader", msg);
        throw e;
    }   
    char magicNum[128];
    int width, height, maxval;
    Image *img;
    Pixel pix;
    FILE *f_in = fopen(filename, "r");
    if (f_in == NULL) {
        char msg[1024];
        sprintf(msg, "%s: invalid filename", "PNMreader"); 
        DataFlowException e("PNMreader", msg);
        throw e;
    }   
    fscanf(f_in, "%s\n%d %d\n%d\n", magicNum, &width, &height, &maxval);
    img = GetOutput();
    img->ResetSize(width, height);
    for (int h = 0; h < height; h++) {
        for (int w = 0; w < width; w++) {
            fread(&pix, sizeof(unsigned char), 3, f_in);
            img->SetPixel(pix, w, h);
        }   
    }
    fclose(f_in);
    return;
}

PNMreader::~PNMreader() {
    delete[] filename;
    return;
}
