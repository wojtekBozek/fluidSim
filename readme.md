# FLUID SIMULATOR

## Requirments

### CMAKE

This project utilizes CMAKE with all dependencies fetched from `external` dircetory.

### OpenGL

For full functionality OPENGL 4.6 is required.

### Nvidia

For hybrid simulations nvidia: extension `GL_NV_shader_atomic_float` is required

## Usage

This project allows for coomparing following methods of fluid simulations:
- SPH (WCSPH) Simulation both in 2D and 3D (customizable in the SPH simulation window).
- MAC simulation for 2D and 3D available in separate contexts.
- Hybrid simulation utilizing PIC and APIC algorithms (only 2D solution).

`WSAD` allows you to move in planar diorections while `pgUp` and `pgDown` allows you to move vertically. `o` centers the screen to original view. 
`TAB` hides and restores GUI view. When GUI is hidden camera rotation by mouse movement is available, while active GUI prevents user from rotating the view.
`F1` sets perspective view as active, while `F2` changes the viewport to ortographic. Mouse scroll changes FOV of camera.

## Simulations 

Samples of simulations are available in following youtube videos:

- [SPH in 3D](https://youtu.be/QT2VW3hVtIY)
- [MAC in 3D](https://youtu.be/AYdLlRCVnpc)
- [SPH in 2D](https://youtu.be/jLFvEdg-TgQ)
- [MAC in 2D](https://youtu.be/OZZ7uC_DFO8)
- [PIC/APIC in 2D](https://youtu.be/wrPfy9bvTtA)
- [Debug view for 2D MAC simulation](https://www.youtube.com/watch?v=NTc2cFe_et0)
