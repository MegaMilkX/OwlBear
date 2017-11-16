#ifndef ITEXTURE2D_H
#define ITEXTURE2D_H

class ITexture2D
{
public:
    virtual void Data(unsigned char* data, int bpp, int width, int height) = 0;
};

#endif
