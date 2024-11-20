### **Brick Collision Detection**

*   **Why**: The ball must break bricks upon collision, so you'll need collision detection for the ball and bricks.
*   **How**: Use the same AABB collision technique for the ball-brick collisions, and remove bricks when hit.

**Next Step:**

*   Detect when the ball hits a brick and remove the brick from the grid.
*   Optionally, implement a "health" system for bricks that take multiple hits to break.

Example:

```c
for (int i = 0; i < numBricks; ++i) {
    if (checkCollision(ball, bricks[i])) {
        bricks[i].health--;
        if (bricks[i].health <= 0) {
            // Remove the brick
        }
        ball.velocityY = -ball.velocityY; // Bounce off the brick
    }
}
```
