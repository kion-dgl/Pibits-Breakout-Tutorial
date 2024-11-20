### **Paddle Movement**

*   **Why**: Players control the paddle, so you’ll need to handle input to move it left and right.
*   **How**: Use SDL to capture key events for moving the paddle based on arrow keys or X-input.

**Next Step:**

*   Capture key inputs to move the paddle.
*   Clamp the paddle’s position to the screen’s width so it doesn’t go off-screen.

Example:

```c
const Uint8* state = SDL_GetKeyboardState(NULL);
if (state[SDL_SCANCODE_LEFT]) {
    paddle.x -= paddleSpeed * deltaTime;
}
if (state[SDL_SCANCODE_RIGHT]) {
    paddle.x += paddleSpeed * deltaTime;
}
```
