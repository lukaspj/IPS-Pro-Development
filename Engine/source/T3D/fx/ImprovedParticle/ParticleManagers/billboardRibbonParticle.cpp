// Copyright (C) 2013 Winterleaf Entertainment L,L,C.
// 
// THE SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTY OF ANY KIND,
// INCLUDING WITHOUT LIMITATION THE WARRANTIES OF MERCHANT ABILITY, FITNESS
// FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT. THE ENTIRE RISK AS TO THE
// QUALITY AND PERFORMANCE OF THE SOFTWARE IS THE RESPONSIBILITY OF LICENSEE.
// SHOULD THE SOFTWARE PROVE DEFECTIVE IN ANY RESPECT, LICENSEE AND NOT LICEN-
// SOR OR ITS SUPPLIERS OR RESELLERS ASSUMES THE ENTIRE COST OF ANY SERVICE AND
// REPAIR. THIS DISCLAIMER OF WARRANTY CONSTITUTES AN ESSENTIAL PART OF THIS
// AGREEMENT. NO USE OF THE SOFTWARE IS AUTHORIZED HEREUNDER EXCEPT UNDER
// THIS DISCLAIMER.
// 
// The use of the WinterLeaf Entertainment LLC Improved Particle System Bundle (IPS Bundle)
// is governed by this license agreement.
// 
// RESTRICTIONS
// 
// (a) Licensee may not: (i) create any derivative works of IPS Bundle, including but not
// limited to translations, localizations, technology add-ons, or game making software
// other than Games without express permission from Winterleaf Entertainment; (ii) redistribute, 
// encumber , sell, rent, lease, sublicense, or otherwise
// transfer rights to IPS Bundle; or (iii) remove or alter any trademark, logo, copyright
// or other proprietary notices, legends, symbols or labels in IPS Bundle; or (iv) use
// the Software to develop or distribute any software that competes with the Software
// without WinterLeaf Entertainment's prior written consent; or (v) use the Software for
// any illegal purpose.
// (b) Licensee may not distribute the IPS Bundle in any manner.
// 
// LICENSEGRANT.
// This license allows companies of any size, government entities or individuals to cre-
// ate, sell, rent, lease, or otherwise profit commercially from, games using executables
// created from the source code of IPS Bundle
// 
// Please visit http://www.winterleafentertainment.com for more information about the project and latest updates.

#include "billboardRibbonParticle.h"
#include "console/consoleTypes.h"
#include "console/typeValidators.h"
#include "core/stream/bitStream.h"
#include "math/mathIO.h"
#include "console/engineAPI.h"

IMPLEMENT_CO_DATABLOCK_V1(BillboardRibbonParticleData);

ConsoleDocClass( BillboardRibbonParticleData,
                "@brief Contains information for how specific particles should look and react "
                "including particle colors, particle imagemap, acceleration value for individual "
                "particles and spin information.\n"

                "@tsexample\n"
                "datablock ParticleData( GLWaterExpSmoke )\n"
                "{\n"
                "   textureName = \"art/shapes/particles/smoke\";\n"
                "   dragCoefficient = 0.4;\n"
                "   gravityCoefficient = -0.25;\n"
                "   inheritedVelFactor = 0.025;\n"
                "   constantAcceleration = -1.1;\n"
                "   lifetimeMS = 1250;\n"
                "   lifetimeVarianceMS = 0;\n"
                "   useInvAlpha = false;\n"
                "   spinSpeed = 1;\n"
                "   spinRandomMin = -200.0;\n"
                "   spinRandomMax = 200.0;\n\n"
                "   colors[0] = \"0.1 0.1 1.0 1.0\";\n"
                "   colors[1] = \"0.4 0.4 1.0 1.0\";\n"
                "   colors[2] = \"0.4 0.4 1.0 0.0\";\n\n"
                "   sizes[0] = 2.0;\n"
                "   sizes[1] = 6.0;\n"
                "   sizes[2] = 2.0;\n\n"
                "   times[0] = 0.0;\n"
                "   times[1] = 0.5;\n"
                "   times[2] = 1.0;\n"
                "};\n"
                "@endtsexample\n"

                "@ingroup FX\n"
                "@see ParticleEmitter\n"
                "@see ParticleEmitterData\n"
                "@see ParticleEmitterNode\n"
                );

static const float sgDefaultSpinSpeed = 1.f;
static const float sgDefaultSpinRandomMin = 0.f;
static const float sgDefaultSpinRandomMax = 0.f;

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
BillboardRibbonParticleData::BillboardRibbonParticleData()
{
   spinSpeed            = sgDefaultSpinSpeed;
   spinRandomMin        = sgDefaultSpinRandomMin;
   spinRandomMax        = sgDefaultSpinRandomMax;
   useInvAlpha          = false;
   animateTexture       = false;

   numFrames            = 1;
   framesPerSec         = numFrames;

   S32 i;
   for( i=0; i<PDC_NUM_KEYS; i++ )
   {
      colors[i].set( 1.0, 1.0, 1.0, 1.0 );
   }

   texCoords[0].set(0.0,0.0);   // texture coords at 4 corners
   texCoords[1].set(0.0,1.0);   // of particle quad
   texCoords[2].set(1.0,1.0);   // (defaults to entire particle)
   texCoords[3].set(1.0,0.0);
   animTexTiling.set(0,0);      // tiling dimensions 
   animTexFramesString = NULL;  // string of animation frame indices
   animTexUVs = NULL;           // array of tile vertex UVs
   hResTextureName = NULL;      // texture filename
   hResTextureHandle = NULL;    // loaded texture handle
   mResTextureName = NULL;      // texture filename
   mResTextureHandle = NULL;    // loaded texture handle
   lResTextureName = NULL;      // texture filename
   lResTextureHandle = NULL;    // loaded texture handle
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
BillboardRibbonParticleData::~BillboardRibbonParticleData()
{
   if (animTexUVs)
   {
      delete [] animTexUVs;
   }
}

FRangeValidator BRPM_spinRandFValidator(-1000.f, 1000.f);

//-----------------------------------------------------------------------------
// initPersistFields
//-----------------------------------------------------------------------------
void BillboardRibbonParticleData::initPersistFields()
{
   addField( "spinSpeed", TYPEID< F32 >(), Offset(spinSpeed, BillboardRibbonParticleData),
      "Speed at which to spin the particle." );
   addFieldV( "spinRandomMin", TYPEID< F32 >(), Offset(spinRandomMin, BillboardRibbonParticleData), &BRPM_spinRandFValidator,
      "Minimum allowed spin speed of this particle, between -1000 and spinRandomMax." );
   addFieldV( "spinRandomMax", TYPEID< F32 >(), Offset(spinRandomMax, BillboardRibbonParticleData), &BRPM_spinRandFValidator,
      "Maximum allowed spin speed of this particle, between spinRandomMin and 1000." );
   addField( "useInvAlpha", TYPEID< bool >(), Offset(useInvAlpha, BillboardRibbonParticleData),
      "@brief Controls how particles blend with the scene.\n\n"
      "If true, particles blend like ParticleBlendStyle NORMAL, if false, "
      "blend like ParticleBlendStyle ADDITIVE.\n"
      "@note If ParticleEmitterData::blendStyle is set, it will override this value." );
   addField( "animateTexture", TYPEID< bool >(), Offset(animateTexture, BillboardRibbonParticleData),
      "If true, allow the particle texture to be an animated sprite." );
   addField( "framesPerSec", TYPEID< S32 >(), Offset(framesPerSec, BillboardRibbonParticleData),
      "If animateTexture is true, this defines the frames per second of the "
      "sprite animation." );

   addField( "textureCoords", TYPEID< Point2F >(), Offset(texCoords, BillboardRibbonParticleData),  4,
      "@brief 4 element array defining the UV coords into textureName to use "
      "for this particle.\n\n"
      "Coords should be set for the first tile only when using animTexTiling; "
      "coordinates for other tiles will be calculated automatically. \"0 0\" is "
      "top left and \"1 1\" is bottom right." );
   addField( "animTexTiling", TYPEID< Point2I >(), Offset(animTexTiling, BillboardRibbonParticleData),
      "@brief The number of frames, in rows and columns stored in textureName "
      "(when animateTexture is true).\n\n"
      "A maximum of 256 frames can be stored in a single texture when using "
      "animTexTiling. Value should be \"NumColumns NumRows\", for example \"4 4\"." );
   addField( "animTexFrames", TYPEID< StringTableEntry >(), Offset(animTexFramesString, BillboardRibbonParticleData),
      "@brief A list of frames and/or frame ranges to use for particle "
      "animation if animateTexture is true.\n\n"
      "Each frame token must be separated by whitespace. A frame token must be "
      "a positive integer frame number or a range of frame numbers separated "
      "with a '-'. The range separator, '-', cannot have any whitspace around "
      "it.\n\n"
      "Ranges can be specified to move through the frames in reverse as well "
      "as forward (eg. 19-14). Frame numbers exceeding the number of tiles will "
      "wrap.\n"
      "@tsexample\n"
      "animTexFrames = \"0-16 20 19 18 17 31-21\";\n"
      "@endtsexample\n" );

   addField( "HighResTexture", TYPEID< StringTableEntry >(), Offset(hResTextureName, BillboardRibbonParticleData),
      "@brief Texture file to use for this particle." );
   addField( "MidResTexture", TYPEID< StringTableEntry >(), Offset(mResTextureName, BillboardRibbonParticleData),
      "@brief Texture file to use for this particle." );
   addField( "LowResTexture", TYPEID< StringTableEntry >(), Offset(lResTextureName, BillboardRibbonParticleData),
      "@brief Texture file to use for this particle." );
   addField( "textureName", TYPEID< StringTableEntry >(), Offset(hResTextureName, BillboardRibbonParticleData),
      "@brief Texture file to use for this particle.\n\n"
      "Deprecated. use HighResTexture instead." );
   addField( "animTexName", TYPEID< StringTableEntry >(), Offset(hResTextureName, BillboardRibbonParticleData),
      "@brief Texture file to use for this particle if animateTexture is true.\n\n"
      "Deprecated. Use textureName instead." );

   // Interpolation variables
   addField( "colors", TYPEID< ColorF >(), Offset(colors, BillboardRibbonParticleData), PDC_NUM_KEYS,
      "@brief Particle RGBA color keyframe values.\n\n"
      "The particle color will linearly interpolate between the color/time keys "
      "over the lifetime of the particle." );
   addProtectedField( "times", TYPEID< F32 >(), Offset(times, BillboardRibbonParticleData), &protectedSetTimes, 
      &defaultProtectedGetFn, PDC_NUM_KEYS,
      "@brief Time keys used with the colors and sizes keyframes.\n\n"
      "Values are from 0.0 (particle creation) to 1.0 (end of lifespace)." );

   Parent::initPersistFields();
}

//-----------------------------------------------------------------------------
// Pack data
//-----------------------------------------------------------------------------
void BillboardRibbonParticleData::packData(BitStream* stream)
{
   Parent::packData(stream);

   if( stream->writeFlag( spinSpeed != sgDefaultSpinSpeed ) )
      stream->write(spinSpeed);
   if(stream->writeFlag(spinRandomMin != sgDefaultSpinRandomMin || spinRandomMax != sgDefaultSpinRandomMax))
   {
      stream->writeInt((S32)(spinRandomMin + 1000), 11);
      stream->writeInt((S32)(spinRandomMax + 1000), 11);
   }
   stream->writeFlag(useInvAlpha);

   S32 i, count;

   // see how many frames there are:
   for(count = 0; count < PDC_NUM_KEYS - 1; count++)
      if(times[count] >= 1)
         break;

   count++;

   stream->writeInt(count, PDC_NUM_BITS);

   for( i=0; i<count; i++ )
   {
      stream->writeFloat( colors[i].red, 7);
      stream->writeFloat( colors[i].green, 7);
      stream->writeFloat( colors[i].blue, 7);
      stream->writeFloat( colors[i].alpha, 7);
      stream->writeFloat( sizes[i]/MaxParticleSize, 14);
   }

   if (stream->writeFlag(hResTextureName && hResTextureName[0]))
      stream->writeString(hResTextureName);
   if (stream->writeFlag(mResTextureName && mResTextureName[0]))
      stream->writeString(mResTextureName);
   if (stream->writeFlag(lResTextureName && lResTextureName[0]))
      stream->writeString(lResTextureName);
   for (i = 0; i < 4; i++)
      mathWrite(*stream, texCoords[i]);
   if (stream->writeFlag(animateTexture))
   {
      if (stream->writeFlag(animTexFramesString && animTexFramesString[0]))
      {
         stream->writeString(animTexFramesString);
      }
      mathWrite(*stream, animTexTiling);
      stream->writeInt(framesPerSec, 8);
   }
}

//-----------------------------------------------------------------------------
// Unpack data
//-----------------------------------------------------------------------------
void BillboardRibbonParticleData::unpackData(BitStream* stream)
{
   Parent::unpackData(stream);

   if(stream->readFlag())
      stream->read(&spinSpeed);
   else
      spinSpeed = sgDefaultSpinSpeed;

   if(stream->readFlag())
   {
      spinRandomMin = (F32)(stream->readInt(11) - 1000);
      spinRandomMax = (F32)(stream->readInt(11) - 1000);
   }
   else
   {
      spinRandomMin = sgDefaultSpinRandomMin;
      spinRandomMax = sgDefaultSpinRandomMax;
   }

   useInvAlpha = stream->readFlag();

   S32 i;
   S32 count = stream->readInt(PDC_NUM_BITS);
   for(i = 0;i < count; i++)
   {
      colors[i].red = stream->readFloat(7);
      colors[i].green = stream->readFloat(7);
      colors[i].blue = stream->readFloat(7);
      colors[i].alpha = stream->readFloat(7);
      sizes[i] = stream->readFloat(14) * MaxParticleSize;
   }
   hResTextureName = (stream->readFlag()) ? stream->readSTString() : 0;
   mResTextureName = (stream->readFlag()) ? stream->readSTString() : 0;
   lResTextureName = (stream->readFlag()) ? stream->readSTString() : 0;
   for (i = 0; i < 4; i++)
      mathRead(*stream, &texCoords[i]);

   animateTexture = stream->readFlag();
   if (animateTexture)
   {
      animTexFramesString = (stream->readFlag()) ? stream->readSTString() : 0;
      mathRead(*stream, &animTexTiling);
      framesPerSec = stream->readInt(8);
   }
}

bool BillboardRibbonParticleData::protectedSetTimes( void *object, const char *index, const char *data) 
{
   BillboardRibbonParticleData *pData = static_cast<BillboardRibbonParticleData*>( object );
   F32 val = dAtof(data);
   U32 i;

   if (!index)
      i = 0;
   else
      i = dAtoui(index);

   pData->times[i] = mClampF( val, 0.f, 1.f );

   return false;
}

//-----------------------------------------------------------------------------
// onAdd
//-----------------------------------------------------------------------------
bool BillboardRibbonParticleData::onAdd()
{
   if (Parent::onAdd() == false)
      return false;

   if (spinSpeed > 1000.f || spinSpeed < -1000.f) {
      Con::warnf(ConsoleLogEntry::General, "ParticleData(%s) spinSpeed invalid", getName());
      return false;
   }
   if (spinRandomMin > 1000.f || spinRandomMin < -1000.f) {
      Con::warnf(ConsoleLogEntry::General, "ParticleData(%s) spinRandomMin invalid", getName());
      spinRandomMin = -360.0;
      return false;
   }
   if (spinRandomMin > spinRandomMax) {
      Con::warnf(ConsoleLogEntry::General, "ParticleData(%s) spinRandomMin greater than spinRandomMax", getName());
      spinRandomMin = spinRandomMax - (spinRandomMin - spinRandomMax );
      return false;
   }
   if (spinRandomMax > 1000.f || spinRandomMax < -1000.f) {
      Con::warnf(ConsoleLogEntry::General, "ParticleData(%s) spinRandomMax invalid", getName());
      spinRandomMax = 360.0;
      return false;
   }
   if (framesPerSec > 255)
   {
      Con::warnf(ConsoleLogEntry::General, "ParticleData(%s) framesPerSec > 255, too high", getName());
      framesPerSec = 255;
      return false;
   }

   // Here we validate parameters
   if (animateTexture) 
   {
      // Tiling dimensions must be positive and non-zero
      if (animTexTiling.x <= 0 || animTexTiling.y <= 0)
      {
         Con::warnf(ConsoleLogEntry::General, 
            "ParticleData(%s) bad value(s) for animTexTiling [%d or %d <= 0], invalid datablock", 
            animTexTiling.x, animTexTiling.y, getName());
         return false;
      }

      // Indices must fit into a byte so these are also bad
      if (animTexTiling.x * animTexTiling.y > 256)
      {
         Con::warnf(ConsoleLogEntry::General, 
            "ParticleData(%s) bad values for animTexTiling [%d*%d > %d], invalid datablock", 
            animTexTiling.x, animTexTiling.y, 256, getName());
         return false;
      }

      // A list of frames is required
      if (!animTexFramesString || !animTexFramesString[0]) 
      {
         Con::warnf(ConsoleLogEntry::General, "ParticleData(%s) no animTexFrames, invalid datablock", getName());
         return false;
      }

      // The frame list cannot be too long.
      if (animTexFramesString && dStrlen(animTexFramesString) > 255) 
      {
         Con::errorf(ConsoleLogEntry::General, "ParticleData(%s) animTexFrames string too long [> 255 chars]", getName());
         return false;
      }
   }

   return true;
}

//-----------------------------------------------------------------------------
// preload
//-----------------------------------------------------------------------------
bool BillboardRibbonParticleData::preload(bool server, String &errorStr)
{
   if (Parent::preload(server, errorStr) == false)
      return false;

   bool error = false;
   if(!server)
   {
      // Here we attempt to load the particle's texture if specified. An undefined
      // texture is *not* an error since the emitter may provide one.
      if (hResTextureName && hResTextureName[0])
      {
         hResTextureHandle = GFXTexHandle(hResTextureName, &GFXDefaultStaticDiffuseProfile, avar("%s() - hResTextureHandle (line %d)", __FUNCTION__, __LINE__));
         if (!hResTextureHandle)
         {
            errorStr = String::ToString("Missing particle texture: %s", hResTextureName);
            error = true;
         }
      }

      // Here we attempt to load the particle's texture if specified. An undefined
      // texture is *not* an error since the emitter may provide one.
      if (mResTextureName && mResTextureName[0])
      {
         mResTextureHandle = GFXTexHandle(mResTextureName, &GFXDefaultStaticDiffuseProfile, avar("%s() - mResTextureHandle (line %d)", __FUNCTION__, __LINE__));
         if (!mResTextureHandle)
         {
            errorStr = String::ToString("Missing particle texture: %s", mResTextureName);
            error = true;
         }
      }

      // Here we attempt to load the particle's texture if specified. An undefined
      // texture is *not* an error since the emitter may provide one.
      if (lResTextureName && lResTextureName[0])
      {
         lResTextureHandle = GFXTexHandle(lResTextureName, &GFXDefaultStaticDiffuseProfile, avar("%s() - lResTextureHandle (line %d)", __FUNCTION__, __LINE__));
         if (!lResTextureHandle)
         {
            errorStr = String::ToString("Missing particle texture: %s", lResTextureName);
            error = true;
         }
      }

      if (animateTexture) 
      {
         // Here we parse animTexFramesString into byte-size frame numbers in animTexFrames.
         // Each frame token must be separated by whitespace.
         // A frame token must be a positive integer frame number or a range of frame numbers
         // separated with a '-'. 
         // The range separator, '-', cannot have any whitspace around it.
         // Ranges can be specified to move through the frames in reverse as well as forward.
         // Frame numbers exceeding the number of tiles will wrap.
         //   example:
         //     "0-16 20 19 18 17 31-21"

         S32 n_tiles = animTexTiling.x * animTexTiling.y;
         AssertFatal(n_tiles > 0 && n_tiles <= 256, "Error, bad animTexTiling setting." );

         animTexFrames.clear();

         char* tokCopy = new char[dStrlen(animTexFramesString) + 1];
         dStrcpy(tokCopy, animTexFramesString);

         char* currTok = dStrtok(tokCopy, " \t");
         while (currTok != NULL) 
         {
            char* minus = dStrchr(currTok, '-');
            if (minus)
            { 
               // add a range of frames
               *minus = '\0';
               S32 range_a = dAtoi(currTok);
               S32 range_b = dAtoi(minus+1);
               if (range_b < range_a)
               {
                  // reverse frame range
                  for (S32 i = range_a; i >= range_b; i--)
                     animTexFrames.push_back((U8)(i % n_tiles));
               }
               else
               {
                  // forward frame range
                  for (S32 i = range_a; i <= range_b; i++)
                     animTexFrames.push_back((U8)(i % n_tiles));
               }
            }
            else
            {
               // add one frame
               animTexFrames.push_back((U8)(dAtoi(currTok) % n_tiles));
            }
            currTok = dStrtok(NULL, " \t");
         }

         // Here we pre-calculate the UVs for each frame tile, which are
         // tiled inside the UV region specified by texCoords. Since the
         // UVs are calculated using bilinear interpolation, the texCoords
         // region does *not* have to be an axis-aligned rectangle.

         if (animTexUVs)
            delete [] animTexUVs;

         animTexUVs = new Point2F[(animTexTiling.x+1)*(animTexTiling.y+1)];

         // interpolate points on the left and right edge of the uv quadrangle
         Point2F lf_pt = texCoords[0];
         Point2F rt_pt = texCoords[3];

         // per-row delta for left and right interpolated points
         Point2F lf_d = (texCoords[1] - texCoords[0])/(F32)animTexTiling.y;
         Point2F rt_d = (texCoords[2] - texCoords[3])/(F32)animTexTiling.y;

         S32 idx = 0;
         for (S32 yy = 0; yy <= animTexTiling.y; yy++)
         {
            Point2F p = lf_pt;
            Point2F dp = (rt_pt - lf_pt)/(F32)animTexTiling.x;
            for (S32 xx = 0; xx <= animTexTiling.x; xx++)
            {
               animTexUVs[idx++] = p;
               p += dp;
            }
            lf_pt += lf_d;
            rt_pt += rt_d;
         }

         // cleanup
         delete [] tokCopy;
         numFrames = animTexFrames.size();
      }
   }

   return !error;
}

bool BillboardRibbonParticleData::reload(char errorBuffer[256])
{
   bool error = Parent::reload(errorBuffer);
   if (hResTextureName && hResTextureName[0])
   {
      hResTextureHandle = GFXTexHandle(hResTextureName, &GFXDefaultStaticDiffuseProfile, avar("%s() - hResTextureHandle (line %d)", __FUNCTION__, __LINE__));
      if (!hResTextureHandle)
      {
         dSprintf(errorBuffer, 256, "Missing particle texture: %s", hResTextureName);
         error = true;
      }
   }
   if (mResTextureName && mResTextureName[0])
   {
      mResTextureHandle = GFXTexHandle(mResTextureName, &GFXDefaultStaticDiffuseProfile, avar("%s() - mResTextureHandle (line %d)", __FUNCTION__, __LINE__));
      if (!mResTextureHandle)
      {
         dSprintf(errorBuffer, 256, "Missing particle texture: %s", mResTextureName);
         error = true;
      }
   }
   if (lResTextureName && lResTextureName[0])
   {
      lResTextureHandle = GFXTexHandle(lResTextureName, &GFXDefaultStaticDiffuseProfile, avar("%s() - lResTextureHandle (line %d)", __FUNCTION__, __LINE__));
      if (!lResTextureHandle)
      {
         dSprintf(errorBuffer, 256, "Missing particle texture: %s", lResTextureName);
         error = true;
      }
   }
   /*
   numFrames = 0;
   for( int i=0; i<PDC_MAX_TEX; i++ )
   {
   if( textureNameList[i] && textureNameList[i][0] )
   {
   textureList[i] = TextureHandle( textureNameList[i], MeshTexture );
   if (!textureList[i].getName())
   {
   dSprintf(errorBuffer, 256, "Missing particle texture: %s", textureNameList[i]);
   error = true;
   }
   numFrames++;
   }
   }
   */
   return !error;
}

void BillboardRibbonParticleData::preloadVector(Vector<ParticleData*> _particleDataBlocks, U32 mask, ParticleEmitterData* emitterData)
{
   if(mask & ParticleVectorPreloadFlags::ParticleTextures)
   {
      BillboardRibbonParticleData* particleDataBlock = static_cast<BillboardRibbonParticleData*>(_particleDataBlocks[0]);
      StringTableEntry htxr_name, mtxr_name, ltxr_name;
      htxr_name = particleDataBlock->hResTextureName;
      mtxr_name = particleDataBlock->mResTextureName;
      ltxr_name = particleDataBlock->lResTextureName;
      for (S32 i = 1; i < _particleDataBlocks.size(); i++)
      {
         particleDataBlock = static_cast<BillboardRibbonParticleData*>(_particleDataBlocks[i]);
         // warn if particle textures are inconsistent
         if (particleDataBlock->hResTextureName != htxr_name &&
            particleDataBlock->mResTextureName != mtxr_name &&
            particleDataBlock->lResTextureName != ltxr_name)
         {
            Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) particles reference different textures.", getName());
            break;
         }
      }
   }
   if(mask & ParticleVectorPreloadFlags::InvAlpha)
   {
      BillboardRibbonParticleData* particleDataBlock = static_cast<BillboardRibbonParticleData*>(_particleDataBlocks[0]);
      bool useInvAlpha = particleDataBlock->useInvAlpha;
      for (S32 i = 1; i < _particleDataBlocks.size(); i++)
      {
         particleDataBlock = static_cast<BillboardRibbonParticleData*>(_particleDataBlocks[i]);
         // warn if blend-style legacy useInvAlpha settings are inconsistent
         if (particleDataBlock->useInvAlpha != useInvAlpha)
         {
            Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) particles have inconsistent useInvAlpha settings.", getName());
            break;
         }
      }
      emitterData->blendStyle = (useInvAlpha) ? ParticleRenderInst::BlendNormal : ParticleRenderInst::BlendAdditive;
   }
}

DefineEngineMethod(BillboardRibbonParticleData, reload, void, (),,
                   "Reloads this particle.\n"
                   "@tsexample\n"
                   "// Get the editor's current particle\n"
                   "%particle = PE_ParticleEditor.currParticle\n\n"
                   "// Change a particle value\n"
                   "%particle.setFieldValue( %propertyField, %value );\n\n"
                   "// Reload it\n"
                   "%particle.reload();\n"
                   "@endtsexample\n" )
{
   char errorBuffer[256];
   object->reload(errorBuffer);
}