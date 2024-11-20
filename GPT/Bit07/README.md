### **Draw Bricks in a Grid**

*   **Why**: Bricks are a core element of _Brickout_, and you’ll want to render multiple bricks arranged in a grid.
*   **How**: Create an array of `Brick` structs and render them in rows and columns.

**Next Step:**

*   Define a grid size and dynamically generate a set of bricks.
*   Draw each brick as a colored rectangle.

Example:

```c
int numRows = 5;
int numCols = 10;
Brick bricks[numRows * numCols];
// Initialize brick positions in a loop
for (int i = 0; i < numRows; ++i) {
    for (int j = 0; j < numCols; ++j) {
        bricks[i * numCols + j].x = j * brickWidth;
        bricks[i * numCols + j].y = screenHeight - (i + 1) * brickHeight;
    }
}
```
