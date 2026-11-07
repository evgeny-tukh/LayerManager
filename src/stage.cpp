#include "stage.hpp"

Stage::Stage(const Stage& source) {
    transparent = source.transparent;
    fill = source.fill;
    size = source.size;
    shift = source.shift;
    image = source.image.clone();
    pixelBuf.insert(pixelBuf.begin(), source.pixelBuf.begin(), source.pixelBuf.end());
}
