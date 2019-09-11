#include <filter.h>
#include <stdio.h>
#include <logging.h>
#include <string.h>

char msg[128];

Color::Color(int w, int h, unsigned char r, unsigned char g, unsigned char b) {
    if (w  <= 0 || h <= 0) {
        char msg[1024];
        sprintf(msg, "%s: invalid image dimensions: %i, %i", SourceName(), w, h);
        DataFlowException e(SourceName(), msg);
        throw e;
    }

    if (r < 0 || g < 0 || b < 0) {
        char msg[1024];
        sprintf(msg, "%s: invalid rgb value: %d, %d, %d", SourceName(), r, g, b);
        DataFlowException e(SourceName(), msg);
        throw e;
    }
    if (r > 255 || g > 255 || b > 255) {
        char msg[1024];
        sprintf(msg, "%s: invalid rbg value: %d, %d, %d", SourceName(), r, g, b);
        DataFlowException e(SourceName(), msg);
        throw e;
    }
    width = w;
    height = h;
    red = r;
    green = g;
    blue = b;
}

void Color::Update() {
    updateCount ++;
    Execute();
}

void Filter::Update() {
    updateCount ++;
    if (updateCount > 200) {
        char msg[1024];
        sprintf(msg, "%s: circular dependency", SinkName());
        DataFlowException e(SinkName(), msg);
        throw e;

    }
    if (input == GetOutput() || input2 == GetOutput()) {
        char msg[1024];
        sprintf(msg, "%s: input and output must differ", SinkName());
        DataFlowException e(SinkName(), msg);
        throw e;
    }
    if (input != NULL) {
        sprintf(msg, "%s: about to update input1\n", SourceName());
		Logger::LogEvent(msg);
        input->Update();
        sprintf(msg, "%s: done updating input1\n", SourceName());
		Logger::LogEvent(msg);
    }
    if (input2 != NULL) {

        sprintf(msg, "%s: about to update input2\n", SourceName());
		Logger::LogEvent(msg);
        input2->Update();
        sprintf(msg, "%s: done updating input2\n", SourceName());
		Logger::LogEvent(msg);
    }   

    sprintf(msg, "%s: about to execute\n", SourceName());
	Logger::LogEvent(msg);
    Execute();
    sprintf(msg, "%s: done executing\n", SourceName());
	Logger::LogEvent(msg);
}

const char *CheckSum::SinkName() {
    return "CheckSum";
}

const char *Color::SourceName() {
    return "Color";
}

const char *Filter::SourceName() {
    return FilterName();
}

const char * Filter::SinkName() {
    return FilterName();
}

const char *Mirror::FilterName() {
    return "Mirror";
}

const char *Rotate::FilterName() {
    return "Rotate";
}

const char *Subtract::FilterName() {
    return "Subtract";
}

const char *Grayscale::FilterName() {
    return "Grayscale";
}

const char *Blur::FilterName() {
    return "Blur";
}

const char *Shrinker::FilterName() {
    return "Shrinker";
}

const char *LRCombine::FilterName() {
    return "LRCombine";
}

const char *TBCombine::FilterName() {
    return "TBCombine";
}

const char *Blender::FilterName() {
    return "Blender";
}

void CheckSum::OutputCheckSum(char *file) {
    unsigned char redSum = 0;
    unsigned char blueSum = 0;
    unsigned char greenSum = 0;
    Pixel setter;
    FILE * f = fopen(file, "w");
    for (int h = 0; h < input->GetHeight(); h++) {
        for (int w = 0; w < input->GetWidth(); w++) {
        setter = input->GetPixel(w,h);
        redSum += setter.red;
        blueSum += setter.blue;
        greenSum += setter.green;
        }
    }
    sprintf(msg, "CHECKSUM: %d, %d, %d\n", redSum, greenSum, blueSum);
    fwrite(msg, strlen(msg), 1, f);
    fclose(f);
    return;
}

void Color::Execute() {
    Image *out = GetOutput();
    Pixel setter;
    setter.red = red;
    setter.green = green;
    setter.blue = blue;
    out->ResetSize(width, height);
    for (int h = 0; h < height; h++) {
        for (int w = 0; w < width; w++) {
            out->SetPixel(setter, w, h);
        }
    }
}

void Mirror::Execute() {
    if (input == NULL) {
        char msg[1024];
        sprintf(msg, "%s: missing input", SinkName());
        DataFlowException e(SinkName(), msg);
        throw e;
    }
    Image *out = GetOutput();
    Pixel setter;
    int inW = input->GetWidth();
    out->ResetSize(input->GetWidth(), input->GetHeight());
    for (int h = 0; h < input->GetHeight(); h++) {
        for (int w = 0; w < input->GetWidth(); w++) {
            setter = input->GetPixel(w,h);
            out->SetPixel(setter, inW-w-1, h);        
        }
    }
    return;
}

void Rotate::Execute() {
    if (input == NULL) {
        char msg[1024];
        sprintf(msg, "%s: missing input", SinkName());
        DataFlowException e(SinkName(), msg);
        throw e;
    }
    Image *out = GetOutput();
    Pixel setter;
    out->ResetSize(input->GetHeight(), input->GetWidth());
    for (int h = 0; h < input->GetHeight(); h++) {
        for (int w = 0; w < input->GetWidth(); w++) {
            setter = input->GetPixel(w,h);
            out->SetPixel(setter,input->GetHeight()-1- h, w);
        }
    }
    return;
}

unsigned char PixelDiff(unsigned char pix1, unsigned char pix2) {
    if (pix1 < pix2) {
        return 0;
    }
    else {
        return pix1-pix2;
    }
}

void Subtract::Execute() {
    if (input == NULL || input2 == NULL) {
        char msg[1024];
        sprintf(msg, "%s: missing input", SinkName());
        DataFlowException e(SinkName(), msg);
        throw e;
    }
    if (input->GetHeight() != input2->GetHeight()) {
        char msg[1024];
        sprintf(msg, "%s: heights must match: %d, %d", SinkName(), input->GetHeight(), input2->GetHeight());
        DataFlowException e(SinkName(), msg);
        throw e;
    }
    if (input->GetWidth() != input2->GetWidth()) {
        char msg[1024];
        sprintf(msg, "%s: widths must match: %d, %d", SinkName(), input->GetWidth(), input2->GetWidth());
        DataFlowException e(SinkName(), msg);
        throw e;
    }

    Image *out = GetOutput();
    Pixel setter, setter2;
    Pixel setterDiff;
    int inW = input->GetWidth();
    int inH = input->GetHeight();
    out->ResetSize(inW,inH);
    for (int h = 0; h < inH; h++) {
        for (int w = 0; w < inW; w++) {
            setter = input->GetPixel(w,h);
            setter2 = input2->GetPixel(w,h);
            setterDiff.red = PixelDiff(setter.red, setter2.red);
            setterDiff.green = PixelDiff(setter.green, setter2.green);
            setterDiff.blue = PixelDiff(setter.blue, setter2.blue);
            out->SetPixel(setterDiff, w, h);
        }
    }    
    return;
}

void Grayscale::Execute() {
    if (input == NULL) {
        char msg[1024];
        sprintf(msg, "%s: missing input", SinkName());
        DataFlowException e(SinkName(), msg);
        throw e;
    }
    Image *out = GetOutput();
    Pixel setter;
    unsigned char grayVal;
    out->ResetSize(input->GetWidth(), input->GetHeight());
    for (int h = 0; h < input->GetHeight(); h++) {
        for (int w = 0; w < input->GetWidth(); w++) {
            setter = input->GetPixel(w,h);
            grayVal = setter.red/5 + setter.green/2 + setter.blue/4;
            setter.red = grayVal;
            setter.green = grayVal;
            setter.blue = grayVal;
            out->SetPixel(setter, w, h);
        }
    }
    return;
}

void Blur::Execute() {
    if (input == NULL) {
        char msg[1024];
        sprintf(msg, "%s: missing input", SinkName());
        DataFlowException e(SinkName(), msg);
        throw e;
    }
    Image *out = GetOutput();
    Pixel setter, setterBlur;
    int inW = input->GetWidth();
    int inH = input->GetHeight();
    out->ResetSize(input->GetWidth(), input->GetHeight());
    for (int h = 0; h < input->GetHeight(); h++) {
        for (int w = 0; w < input->GetWidth(); w++) {
            setter = input->GetPixel(w,h);
            if (w == 0 || h == 0 || w == inW-1 || h == inH-1) {
                out->SetPixel(setter, w, h);
            } 
            else { 
                setterBlur.red = input->GetPixel(w-1,h-1).red/8 + input->GetPixel(w,h-1).red/8 + 
                    input->GetPixel(w+1, h-1).red/8 + input->GetPixel(w-1,h).red/8 + input->GetPixel(w+1, h).red/8 +
                    input->GetPixel(w-1,h+1).red/8 + input->GetPixel(w,h+1).red/8 + input->GetPixel(w+1, h+1).red/8;
                setterBlur.green = input->GetPixel(w-1,h-1).green/8 + input->GetPixel(w,h-1).green/8 + 
                    input->GetPixel(w+1, h-1).green/8 + input->GetPixel(w-1,h).green/8 + input->GetPixel(w+1, h).green/8 +
                    input->GetPixel(w-1,h+1).green/8 + input->GetPixel(w,h+1).green/8 + input->GetPixel(w+1, h+1).green/8;
                setterBlur.blue = input->GetPixel(w-1,h-1).blue/8 + input->GetPixel(w,h-1).blue/8 + 
                    input->GetPixel(w+1, h-1).blue/8 + input->GetPixel(w-1,h).blue/8 + input->GetPixel(w+1, h).blue/8 +
                    input->GetPixel(w-1,h+1).blue/8 + input->GetPixel(w,h+1).blue/8 + input->GetPixel(w+1, h+1).blue/8;
                out->SetPixel(setterBlur, w, h);
            }
        }
    }
    return;
}

void Shrinker::Execute() {
    if (input == NULL) {
        char msg[1024];
        sprintf(msg, "%s: missing input", SinkName());
        DataFlowException e(SinkName(), msg);
        throw e;
    }
    int width = input->GetWidth();
    int height = input->GetHeight();
    int outW = 0;
    int outH = 0;
    Pixel setter;
    Image *out = GetOutput();
    out->ResetSize(width/2, height/2);
    for (int h=0; h < height-1; h += 2) {
        for (int w=0; w < width-1; w += 2) {
            outW = w/2;
            outH = h/2;
            setter = input->GetPixel(w,h);
            out->SetPixel(setter, outW, outH);
        }
    }
    return;
}

void LRCombine::Execute() {
    int tempW;
    int tempH;
    Pixel setter;
    
    if (input == NULL || input2 == NULL) {
        printf("LRCombine missing input\n");
        char msg[1024];
        sprintf(msg, "%s: missing input", SinkName());
        DataFlowException e(SinkName(), msg);
        throw e;
    }
    if (input->GetHeight() != input2->GetHeight()) {
        char msg[1024];
        sprintf(msg, "%s: heights must match: %d, %d", SinkName(), input->GetHeight(), input2->GetHeight());
        DataFlowException e(SinkName(), msg);
        throw e;
    }



    Image *out = GetOutput();
    out->ResetSize(input->GetWidth()+input2->GetWidth(), input->GetHeight());
    for (int h = 0; h < out->GetHeight(); h++) {
        for (int w = 0; w < out->GetWidth(); w++) {
            if (w%out->GetWidth() < input->GetWidth()) {
                setter = input->GetPixel(w,h);
                out->SetPixel(setter, w, h); 
            }   
            else {
                setter = input2->GetPixel(w-input->GetWidth(), h); 
                out->SetPixel(setter, w, h); 
            }   
        }   
    }   
    return;
}

void TBCombine::Execute() {
    Pixel setter;
    
    if (input == NULL || input2 == NULL) {
        char msg[1024];
        sprintf(msg, "%s: missing input", SinkName());
        DataFlowException e(SinkName(), msg);
        throw e;
    }
    if (input->GetWidth() != input2->GetWidth()) {
        char msg[1024];
        sprintf(msg, "%s: widths must match: %d, %d", SinkName(), input->GetWidth(), input2->GetWidth());
        DataFlowException e(SinkName(), msg);
        throw e;
    }

    Image *out = GetOutput();
    out->ResetSize(input->GetWidth(), input->GetHeight()+input2->GetHeight());
    for (int h = 0; h < out->GetHeight(); h++) {
        for (int w = 0; w < out->GetWidth(); w++) {
            if (h < input->GetHeight()) {
                setter = input->GetPixel(w,h);
                out->SetPixel(setter, w, h); 
            }   
            else {
                setter = input2->GetPixel(w,h-input->GetHeight());
                out->SetPixel(setter, w, h); 
            }   
        }   
    }
    return;
}

void Blender::Execute() {
    Pixel setter;
    Pixel pix1;
    Pixel pix2;
    
    if (input == NULL || input2 == NULL) {
        char msg[1024];
        sprintf(msg, "%s: missing input", SinkName());
        DataFlowException e(SinkName(), msg);
        throw e;
    }
    if (input->GetHeight() != input2->GetHeight()) {
        char msg[1024];
        sprintf(msg, "%s: heights must match: %d, %d", SinkName(), input->GetHeight(), input2->GetHeight());
        DataFlowException e(SinkName(), msg);
        throw e;
    }   
    if (input->GetWidth() != input2->GetWidth()) {
        char msg[1024];
        sprintf(msg, "%s: widths must match: %d, %d", SinkName(), input->GetWidth(), input2->GetWidth());
        DataFlowException e(SinkName(), msg);
        throw e;
    }   

    if (factor >= 1 || factor < 0) {
        char msg[1024];
        sprintf(msg, "%s: invalid factor for Blender: %d", SinkName(), factor);
        DataFlowException e(SinkName(), msg);
        throw e;
    }
    
    Image *out = GetOutput();
    out->ResetSize(input->GetWidth(), input->GetHeight());
    for (int h = 0; h < out->GetHeight(); h++) {
        for (int w = 0; w < out->GetWidth(); w++) {
            pix1 = input->GetPixel(w,h);
            pix2 = input2->GetPixel(w,h);
            setter.red = pix1.red*factor + pix2.red*(1-factor);
            setter.green = pix1.green*factor + pix2.green*(1-factor);
            setter.blue = pix1.blue*factor + pix2.blue*(1-factor);
            out->SetPixel(setter, w, h); 
        }   
    } 
    return;
}

