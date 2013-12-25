This is a simplified version of the implementation of our "Precomputed Atmospheric Scattering" article (EGSR 2008). This version uses a perfectly spherical terrain for rendering, so there are no light shafts and the code for them is not included. Also the aerial perspective rendering is simplified, since only a single lookup in the precomputed S table is necessary (instead of two for an irregular terrain). For these reasons the rendering performance is higher than what is described in the paper. The code to precompute our 2D and 4D tables is fully included, without any simplification.

User Interface
- mouse: 
   drag = move the Sun
   SHIFT + drag = move the camera (longitude, latitude)
   CTRL + drag = move the camera (azimut, site)
- page up / page down : increase / decrease distance to ground
- + / - : increase / decrease exposure parameter for tone mapping
- F5 : reload all the shaders and recompute the precomputed tables
- ESC : exit

you can use F5 to see the effect of changing the atmospheric parameters defined in common.glsl
