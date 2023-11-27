//
//  cube.h
//  test
//
//  Created by Nazirul Hasan on 4/10/23.
//

#ifndef cube_h
#define cube_h

#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"

using namespace std;

class Cube {
public:

    // materialistic property
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    // texture property
    float TXmin = 0.0f;
    float TXmax = 1.0f;
    float TYmin = 0.0f;
    float TYmax = 1.0f;
    unsigned int diffuseMap;
    unsigned int specularMap;

    // common property
    float shininess;

    // constructors
    Cube()
    {
        setUpCubeVertexDataAndConfigureVertexAttribute();
    }

    Cube(glm::vec3 amb)
    {
        this->ambient = amb;
        this->diffuse = amb;
        this->specular = glm::vec3(0.5, 0.5, 0.5);
        this->shininess = 32;

        setUpCubeVertexDataAndConfigureVertexAttribute();
    }

    Cube(unsigned int dMap, unsigned int sMap, float shiny, float textureXmin, float textureYmin, float textureXmax, float textureYmax)
    {
        this->diffuseMap = dMap;
        this->specularMap = sMap;
        this->shininess = shiny;
        this->TXmin = textureXmin;
        this->TYmin = textureYmin;
        this->TXmax = textureXmax;
        this->TYmax = textureYmax;

        setUpCubeVertexDataAndConfigureVertexAttribute();
    }

    // destructor
    ~Cube()
    {
        glDeleteVertexArrays(1, &cubeVAO);
        glDeleteVertexArrays(1, &lightCubeVAO);
        glDeleteVertexArrays(1, &lightTexCubeVAO);
        glDeleteBuffers(1, &cubeVBO);
        glDeleteBuffers(1, &cubeEBO);
    }


    void drawCubeWithTexture(Shader& lightingShaderWithTexture, glm::mat4 model = glm::mat4(1.0f))
    {
        lightingShaderWithTexture.use();

        lightingShaderWithTexture.setInt("material.diffuse", 0);
        lightingShaderWithTexture.setInt("material.specular", 1);
        lightingShaderWithTexture.setFloat("material.shininess", this->shininess);


        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->diffuseMap);
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, this->specularMap);

        lightingShaderWithTexture.setMat4("model", model);

        glBindVertexArray(lightTexCubeVAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }

    void drawCubeWithMaterialisticProperty(Shader& lightingShader, glm::mat4 model = glm::mat4(1.0f))
    {
        lightingShader.use();

        lightingShader.setVec3("material.ambient", this->ambient);
        lightingShader.setVec3("material.diffuse", this->diffuse);
        lightingShader.setVec3("material.specular", this->specular);
        lightingShader.setFloat("material.shininess", this->shininess);

        lightingShader.setMat4("model", model);

        glBindVertexArray(lightCubeVAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }

    void drawCube(Shader& shader, glm::mat4 model = glm::mat4(1.0f), float r = 1.0f, float g = 1.0f, float b = 1.0f)
    {
        shader.use();

        shader.setVec3("color", glm::vec3(r, g, b));
        shader.setMat4("model", model);

        glBindVertexArray(cubeVAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }

    void setMaterialisticProperty(glm::vec3 amb)
    {
        this->ambient = amb;
        this->diffuse = amb;
        this->specular = glm::vec3(0.5, 0.5, 0.5);
        this->shininess = 32;
    }

    void setTextureProperty(unsigned int dMap, unsigned int sMap, float shiny)
    {
        this->diffuseMap = dMap;
        this->specularMap = sMap;
        this->shininess = shiny;
    }

    void initiate(vector<float>points)
    {
        cntrlPoints = points;
        bezierVAO = hollowBezier(cntrlPoints.data(), ((unsigned int)cntrlPoints.size() / 3) - 1);
    }

    void drawObject(Shader& lightingShader, glm::mat4 model = glm::mat4(1.0f))
    {
        lightingShader.use();

        lightingShader.setVec3("material.ambient", this->ambient);
        lightingShader.setVec3("material.diffuse", this->diffuse);
        lightingShader.setVec3("material.specular", this->specular);
        lightingShader.setFloat("material.shininess", this->shininess);

        lightingShader.setMat4("model", model);

        glBindVertexArray(bezierVAO);
        glDrawElements(GL_TRIANGLES, vertices.size(), GL_UNSIGNED_INT, 0);
    }
    void drawObjectFilled(Shader& lightingShader, glm::mat4 model = glm::mat4(1.0f))
    {
        lightingShader.use();

        lightingShader.setVec3("material.ambient", this->ambient);
        lightingShader.setVec3("material.diffuse", this->diffuse);
        lightingShader.setVec3("material.specular", this->specular);
        lightingShader.setFloat("material.shininess", this->shininess);

        lightingShader.setMat4("model", model);

        glBindVertexArray(bezierVAO);
        glDrawElements(GL_TRIANGLE_STRIP, vertices.size(), GL_UNSIGNED_INT, 0);
    }

private:
    unsigned int cubeVAO;
    unsigned int lightCubeVAO;
    unsigned int lightTexCubeVAO;
    unsigned int cubeVBO;
    unsigned int cubeEBO;
    vector <float> cntrlPoints;
    vector <float> coordinates;
    vector <float> normals;
    vector <int> indices;
    vector <float> vertices;
    unsigned int bezierVAO;
    const double pi = 3.14159265389;
    const int nt = 40;
    const int ntheta = 20;


    long long nCr(int n, int r)
    {
        if (r > n / 2)
            r = n - r; // because C(n, r) == C(n, n - r)
        long long ans = 1;
        int i;

        for (i = 1; i <= r; i++)
        {
            ans *= n - r + i;
            ans /= i;
        }

        return ans;
    }

    //polynomial interpretation for N points
    void BezierCurve(double t, float xy[2], GLfloat ctrlpoints[], int L)
    {
        double y = 0;
        double x = 0;
        t = t > 1.0 ? 1.0 : t;
        for (int i = 0; i < L + 1; i++)
        {
            long long ncr = nCr(L, i);
            double oneMinusTpow = pow(1 - t, double(L - i));
            double tPow = pow(t, double(i));
            double coef = oneMinusTpow * tPow * ncr;
            x += coef * ctrlpoints[i * 3];
            y += coef * ctrlpoints[(i * 3) + 1];

        }
        xy[0] = float(x);
        xy[1] = float(y);
    }

    unsigned int hollowBezier(GLfloat ctrlpoints[], int L)//
    {
        int i, j;
        float x, y, z, r;                //current coordinates
        float theta;
        float nx, ny, nz, lengthInv;    // vertex normal


        const float dtheta = 2 * pi / ntheta;        //angular step size

        float t = 0;
        float dt = 1.0 / nt;
        float xy[2];

        for (i = 0; i <= nt; ++i)              //step through y
        {
            BezierCurve(t, xy, ctrlpoints, L);
            r = xy[0];
            y = xy[1];
            theta = 0;
            t += dt;
            lengthInv = 1.0 / r;

            for (j = 0; j <= ntheta; ++j)
            {
                double cosa = cos(theta);
                double sina = sin(theta);
                z = r * cosa;
                x = r * sina;

                coordinates.push_back(x);
                coordinates.push_back(y);
                coordinates.push_back(z);

                // normalized vertex normal (nx, ny, nz)
                // center point of the circle (0,y,0)
                nx = (x - 0) * lengthInv;
                ny = (y - y) * lengthInv;
                nz = (z - 0) * lengthInv;
                if (j <= 2 * ntheta / 3 + 1 && j >= ntheta / 4)
                {
                    nx = -nx;
                    ny = -ny;
                    nz = -nz;
                }
                normals.push_back(nx);
                normals.push_back(ny);
                normals.push_back(nz);

                theta += dtheta;
            }
        }

        // generate index list of triangles
        // k1--k1+1
        // |  / |
        // | /  |
        // k2--k2+1

        int k1, k2;
        for (int i = 0; i < nt; ++i)
        {
            k1 = i * (ntheta + 1);     // beginning of current stack
            k2 = k1 + ntheta + 1;      // beginning of next stack

            for (int j = 0; j < ntheta; ++j, ++k1, ++k2)
            {
                // k1 => k2 => k1+1
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);

                // k1+1 => k2 => k2+1
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }

        size_t count = coordinates.size();
        for (int i = 0; i < count; i += 3)
        {
            vertices.push_back(coordinates[i]);
            vertices.push_back(coordinates[i + 1]);
            vertices.push_back(coordinates[i + 2]);

            vertices.push_back(normals[i]);
            vertices.push_back(normals[i + 1]);
            vertices.push_back(normals[i + 2]);
        }

        unsigned int bezierVAO;
        glGenVertexArrays(1, &bezierVAO);
        glBindVertexArray(bezierVAO);

        // create VBO to copy vertex data to VBO
        unsigned int bezierVBO;
        glGenBuffers(1, &bezierVBO);
        glBindBuffer(GL_ARRAY_BUFFER, bezierVBO);           // for vertex data
        glBufferData(GL_ARRAY_BUFFER,                   // target
            (unsigned int)vertices.size() * sizeof(float), // data size, # of bytes
            vertices.data(),   // ptr to vertex data
            GL_STATIC_DRAW);                   // usage

        // create EBO to copy index data
        unsigned int bezierEBO;
        glGenBuffers(1, &bezierEBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bezierEBO);   // for index data
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,           // target
            (unsigned int)indices.size() * sizeof(unsigned int),             // data size, # of bytes
            indices.data(),               // ptr to index data
            GL_STATIC_DRAW);                   // usage

        // activate attrib arrays
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        // set attrib arrays with stride and offset
        int stride = 24;     // should be 24 bytes
        glVertexAttribPointer(0, 3, GL_FLOAT, false, stride, (void*)0);
        glVertexAttribPointer(1, 3, GL_FLOAT, false, stride, (void*)(sizeof(float) * 3));

        // unbind VAO, VBO and EBO
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        return bezierVAO;
    }



    void setUpCubeVertexDataAndConfigureVertexAttribute()
    {
        // set up vertex data (and buffer(s)) and configure vertex attributes
        // ------------------------------------------------------------------

        float cube_vertices[] = {
            // positions      // normals         // texture
            0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, TXmax, TYmin,
            0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, TXmin, TYmin,
            0.5f, 0.5f, 0.0f, 0.0f, 0.0f, -1.0f, TXmin, TYmax,
            0.0f, 0.5f, 0.0f, 0.0f, 0.0f, -1.0f, TXmax, TYmax,

            0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, TXmax, TYmin,
            0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, TXmax, TYmax,
            0.5f, 0.0f, 0.5f, 1.0f, 0.0f, 0.0f, TXmin, TYmin,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, TXmin, TYmax,

            0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 1.0f, TXmin, TYmin,
            0.5f, 0.0f, 0.5f, 0.0f, 0.0f, 1.0f, TXmax, TYmin,
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, TXmax, TYmax,
            0.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, TXmin, TYmax,

            0.0f, 0.0f, 0.5f, -1.0f, 0.0f, 0.0f, TXmax, TYmin,
            0.0f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, TXmax, TYmax,
            0.0f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, TXmin, TYmax,
            0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, TXmin, TYmin,

            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, TXmax, TYmin,
            0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, TXmax, TYmax,
            0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, TXmin, TYmax,
            0.0f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, TXmin, TYmin,

            0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, TXmin, TYmin,
            0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, TXmax, TYmin,
            0.5f, 0.0f, 0.5f, 0.0f, -1.0f, 0.0f, TXmax, TYmax,
            0.0f, 0.0f, 0.5f, 0.0f, -1.0f, 0.0f, TXmin, TYmax
        };
        unsigned int cube_indices[] = {
            0, 3, 2,
            2, 1, 0,

            4, 5, 7,
            7, 6, 4,

            8, 9, 10,
            10, 11, 8,

            12, 13, 14,
            14, 15, 12,

            16, 17, 18,
            18, 19, 16,

            20, 21, 22,
            22, 23, 20
        };

        glGenVertexArrays(1, &cubeVAO);
        glGenVertexArrays(1, &lightCubeVAO);
        glGenVertexArrays(1, &lightTexCubeVAO);
        glGenBuffers(1, &cubeVBO);
        glGenBuffers(1, &cubeEBO);


        glBindVertexArray(lightTexCubeVAO);

        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // vertex normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)12);
        glEnableVertexAttribArray(1);

        // texture coordinate attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)24);
        glEnableVertexAttribArray(2);


        glBindVertexArray(lightCubeVAO);

        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)12);
        glEnableVertexAttribArray(1);


        glBindVertexArray(cubeVAO);

        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }

};


#endif /* cube_h */
