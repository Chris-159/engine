#ifndef VERTEX_H
#define VERTEX_H

namespace core
{
    struct Vertex
    {
        float x;
        float y;
        float z;

        Vertex() : x(0), y(0), z(0) {}
        Vertex(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
    };
} // namespace vertex

#endif // VERTEX_H