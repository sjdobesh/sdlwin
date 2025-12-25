# **win**
a window interface for sdl2 and opengl in c

## dependencies
SDL2 and OpenGL

## usage
```c
git clone https://github.com/sjdobesh/win.git
cd win
./install
```
## data structures
### shader
```c
struct shader {
  GLuint gl_ptr; // open gl ptr for the object
  char* name;    // file path or user given name
  int bound;     // bool
}l
```
### shader collection
```c
struct shader_collection {
  shader* shader; // list of shaders
  int count;      // number of shaders stored
}l
```
### win
A win struct is the master container for all window data.
```c
struct win {
  SDL_Window*       window;
  SDL_GLContext     context;
  shader_collection shader_programs, vert_shaders, frag_shaders;
  shader*           shader_program, vert_shader, frag_shader;
  int w, h;
};
```
## window interface
```c
win init_win(int w, int h); // initialize a window structure
int load_vert(win*, "file/path/vert:");
int load_frag(win*, "file/path/frag:");
int compile_shader(win*, "name");
int bind_shader(win*, "name");
int render(win);
```
