
### **Move the Ball**

* **Why**: One of the core mechanics of _Brickout_ is the ball bouncing around the screen, hitting the paddle and bricks.
* **How**: Implement ball movement using its velocity, updating its position each frame.

**Next Step:**

*   Update the ball’s position in the main loop.
*   Detect if the ball hits the window boundaries (left, right, top) and make it bounce by reversing its velocity.

Example:

```c
ball.x += ball.velocityX * deltaTime;
ball.y += ball.velocityY * deltaTime;

if (ball.x <= 0 || ball.x + ball.width >= screenWidth) {
    ball.velocityX = -ball.velocityX;
}
if (ball.y >= screenHeight) {
    ball.velocityY = -ball.velocityY;
}
```
