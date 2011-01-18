// CUDA KI: OpenGL template

#include <cstdio>
#include <GL/glut.h>
#include <GL/glext.h>
#include <math.h>
#include <sys/time.h>

struct timeval start_time;
int size_x, size_y;

GLuint glprog;
GLuint vp, fp;

float get_time()
{
  struct timeval frame_time;
  gettimeofday(&frame_time, NULL);
  return (float)(frame_time.tv_sec - start_time.tv_sec) + (float)frame_time.tv_usec/1000000.0f;

}

void reshape(int w, int h)
{
  size_x = w;
  size_y = h;
  glViewport(0, 0, w, h);
}

char* readfile(const char *filename)
{
  char* buf;
  size_t flen;

  FILE *f = fopen(filename, "r");
  if(!f) return NULL;

  fseek(f, 0, SEEK_END);
  flen = ftell(f);
  fseek(f, 0, SEEK_SET);

  buf = new char[flen+1];
  fread(buf, flen, 1, f);
  buf[flen] = 0;
  fclose(f);
  return buf;
}

int init()
{
  const GLchar *vsrc, *fsrc;

  glprog = glCreateProgram();
  vp = glCreateShader(GL_VERTEX_SHADER);
  fp = glCreateShader(GL_FRAGMENT_SHADER);
  
  vsrc = readfile("vertex.glsl");
  fsrc = readfile("fragment.glsl");
  glShaderSource(vp, 1, &vsrc, NULL);
  glShaderSource(fp, 1, &fsrc, NULL);
  delete[] vsrc;
  delete[] fsrc;

  int status, loglen;
  char* log = new char[16384];
  glCompileShader(vp);
  glGetShaderiv(vp, GL_COMPILE_STATUS, &status);
  if(status != GL_TRUE) {
    glGetShaderInfoLog(vp, 16384, &loglen, log);
    printf("VP LOG:\n\n%s\n", log);
    delete[] log;
    return 1;
  }

  glCompileShader(fp);
  glGetShaderiv(fp, GL_COMPILE_STATUS, &status);
  if(status != GL_TRUE) {
    glGetShaderInfoLog(fp, 16384, &loglen, log);
    printf("FP LOG:\n\n%s\n", log);
    delete[] log;
    return 1;
  }

  glAttachShader(glprog, vp);
  glAttachShader(glprog, fp);
  glLinkProgram(glprog);

  glGetProgramiv(glprog, GL_LINK_STATUS, &status);
  if(status != GL_TRUE) {
    glGetProgramInfoLog(glprog, 16384, &loglen, log);
    printf("LINK LOG:\n\n%s\n", log);
    delete[] log;
    return 1;
  }
  delete[] log;
  return 0;
}

void cleanup()
{
  glUseProgram(0);
  glDeleteShader(vp);
  glDeleteShader(fp);
  glDeleteProgram(glprog);
}

void display()
{
  GLint time, w, h;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(glprog);
  time = glGetUniformLocation(glprog, "time");
  w    = glGetUniformLocation(glprog, "w");
  h    = glGetUniformLocation(glprog, "h");

  glUniform1f(time, get_time());
  glUniform1i(w, size_x);
  glUniform1i(h, size_y);
  
  glBegin(GL_QUADS);
    glTexCoord2i(0, 0);
    glVertex2i(0, 0);
    glTexCoord2i(0, 1);
    glVertex2i(0, 1);
    glTexCoord2i(1, 1);
    glVertex2i(1, 1);
    glTexCoord2i(1, 0);
    glVertex2i(1, 0);
  glEnd();
  glUseProgram(0);

  glutSwapBuffers();
}

int main(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitWindowSize(800, 600);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
  glutCreateWindow("GL");

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutIdleFunc(display);

  gettimeofday(&start_time, NULL);

  if(init() != 0)
    return 1;

  glutMainLoop();
  cleanup();
  return 0;
}
