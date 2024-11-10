# Brickout: A Retro Game for the Pibit Console

Welcome to **Brickout**, a classic block-breaking game built for the **Pibit**, a fictional handheld console concept designed to bring retro gaming charm to the modern world! Powered by a **Raspberry Pi Zero 2W**, an **800x480 screen**, and **X-input controls** in a format similar to a Gameboy Advance, Pibit is your gateway to learning and building nostalgic games like *Brickout* using **C**, **OpenGL**, and **SDL**.

This repository contains the full tutorial for programming **Brickout**, making it the perfect first step into the world of graphics programming for the Raspberry Pi.

## 🎮 What is Pibit?

Pibit is a fictional handheld console project designed to inspire makers, coders, and hobbyists to dive into game development on the Raspberry Pi platform. It's a fun, educational journey that helps you learn how to program graphics in **C**, using powerful libraries like OpenGL and SDL, all while emulating a charming retro aesthetic.

## 🔨 What You'll Learn

Throughout this tutorial, you'll go through the following steps:

1. **Introduction** – Get started with the Pibit setup and learn the basics of the project.
2. **Open a Window** – Set up your development environment and create a simple window.
3. **Draw a Triangle** – Your first step into OpenGL rendering.
4. **Separate Shaders** – Organize your shaders for flexible rendering.
5. **Shader Program** – Compile and link shaders into a reusable program.
6. **Orthogonal Coordinates** – Switch to 2D orthographic projection for classic 2D games.
7. **Draw a Ball** – Create a simple shape to represent the ball in Brickout.
8. **Set Ball Position** – Position the ball dynamically on the screen.
9. **Move a Ball** – Implement basic movement and animation for the ball.
10. **Bounce a Ball** – Add collision detection and make the ball bounce off walls.
11. **Define Ball Struct** – Organize your ball's properties using C structures.
12. **Draw a Paddle** – Render the paddle and prepare it for gameplay.
13. **Paddle Uniform Color** – Add color and control properties to the paddle.
14. **Keydown Callbacks** – Implement keyboard controls to move the paddle.
15. **Paddle Hit Detection** – Handle ball-paddle collisions.
16. **Draw a Row of Bricks** – Start building the brick wall.
17. **Draw Grid of Bricks** – Expand your brick wall into a full grid.
18. **Break Some Bricks** – Implement the satisfying breaking mechanics when bricks are hit.

By the end of the tutorial, you'll have a fully functional *Brickout* game running on your Pibit console!

## 🚀 Getting Started

To follow along with the tutorial, you'll need:

- A **Raspberry Pi Zero 2W**
- An **800x480 screen** (or any compatible display)
- **X-input controls** (e.g., a compatible gamepad or keyboard for testing)
- A working knowledge of **C programming**

Clone this repository to get started:

```bash
git clone https://github.com/kion-dgl/pibit-brickout-tutorial.git
cd pibit-brickout-tutorial
```

Follow the step-by-step instructions in the tutorial, and you'll be coding your own Brickout game in no time!

## Essential Dependencies:

- SDL2 – The core library for handling input, window management, and rendering.
- OpenGL – For rendering 2D/3D graphics.
- GLEW – The OpenGL Extension Wrangler Library to manage OpenGL extensions.
- CMake – A build system generator to help you compile your code (if needed).
- GCC – The GNU Compiler Collection to compile your C code.

```bash
sudo apt update
sudo apt install libsdl2-dev libsdl2-image-dev libglew-dev libgl1-mesa-dev cmake gcc
```

## 📚 Tutorial Structure

Each section of the tutorial is broken down into clearly labeled directories, so you can start from any step and follow the code as it evolves. Make sure to check out the comments in each source file for more details!

## 🤝 Contributing

Feel free to open issues or submit pull requests if you find any bugs, have suggestions for improvements, or want to add your own spin to the project!

##

```math
\left( \sum_{k=1}^n a_k b_k \right)^2 \leq \left( \sum_{k=1}^n a_k^2 \right) \left( \sum_{k=1}^n b_k^2 \right)
```

## 📝 License

This project is licensed under the MIT License – see the LICENSE file for details.

---

Enjoy building your own retro game with Pibit, and happy coding! 🎉
