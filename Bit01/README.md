### Explanation of Updates:
1. **OpenGL ES 2.0 Initialization:**
   - `SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);` and `SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);`
     ensure that OpenGL ES 2.0 is used.
   - The profile mask is set to `SDL_GL_CONTEXT_PROFILE_ES` to use OpenGL ES.

2. **Shader Program:**
   - A simple **vertex shader** (`vertexShaderSource`) positions the triangle vertices.
   - A **fragment shader** (`fragmentShaderSource`) colors the triangle blue (`vec4(0.0, 0.0, 1.0, 1.0)`).

3. **Triangle Setup:**
   - The triangle vertices are defined in `vertices[]`.
   - The triangle is drawn using `glDrawArrays(GL_TRIANGLES, 0, 3)` in the main loop.

4. **GLEW Initialization:**
   - `glewExperimental = GL_TRUE;` ensures GLEW uses the latest techniques.
   - `glewInit()` initializes GLEW after creating the OpenGL context.

### Coordinates Used in OpenGL

In OpenGL, the coordinate system used for rendering is a **Cartesian coordinate system** in 3D space. Here's a breakdown of the coordinate systems and how they work in different stages of the rendering process:

### 1. **Normalized Device Coordinates (NDC):**

At the heart of OpenGL’s rendering pipeline is the **Normalized Device Coordinate (NDC)** system. This system defines a 3D space in which all coordinates are within a range of `[-1, 1]` for each axis:

- **X-axis**: Left is `-1`, and right is `+1`.
- **Y-axis**: Bottom is `-1`, and top is `+1`.
- **Z-axis**: Near is `-1`, and far is `+1`.

Any vertices that lie outside this range are clipped and not rendered.

### 2. **Coordinate System Used in the Vertex Shader:**

In the vertex shader, the coordinates you provide are transformed into NDCs. In the case of 2D graphics (like your triangle), you typically work in **clip space**, where the coordinates should fit within this normalized `[-1, 1]` range so that the object is visible on the screen.

For example, in your code, the triangle vertices are defined as follows:

```c
GLfloat vertices[] = {
    0.0f,  0.5f, 0.0f,  // Top vertex
  -0.5f, -0.5f, 0.0f,  // Bottom-left vertex
    0.5f, -0.5f, 0.0f   // Bottom-right vertex
};
```

Explanation of the Coordinates:

- The X-axis represents horizontal positioning:
  - -1.0 is the leftmost part of the screen.
  - 1.0 is the rightmost part of the screen.
- The Y-axis represents vertical positioning:
  - -1.0 is the bottom of the screen.
  - 1.0 is the top of the screen.

The Z-axis represents depth (for 3D rendering), but in this case, since you’re working in 2D, the Z-coordinate is always 0.0 (indicating that the object lies directly on the screen's plane).
