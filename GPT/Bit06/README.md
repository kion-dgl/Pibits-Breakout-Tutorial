### **Basic Collision Detection (Ball-Paddle)**

*   **Why**: The game needs to detect when the ball hits the paddle and bounce the ball off it.
*   **How**: Implement basic AABB (Axis-Aligned Bounding Box) collision detection to check if the ball intersects the paddle.

**Next Step:**

*   Detect when the ball hits the paddle and reverse the ball’s vertical velocity.
*   Ensure the ball bounces correctly off the paddle based on where it hits.

Example (AABB Collision):

```c
int checkCollision (Ball ball, Paddle paddle) {
    return ball.x < paddle.x + paddle.width &&
            ball.x + ball.width > paddle.x &&
            ball.y < paddle.y + paddle.height &&
            ball.y + ball.height > paddle.y;
}

if (checkCollision(ball, paddle)) {
    ball.velocityY = -ball.velocityY;
}
```
