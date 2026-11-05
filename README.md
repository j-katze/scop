_This project has been created as part of the 42 curriculum by jkatzenb._

# 42 mastery project: SCOP

## Description:
An introduction to GPU rendering.\
The goal of the project is to write a program in C, C++, or Rust that can load a 3D model from a .obj file and display it in 3D space using either OpenGL, Vulkan, or Metal.\
I chose to use C++ since out of the 3 languages available it is the one I'm most familiar with and Opengl because, judging by what I could find online, it appeared more beginner friendly than Vulkan. Metal was never considered because our projects need to run on Linux and Metal is Apple exclusive (macOS, iOS, iPadOS, etc.).

The program displays the 3D model in perspective, meaning things further away look smaller, with random gray tones applied to the models triangles to make them easy to distinguish.\
The object can be rotated around its center, moved around in 3D space, and scaled bigger or smaller.\
The program also loads an image texture from a 24 bit .bmp file, at the press of a button the model switches between the grayscale vertex colours to the image texture and back.\
If the .obj file requests a material to be loaded and the corresponding .mtl file exists in the same directory as the .obj file the colours loaded from the material can also be displayed on the model, if no .mtl file was loaded a default material will be generated and used instead.
___

## Instructions:
from the root of the project:\
compile the program with: `make`\
run the program with: `./scop models/model_name.obj textures/texture_name.bmp`

once the project is running use:\
`left mouse button and drag` to rotate the model\
`WASD` to translate the model\
`mouse wheel` to scale the model\
`1` to switch between textured and untextured display\
`2` to display the colours loaded by the .mtl file\
`3` switch wireframe display on and off\
`T` switch between controlling the model and controlling the camera\
`ESC` to close the program

the camera will be in __ORBIT__ mode by default, orbiting around the 0,0,0 world coordinate\
in __ORBIT__ mode:\
`move the mouse` to move the camera around the world center\
`mouse wheel` to change distance to the world center\
`R` to switch between __ORBIT__ and __FREE__ camera mode

in __FREE__ camera mode:\
`move the mouse` to look around\
`WASD` to move the camera in 3D space
___

## Resources:
#### OpenGL specific:
- introduction to OpenGL: https://learnopengl.com
- OpenGL documentation: https://registry.khronos.org/OpenGL-Refpages/
- GLFW documentation: https://www.glfw.org/documentation.html

#### loading data from files:
- .obj and .mtl: https://en.wikipedia.org/wiki/Wavefront_.obj_file
- triangulating the models faces: https://en.wikipedia.org/wiki/Polygon_triangulation
- - https://en.wikipedia.org/wiki/Fan_triangulation
- - https://en.wikipedia.org/wiki/UV_mapping
- .bmp: https://en.wikipedia.org/wiki/BMP_file_format
- - https://ewpratten.com/blog/reading-a-bitmap/

#### various maths concepts:
- vectors: https://www.mathsisfun.com/algebra/vectors.html
- - https://en.wikipedia.org/wiki/Unit_vector
- matrices: https://www.mathsisfun.com/algebra/matrix-introduction.html
- - https://en.wikipedia.org/wiki/Transformation_matrix#Examples_in_3_dimensions
- converting between coordinate systems: https://en.wikipedia.org/wiki/Spherical_coordinate_system#Cartesian_coordinates

#### perspective projection:
- creating a projection matrix: https://www.geeksforgeeks.org/engineering-mathematics/projection-matrix/
- making projection matrix work with non square aspect ratios: https://community.khronos.org/t/perspective-matrix-implementation/56407/9

#### others:
- interpolation: https://en.wikipedia.org/wiki/Linear_interpolation
- Lerp smoothing is broken by Freya holmér: 
  https://www.youtube.com/watch?v=LSNQuFEDOyQ
- my go to page for looking up anything c++ related: https://en.cppreference.com/

### How AI was used:
I used Duck.ai as a research and learning aid when I couldn’t find useful information through regular DuckDuckGo searches. I used it to help me understand mathematical formulas I found on Wikipedia and translate them into code and for troubleshooting issues I wasn't able to resolve on my own.

I also used it to learn about implementing planar UV projection. Most of the resources I found described how to use the feature in 3D modeling software rather than how to implement it in code, or were no longer available.

Finally, I used it to help word this section of the README.