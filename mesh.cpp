#include "mesh.h"

Vertex::Vertex() {
    position = glm::vec3(0.f, 0.f, 0.f);
    color = glm::vec4(0.f, 0.f, 0.f, 0.f);
    normal = glm::vec3(0.f, 0.f, 0.f);
    texCoord = glm::vec2(0.f, 0.f);

    index = 0;
}

Vertex::~Vertex() {}

Mesh::Mesh() {}

Mesh::~Mesh() {}

Triangle::Triangle() : Mesh() {}

Triangle::~Triangle() {}

void Triangle::setPositions(glm::vec3 a, glm::vec3 b, glm::vec3 c) {
    positions.push_back(a);
    positions.push_back(b);
    positions.push_back(c);
}

void Triangle::setColor(glm::vec4 c) {
    colors.push_back(c);
    colors.push_back(c);
    colors.push_back(c);
}

Quadrilateral::Quadrilateral() : Mesh() {}

Quadrilateral::~Quadrilateral() {}

void Quadrilateral::setPositions(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d) {
    positions.push_back(a);
    positions.push_back(b);
    positions.push_back(c);

    positions.push_back(a);
    positions.push_back(c);
    positions.push_back(d);
}

void Quadrilateral::setColor(glm::vec4 c) {
    colors.push_back(c);
    colors.push_back(c);
    colors.push_back(c);

    colors.push_back(c);
    colors.push_back(c);
    colors.push_back(c);
}
