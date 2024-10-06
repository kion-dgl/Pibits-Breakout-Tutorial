### **Define Game Objects (Ball, Paddle, Bricks)**

*   **Why**: You’ll need to encapsulate game objects like the ball, paddle, and bricks into C structures that hold their position, size, velocity, etc.
*   **How**: Start by defining these objects with appropriate fields and methods for rendering and updating them.

**Next Step:**

*   Define a **Ball struct** with position, velocity, and size.
*   Define a **Paddle struct** with position and size.
*   Define a **Brick struct** with position and health (number of hits to break).

Example:

```c
typedef struct {
    float x, y;
    float width, height;
    float velocityX, velocityY;
} Ball;

typedef struct {
    float x, y;
    float width, height;
} Paddle;

typedef struct {
    float x, y;
    float width, height;
    int health;
} Brick;
```
