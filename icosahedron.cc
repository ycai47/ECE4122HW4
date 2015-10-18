// Draw an Icosahedron
// ECE4893/8893 Project 4
// Yushan Cai

#include <iostream>
#include <math.h>
#include <GL/glut.h>
#include <GL/glext.h>
#include <GL/gl.h>
#include <GL/glu.h>

using namespace std;

#define NFACE 20
#define NVERTEX 12

#define X .525731112119133606 
#define Z .850650808352039932

// These are the 12 vertices for the icosahedron
static GLfloat vdata[NVERTEX][3] = {    
  {-X, 0.0, Z}, {X, 0.0, Z}, {-X, 0.0, -Z}, {X, 0.0, -Z},
  {0.0, Z, X}, {0.0, Z, -X}, {0.0, -Z, X}, {0.0, -Z, -X},
  {Z, X, 0.0}, {-Z, X, 0.0}, {Z, -X, 0.0}, {-Z, -X, 0.0} 
};

// These are the 20 faces.  Each of the three entries for each 
// vertex gives the 3 vertices that make the face.
static GLint tindices[NFACE][3] = {
  {0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},
  {8,10,1}, {8,3,10}, {5,3,8}, {5,2,3}, {2,7,3},
  {7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6},
  {6,1,10}, {9,0,11}, {9,11,2}, {9,2,5}, {7,2,11} };

int testNumber; // Global variable indicating which test number is desired
int depth;
const int default_num_faces = 20; // Default of 20 faces

// Helper class definitions
class Point
{
public:
  Point()
    :x(0.0), y(0.0), z(0.0)
    {};
  Point(GLfloat a, GLfloat b, GLfloat c)
    :x(a), y(b), z(c)
    {};
  ~Point(){};
  Point(const Point& p)
  {
    x = p.x;
    y = p.y;
    z = p.z;
  } //copy constructor
  Point operator+(const Point& rhs)
  {
    return Point(x + rhs.x, y + rhs.y, z + rhs.z);
  } // + operator
  Point operator/(const GLfloat& i)
  {
    return Point(x / i, y / i, z / i);
  } // / operator
  void normalize()
  {
    GLfloat l = sqrt(x * x + y * y + z * z);
    x = x / l;
    y = y / l;
    z = z / l;
  } // normalize vector
  void setPoint(const GLfloat* v)
  {
    x = v[0];
    y = v[1];
    z = v[2];
  } //set values
  GLfloat getX()
  {
    return x;
  }
  GLfloat getY()
  {
    return y;
  }
  GLfloat getZ()
  {
    return z;
  } //get values
private:
  GLfloat x, y, z;
};

class Triangle
{
public:
  Triangle(){};
  ~Triangle(){};
  void setVertex(const Point& vertex0, const Point& vertex1, const Point& vertex2)
  {
    this->vertex[0] = vertex0;
    this->vertex[1] = vertex1;
    this->vertex[2] = vertex2;
  }
  void setVertex(const GLint& v0,const GLint& v1,const GLint& v2)
  {
    this->vertex[0].setPoint(vdata[v0]);
    this->vertex[1].setPoint(vdata[v1]);
    this->vertex[2].setPoint(vdata[v2]);
  }
  Point getVertex(int index)
  {
    return vertex[index];
  }
private:
  Point vertex[3];
};

class Icosahedron
{
public:
  Icosahedron():num(default_num_faces)
  {
    triangle = new Triangle[num];
  };
  ~Icosahedron()
  {
    delete [] triangle;
  }
  void reset(Triangle* trilist, int length)
  {
    num = length;
    triangle = new Triangle[num];
    triangle = trilist;
  } //reset triangle for expand
  void setTriangle(int index, Triangle* tri)
  {
    triangle[index] = *tri;
  }
  Triangle getTriangle(int index)
  {
    return triangle[index];
  }
  int getFaces()
  {
    return num;
  }
private:
  Triangle * triangle;
  int num;
};

Icosahedron icosahedron;

void init()
{
  glClearColor(0.0, 0.0, 0.0, 0.0); //background color of black
  glShadeModel(GL_FLAT); //flat shading
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_POLYGON_SMOOTH);
}

void callback(int arg)
{
  glutPostRedisplay();
  glutTimerFunc(100.0, callback, 0);
}

void reshape(int w, int h)
{
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, (GLdouble)w, (GLdouble)0.0, h, (GLdouble)w, (GLdouble)-w);
  //glFrustum(0.0, (GLdouble)w, (GLdouble)0.0, h, (GLdouble)1, (GLdouble)40);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
} // from class

void extend(Icosahedron& icos)
{
  int current_num_faces = icosahedron.getFaces();
  int new_num_faces = current_num_faces * 4; // each extention times current number of faces by 4
  Triangle tmp;
  Point vertex0, vertex1, vertex2;
  Triangle * temp_triangles = new Triangle[new_num_faces]; //new set of triangles
  Point vertex01, vertex02, vertex12;
  for(int i = 0; i < current_num_faces; i++) // each triangle will be devided into 4 smaller triangles
  {
    // find original vertexes
    tmp = icos.getTriangle(i);
    vertex0 = tmp.getVertex(0);
    vertex1 = tmp.getVertex(1);
    vertex2 = tmp.getVertex(2);
    // find mid point
    vertex01 = (vertex0 + vertex1)/2;
    vertex02 = (vertex0 + vertex2)/2;
    vertex12 = (vertex1 + vertex2)/2;
    // normalize
    vertex01.normalize();
    vertex02.normalize();
    vertex12.normalize();
    // set new vertexes of triangles
    temp_triangles[i*4].setVertex(vertex0,vertex01,vertex02);
    temp_triangles[i*4+1].setVertex(vertex01,vertex02,vertex12);
    temp_triangles[i*4+2].setVertex(vertex1,vertex01,vertex12);
    temp_triangles[i*4+3].setVertex(vertex2,vertex12,vertex02);
  }
  // update the triangle array in object icosahedron
  icos.reset(temp_triangles, new_num_faces);
  // delete [] temp_triangles;
}

void icosInit(Icosahedron& icos, const int& dep)
{
  int num_faces = icosahedron.getFaces();
  Triangle * tmp = new Triangle();
  for (int i = 0; i < num_faces; ++i)
  {
    tmp->setVertex(tindices[i][0], tindices[i][1], tindices[i][2]);
    icos.setTriangle(i, tmp);
  } // initialize icosahedron object with given indices
  delete tmp;
  if (dep != 0)
  {
    for (int i = 0; i < dep; ++i)
    {
      extend(icos);
    }
  } // apply propor extension
}

void icosDraw()
{
  Triangle tmp;
  Point vertex0, vertex1, vertex2;
  int faces = icosahedron.getFaces();
  // Draw faces
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glColor3f(0.0, 0.0, 1.0); //blue faces
  glBegin(GL_TRIANGLES);
  for(int i = 0; i < faces; i++)
  {
    tmp = icosahedron.getTriangle(i);
    vertex0 = tmp.getVertex(0);
    vertex1 = tmp.getVertex(1);
    vertex2 = tmp.getVertex(2);
    glVertex3f(vertex0.getX(), vertex0.getY(), vertex0.getZ()); //first vertex   
    glVertex3f(vertex1.getX(), vertex1.getY(), vertex1.getZ()); // second vertex
    glVertex3f(vertex2.getX(), vertex2.getY(), vertex2.getZ()); // third vertex
  }
  glEnd();
  // Draw edges
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glColor3f(1.0, 1.0, 1.0);
  glLineWidth(1.0);
  glBegin(GL_TRIANGLES);
  for(int i = 0; i < faces; i++)
  {
    tmp = icosahedron.getTriangle(i);
    vertex0 = tmp.getVertex(0);
    vertex1 = tmp.getVertex(1);
    vertex2 = tmp.getVertex(2);
    glVertex3f(vertex0.getX(), vertex0.getY(), vertex0.getZ()); //first vertex   
    glVertex3f(vertex1.getX(), vertex1.getY(), vertex1.getZ()); // second vertex
    glVertex3f(vertex2.getX(), vertex2.getY(), vertex2.getZ());// third vertex
  }
  glEnd();
}

// Display from front viewing point
void display()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  gluLookAt(0.0, 0.0, 50.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
  glTranslatef(250, 250, 0);
  glScalef(200.0, 200.0, 200.0);

  icosDraw();

  // if (glGetError() != GL_NO_ERROR)
  // {
  //   cout << "Error occured" << endl;
  // }
  glutSwapBuffers();
}

// Add a 10 hertz (update every 100ms) animation rotation about both the x and y axes
void rotate()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  gluLookAt(0.0, 0.0, 70.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
  glTranslatef(250, 250, 0);
  glScalef(200.0, 200.0, 200.0);
  //rotation
  static GLfloat rotX = 0;
  static GLfloat rotY = 0;
  glRotatef(rotX, 1.0, 0.0, 0.0);
  glRotatef(rotY, 0.0, 1.0, 0.0);
  rotX += 1;
  rotY -= 1;

  icosDraw();
  glutSwapBuffers();
}

int main(int argc, char** argv)
{
  if (argc < 2)
  {
    std::cout << "Usage: icosahedron testnumber" << endl;
    exit(1);
  }
  // Set the global test number
  testNumber = atol(argv[1]);

  //Set the depth
  if (testNumber > 4)
  {
    if (argv[2])
    {
      depth = atol(argv[2]);
    }
    else
    {
      cout << "Usage: input depth please" << endl;
      exit(1);
    }
  }
  
  // Initialize glut and create window
  glutInit(&argc,argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(500, 500);
  glutInitWindowPosition(250, 250);
  glutCreateWindow("Icosahedron");
  init();
  // Set glut callbacks
  switch(testNumber)
  {
    case 1:
      icosInit(icosahedron, 0);
      glutDisplayFunc(display);
      break;
    case 2:
      icosInit(icosahedron, 0);
      glutDisplayFunc(rotate);
      break;
    case 3:
      icosInit(icosahedron, 1);
      glutDisplayFunc(display);
      break;
    case 4:
      icosInit(icosahedron, 1);
      glutDisplayFunc(rotate);
      break;
    case 5:
      icosInit(icosahedron, depth);
      glutDisplayFunc(display);
      break;
    case 6:
      icosInit(icosahedron, depth);
      glutDisplayFunc(rotate);
      break;
    default:
    cout << "test number unknown, enter from 1 to 6:" << endl;
  }
  glutReshapeFunc(reshape);
  glutTimerFunc(100.0, callback, 0);
  // Enter the glut main loop
  glutMainLoop();
  return 0;
}