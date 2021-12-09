# Renderer - Racing Game

Implementation of a 2D video game renderer with OpenGL
Used to develop simple games with Box2D.

Mainly focus to be used for Reinforcement Learning. Re-implement CarRacing environment from OpenAI (https://github.com/openai/gym/blob/master/gym/envs/box2d/car_racing.py)

MIT License

## Dependencies
* OpenGL
* Box2D
* glad
* glfw
* glm
* glut

## Compile on Ubuntu
Using Cmake to generate makefiles

```
git clone https://github.com/Amathlog/renderer
cd renderer
mkdir build
cd build
cmake ..
make
./Renderer
```

## Results
<img src=images/results.png>

## Controls
* Arrow Up -> Accelerate
* Arrow Down -> Brake
* Arrow Left/Right -> Steering
* Q -> Unzoom
* E -> Zoom
