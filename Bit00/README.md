## Explanation

- `SDL_Init` initializes the SDL library, specifically the video subsystem (SDL_INIT_VIDEO).
- `SDL_CreateWindow` creates a window of size 800x480 with the title "Brickout - Pibit" in the center of the screen. The SDL_WINDOW_OPENGL flag sets the window up for use with OpenGL.
- `SDL_GL_CreateContext` creates an OpenGL context associated with the window.
- The main loop handles events (like closing the window) and clears the screen with glClear(GL_COLOR_BUFFER_BIT) each frame.
- `SDL_GL_SwapWindow` swaps the OpenGL buffers to update the window’s content.
- Finally, it cleans up by destroying the OpenGL context, window, and calling SDL_Quit().

### Compiling:

You can compile the code on your Raspberry Pi with the following command:

```bash
gcc -o brickout brickout.c -lSDL2 -lGL
```

This compiles the program, linking against SDL2 (-lSDL2) and OpenGL (-lGL), and creates an executable named brickout.
