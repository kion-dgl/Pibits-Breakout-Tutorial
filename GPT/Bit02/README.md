
### **Set Up Orthographic Projection**

*   **Why**: Brickout is a 2D game, so you’ll need an orthographic projection (2D coordinate system) instead of the default 3D perspective.
*   **How**: You’ll modify the vertex shader to work with orthographic coordinates, which map your 2D game objects to screen space.

**Next Step:**

*   Implement a simple orthographic projection matrix in your vertex shader.
*   Map your game objects (like paddles, balls, and bricks) to fit within the visible screen.

Example:

```c
// Simple orthographic projection setup in 2D
glm::mat4 projection = glm::ortho(0.0f, screenWidth, 0.0f, screenHeight);
```
