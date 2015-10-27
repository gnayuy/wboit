// created 10/27/2015 by Yang Yu <gnayuy@gmail.com>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"

/// Vertex
class Vertex
{
public:
    Vertex();
    ~Vertex();

public:
    glm::vec3 position;
    glm::vec4 color;
    glm::vec3 normal;
    glm::vec2 texCoord;

    unsigned int index;
};

/// Mesh
class Mesh
{
public:
    Mesh();
    ~Mesh();

public:
    std::vector<glm::vec3> positions;
    std::vector<glm::vec4> colors;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;

    std::vector<unsigned int> indices;

    std::vector<Vertex> vertices; //
};

/// Triangle
class Triangle : public Mesh
{
public:
    Triangle();
    ~Triangle();

public:
    void setPositions(glm::vec3 a, glm::vec3 b, glm::vec3 c);
    void setColor(glm::vec4 c);

};

/// Quadrilateral
//  d_______c
//   |      |
//   |      |
//  a|______|b
//
// triangle (a,b,c) + triangle (a,c,d)
//
class Quadrilateral : public Mesh
{
public:
    Quadrilateral();
    ~Quadrilateral();

public:
    void setPositions(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d);
    void setColor(glm::vec4 c);
};


