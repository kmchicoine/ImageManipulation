#include <source.h>
#include <image.h>
#include <stdio.h>

Image * Source::GetOutput() {
    return &output;
}

Source::Source() {
    output.SetSource(this);
}
