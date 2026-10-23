_This project has been created as part of the 42 curriculum by jkatzenb._

Description:
A small project that serves as an introduction to GPU rendering using either OpenGL, Vulkan, or Metal.
The goal of the project is to render a 3D object read from a .obj file. The object must be diplayed in perspective (objects farther away appear smaller) and must be able to be translated (moved around) and rotated along its three main axes of symmetry with the origin at the center of the object.
The program must also use various colours to distinquish the different faces of the object and should apply a texture to the object when a dedicated key is pressed. Pressing the same key again should toggle the texture off again. Transitioning between the textured and untextured state must be smooth.

resources used:
https://learnopengl.com
https://en.wikipedia.org/wiki/Wavefront_.obj_file
https://en.wikipedia.org/wiki/Spherical_coordinate_system#Cartesian_coordinates
https://en.wikipedia.org/wiki/Rotation_formulations_in_three_dimensions#Rotation_matrix_%E2%86%92_Euler_angles_(z-x-z_extrinsic)
lerp smoothing is broken by freya holmer
https://www.youtube.com/watch?v=LSNQuFEDOyQ
https://en.wikipedia.org/wiki/Remainder#For_floating-point_numbers
https://www.geeksforgeeks.org/engineering-mathematics/projection-matrix/
https://community.khronos.org/t/perspective-matrix-implementation/56407/9
