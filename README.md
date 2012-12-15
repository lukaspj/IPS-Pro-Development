IPS-Pro-Development
===================

# The most recent is in the precompiled folder, this makes it easier to test!

# Important information
* The particle effect is still a prototype please focus on the emitters atm.

# POI's
* MeshEmitter and RadiusMeshEmitter might have performance issues.
* Making the emitters properly load and save values is a tricky business. Please keep an eye out for load/save errors.

# [Documentation](https://github.com/lukaspj/IPS-Pro-Development/wiki)

# [Update news](https://github.com/lukaspj/IPS-Pro-Development/wiki/Update-news)

# Installation
You can either just copy and paste to merge your source folder with the source folder in the repo or do it manually:

Copy "core/util/xml"
Into the "core/util" folder of your T3D engine directory

Copy "math/muParser" into the math folder in your T3D engine directory

Add "Point3F relPos;" (without the quotes) to the bottom of the struct "Particle" in "T3D/fx/particle.h"

Overwrite ParticleEmitter and ParticleEmitterNode (both .h and .cpp file) with the ones from the repo and add the ImprovedParticle folder in "T3D/fx"

Remember to add them to the solution (in VS right click the folder in the solution explorer ->Add->Existing Item)

