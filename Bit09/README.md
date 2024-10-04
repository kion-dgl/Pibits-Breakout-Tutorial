### **Game States and Win/Lose Conditions**

*   **Why**: You’ll need a system to track whether the player has won (cleared all bricks) or lost (ball falls below the screen).
*   **How**: Implement basic game states to track these conditions.

**Next Step:**

*   Detect when the ball goes below the paddle (player loses).
*   Detect when all bricks are destroyed (player wins).

Example:

```c
if (ball.y < 0) {
    // Game over logic
}

// Check if all bricks are destroyed
bool allBricksDestroyed = true;
for (int i = 0; i < numBricks; ++i) {
    if (bricks[i].health > 0) {
        allBricksDestroyed = false;
        break;
    }
}

if (allBricksDestroyed) {
    // Player wins
}
```
