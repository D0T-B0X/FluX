#ifndef BUFFER_H
#define BUFFER_H

#include <glad/glad.h>
#include <cstddef> // for size_t

struct Buffer {
    GLuint            bufferID;
    GLuint            bufferBindBase;
    size_t            bufferDataSize;
    const void*       bufferData;
};

#endif
