//-----------------------------------------------------------------------------
// Copyright (c) 2012 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------


// Open the particle editor to spawn a test emitter in front of the player.
// Edit the sliders, check boxes, and text fields and see the results in
// realtime.  Switch between emitters and particles with the buttons in the
// top left corner.  When in particle mode, the only particles available will
// be those assigned to the current emitter to avoid confusion.  In the top
// right corner, there is a button marked "Drop Emitter", which will spawn the
// test emitter in front of the player again, and a button marked "Restart
// Emitter", which will play the particle animation again.


//=============================================================================================
//    ParticleEditor.
//=============================================================================================

$PE_EMITTER_TYPE = "None";

//---------------------------------------------------------------------------------------------

function ParticleEditor::initEditor( %this )
{
   $PE_EMITTER_TYPE = "Sphere";
   ParticleEditorSphereEmitterButton.setValue(1);
   ParticleEditorGraphEmitterButton.setValue(0);
   ParticleEditorGroundEmitterButton.setValue(0);
   ParticleEditorMaskEmitterButton.setValue(0);
   ParticleEditorParticleEffectButton.setValue(0);
   echo( "Initializing SphereEmitterData and BillboardParticleData DataBlocks..." );
   
   datablock BillboardParticleData(PE_ParticleEditor_NotDirtyParticle)
   {
      textureName = "art/particles/defaultParticle";
   };
   
   PE_UnlistedEmitters.add( PE_ParticleEditor_NotDirtyParticle );
   
   PEE_EmitterSelector.clear();
   
   ParticleEditor.createParticleList();
   
   PEP_ParticleSelector.clear();
   
   PE_SphereEmitterEditor.initEditor();
   PE_GraphEmitterEditor.initEditor();
   PE_GroundEmitterEditor.initEditor();
   PE_MaskEmitterEditor.initEditor();
   
   PEP_ParticleClassSelector.add("BillboardParticles", 0);
   PEP_ParticleClassSelector.add("TSShapeParticles", 1);
   PEP_ParticleClassSelector.add("EmitterParticles", 2);
   PEP_ParticleClassSelector.add("PointlightParticles", 3);
   PEP_ParticleClassSelector.add("EffectParticles", 4);
   PEP_ParticleClassSelector.add("BillboardRibbonParticles", 5);
   
   PEE_EmitterSelector.setFirstSelected();

   PE_Window-->EditorTabBook.selectPage( 0 );
}

function ParticleEditor::createParticleList( %this )
{
   // This function creates the list of all particles and particle emitters
   
   %emitterCount = 0;
   PEE_EmitterSelector.clear();
   
   foreach( %obj in DatablockGroup )
   {
      if( $PE_EMITTER_TYPE $= "Sphere" && %obj.isMemberOfClass( "SphereEmitterData" ) ||
          $PE_EMITTER_TYPE $= "Graph" && %obj.isMemberOfClass( "GraphEmitterData" ) ||
          $PE_EMITTER_TYPE $= "Ground" && %obj.isMemberOfClass( "GroundEmitterData" ) ||
          $PE_EMITTER_TYPE $= "Mask" && %obj.isMemberOfClass( "MaskEmitterData" ) ||
          $PE_EMITTER_TYPE $= "Effect" && %obj.isMemberOfClass( "ParticleEffectData" ) )
      {
         // Filter out emitters on the PE_UnlistedEmitters list.
         
         %unlistedFound = false;
         foreach( %unlisted in PE_UnlistedEmitters )
            if( %unlisted.getId() == %obj.getId() )
            {
               %unlistedFound = true;
               break;
            }
      
         if( %unlistedFound )
            continue;
            
         // To prevent our default emitters from getting changed,
         // prevent them from populating the list. Default emitters
         // should only be used as a template for creating new ones.
         if ( %obj.getName() $= "DefaultEmitter")
            continue;
         
         PEE_EmitterSelector.add( %obj.getName(), %obj.getId() );
         %emitterCount ++;
      }
   }
   
   PEE_EmitterSelector.sort();
   
   echo( "Found" SPC %emitterCount SPC "emitters and" SPC %particleCount SPC "particles." );
}

//---------------------------------------------------------------------------------------------

function ParticleEditor::openEmitterPane( %this )
{
   PE_Window.text = "Particle Editor - Emitters";
   PE_EmitterEditor.guiSync();
   ParticleEditor.activeEditor = PE_EmitterEditor;
   
   if( !PE_EmitterEditor.dirty )
      PE_EmitterEditor.setEmitterNotDirty();
}

//---------------------------------------------------------------------------------------------

function ParticleEditor::openParticlePane( %this )
{
   PE_Window.text = "Particle Editor - Particles";
   
   PE_ParticleEditor.guiSync();
   ParticleEditor.activeEditor = PE_ParticleEditor;
   
   if( !PE_ParticleEditor.dirty )
      PE_ParticleEditor.setParticleNotDirty();
}

//---------------------------------------------------------------------------------------------

function ParticleEditor::resetEmitterNode( %this )
{
   %tform = ServerConnection.getControlObject().getEyeTransform();
   %vec = VectorNormalize( ServerConnection.getControlObject().getForwardVector() );
   %vec = VectorScale( %vec, 4 );
   %tform = setWord( %tform, 0, getWord( %tform, 0 ) + getWord( %vec, 0 ) );
   %tform = setWord( %tform, 1, getWord( %tform, 1 ) + getWord( %vec, 1 ) );
   %tform = setWord( %tform, 2, getWord( %tform, 2 ) + getWord( %vec, 2 ) );

   if( !isObject( $ParticleEditor::emitterNode ) )
   {
      if( !isObject( TestEmitterNodeData ) )
      {
         datablock SphereEmitterNodeData( TestEmitterNodeData )
         {
            timeMultiple = 1;
         };
      }

      $ParticleEditor::emitterNode = new SphereEmitterNode()
      {
         emitter = PEE_EmitterSelector.getText();
         velocity = 1;
         position = getWords( %tform, 0, 2 );
         rotation = getWords( %tform, 3, 6 );
         datablock = TestEmitterNodeData;
         parentGroup = MissionCleanup;
      };
   }
   else
   {
      $ParticleEditor::emitterNode.setTransform( %tform );
      
      %clientObject = $ParticleEditor::emitterNode.getClientObject();
      if( isObject( %clientObject ) )
         %clientObject.setTransform( %tform );
      
      ParticleEditor.updateEmitterNode();
   }
}

//---------------------------------------------------------------------------------------------

function ParticleEditor::updateEmitterNode( %this )
{
   if( isObject( $ParticleEditor::emitterNode ) )
   {
      %id = PEE_EmitterSelector_Control-->PopUpMenu.getSelected();
      
      %clientObject = $ParticleEditor::emitterNode.getClientObject();
      if( isObject( %clientObject ) )
         %clientObject.setEmitterDataBlock( %id );
   }
   else
      %this.resetEmitterNode();
}

//=============================================================================================
//    PE_TabBook.
//=============================================================================================

//---------------------------------------------------------------------------------------------

function PE_TabBook::onTabSelected( %this, %text, %idx )
{
   if( %idx == 0 )
      ParticleEditor.openEmitterPane();
   else
      ParticleEditor.openParticlePane();
}

//-----------------------------------------------------------------------------

function ParticleEditorNoneModeBtn::onClick(%this)
{
   GlobalGizmoProfile.mode = "None";
   
   EditorGuiStatusBar.setInfo("Selection arrow.");
}

function ParticleEditorMoveModeBtn::onClick(%this)
{
   GlobalGizmoProfile.mode = "Move";
   
   %cmdCtrl = "CTRL";
   if( $platform $= "macos" )
      %cmdCtrl = "CMD";
   
   EditorGuiStatusBar.setInfo( "Move selection.  SHIFT while dragging duplicates objects.  " @ %cmdCtrl @ " to toggle soft snap.  ALT to toggle grid snap." );
}

function ParticleEditorRotateModeBtn::onClick(%this)
{
   GlobalGizmoProfile.mode = "Rotate";
   
   EditorGuiStatusBar.setInfo("Rotate selection.");
}

function ParticleEditorScaleModeBtn::onClick(%this)
{
   GlobalGizmoProfile.mode = "Scale";
   
   EditorGuiStatusBar.setInfo("Scale selection.");
}

function ParticleEditorSphereEmitterButton::onClick(%this)
{
   if($PE_EMITTER_TYPE !$= "Sphere")
   {
      $PE_EMITTER_TYPE = "Sphere";
      ParticleEditor.createParticleList();
      PEE_EmitterSelector.setFirstSelected();
   }
}

function ParticleEditorGraphEmitterButton::onClick(%this)
{
   if($PE_EMITTER_TYPE !$= "Graph")
   {
      $PE_EMITTER_TYPE = "Graph";
      ParticleEditor.createParticleList();
      PEE_EmitterSelector.setFirstSelected();
   }

}

function ParticleEditorGroundEmitterButton::onClick(%this)
{
   if($PE_EMITTER_TYPE !$= "Ground")
   {
      $PE_EMITTER_TYPE = "Ground";
      ParticleEditor.createParticleList();
      PEE_EmitterSelector.setFirstSelected();
   }

}

function ParticleEditorMaskEmitterButton::onClick(%this)
{
   if($PE_EMITTER_TYPE !$= "Mask")
   {
      $PE_EMITTER_TYPE = "Mask";
      ParticleEditor.createParticleList();
      PEE_EmitterSelector.setFirstSelected();
   }

}

function ParticleEditorParticleEffectButton::onClick(%this)
{
   if($PE_EMITTER_TYPE !$= "Effect")
   {
      $PE_EMITTER_TYPE = "Effect";
      ParticleEditor.createParticleList();
      PEE_EmitterSelector.setFirstSelected();
   }

}

//-----------------------------------------------------------------------------