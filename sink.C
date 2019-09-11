#include <sink.h>
#include <logging.h>
#include <image.h>

void Sink::SetInput(Image *img) {
    if (img == input) {
        char msg[1024];
        sprintf(msg, "%s: input and output must differ", "PNMreader"); 
        DataFlowException e("PNMreader", msg);
        throw e;
    }
    input = img;
}

void Sink::SetInput2(Image *img) {
    input2 = img;
}
