IPS-Pro-Development
===================

## T3D/fx/particle.h
At the bottom of this file at the bottom of the struct "Particle" the line
'''Point3F relPos;'''
was added

## Documentation
This folder will hold instructions, information and more.

# Installation
You can either just copy and paste to merge your source folder with the source folder in the repo or do it manually:

Copy "core/util/xml"
Into the "core/util" folder of your T3D engine directory

Copy "math/muParser" into the math folder in your T3D engine directory

Add "Point3F relPos;" (without the quotes) to the bottom of the struct "Particle" in "T3D/fx/particle.h"

Overwrite ParticleEmitter and ParticleEmitterNode (both .h and .cpp file) with the ones from the repo and add the ImprovedParticle folder in "T3D/fx"

Remember to add them to the solution (in VS right click the folder in the solution explorer ->Add->Existing Item)

