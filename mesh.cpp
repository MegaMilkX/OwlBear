#include "gfx.h"

Mesh::Mesh(Gfx* gfx)
    : gfx(gfx)
{
    glGenVertexArrays(1, &vao);
    Bind();
    vertexBuffer = Au::GFX::GLBuffer::Create(GL_ARRAY_BUFFER, GL_STATIC_DRAW);
    vertexBuffer.Bind();
    indexBuffer = Au::GFX::GLBuffer::Create(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW);
    indexBuffer.Bind();
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &vao);
    vertexBuffer.Destroy();
    indexBuffer.Destroy();
}

void Mesh::SetFormat(const char* fmtdesc)
{
    vfmt = VertexFormat(fmtdesc, gfx);

    Bind();

    size_t offset = 0;
    std::map<std::string, int> instanceCounters;
    int attrIndex = 0;
    for (int i = 0; i < vfmt.attribCount; ++i)
    {
        Attribute attr = vfmt.attributes[i];

        GLboolean normalized = GL_FALSE;

        glVertexAttribPointer(attrIndex,        //*attrib index
            attr.elemCount,                     //*attrib element count
            gfx->ElemToGLenum(attr.elemType),   //*element type
            normalized,                         //*normalize?
            vfmt.vertexSize,                     //*stride
            (void*)offset);                     //*offset
        offset += attr.elemSize * attr.elemCount;
        glEnableVertexAttribArray(attrIndex);

        attrIndex++;
    }
}

void Mesh::SetAttribData(const char* attribName, const unsigned char* data, size_t sz)
{
    Attribute* a = gfx->GetAttribute(attribName);
    if (!a)
        return;
    Attribute attrib = *a;
    
    unsigned newVertexCount = sz / attrib.elemSize / attrib.elemCount;
    unsigned oldVertexCount = 0;

    std::vector<char> newBuffer(newVertexCount * vfmt.vertexSize);

    if (vertexBuffer.Valid())
    {
        std::vector<char> oldData = vertexBuffer.Data();
        oldVertexCount = oldData.size() / vfmt.vertexSize;
        if (oldVertexCount > newVertexCount)
            newBuffer.resize(oldVertexCount * vfmt.vertexSize);
        for (unsigned i = 0; i < oldData.size(); ++i)
            newBuffer[i] = oldData[i];
    }

    BlitAttribDataToBuffer(attrib, data, sz, (unsigned char*)newBuffer.data(), newBuffer.size());

    vertexBuffer.Data((void*)newBuffer.data(), newBuffer.size());
}

void Mesh::SetIndices(const unsigned short* data, int count)
{
    if (count == 0)
        return;

    indexBuffer.Data((unsigned char*)data, sizeof(unsigned short) * count);
    indexCount = count;
}

void Mesh::Render(unsigned int indexCount, unsigned int offset)
{
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, (void*)offset);
}

void Mesh::BlitAttribDataToBuffer(Attribute attrib, const unsigned char* attribData, int attribDataSize, const unsigned char* dest, int destSize)
{
    int attrOffset = 0;
    for (int i = 0; i < vfmt.attribCount; ++i)
    {
        if (vfmt.attributes[i] == attrib)
            break;
        else
            attrOffset += vfmt.attributes[i].elemCount * vfmt.attributes[i].elemSize;
    }

    if (attrOffset >= vfmt.vertexSize)
        return;

    unsigned vertexCount = attribDataSize / attrib.elemSize / attrib.elemCount;
    for (unsigned i = 0; i < vertexCount; ++i)
    {
        char* cursor = (char*)(dest + i * vfmt.vertexSize);
        cursor += attrOffset;
        for (int j = 0; j < attrib.elemCount; ++j)
            for(int k = 0; k < attrib.elemSize; ++k)
                *(((unsigned char*)cursor) + j + k) = attribData[i * attrib.elemSize * attrib.elemCount + j + k];
    }
}

void Mesh::Bind()
{
    glBindVertexArray(vao);
}