#include <PNMwriter.h>
#include <stdio.h>
#include <image.h>
#include <string.h>


const char * PNMwriter::SinkName() {
    return "PNMwriter";
}

void PNMwriter::Write(char *filename) {
    char writeArr[50];
    Pixel pix;
    FILE *f_out = fopen(filename, "w");
    sprintf(writeArr, "%s\n%d %d\n%d\n", "P6", input->GetWidth(), input->GetHeight(), 255);
    fwrite(writeArr, strlen(writeArr), 1, f_out);
    for (int h = 0; h < input->GetHeight(); h++) {
        for (int w = 0; w < input->GetWidth(); w++) {
            pix = input->GetPixel(w,h); 
            fwrite(&pix, sizeof(Pixel), 1, f_out);
    
        }
    }   
    fclose(f_out);
    return;
}
