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


$PE_PARTICLEEDITOR_DEFAULT_FILENAME = "art/particles/managedParticleData.cs";


//=============================================================================================
//    PE_ParticleEditor.
//=============================================================================================

//---------------------------------------------------------------------------------------------

function PE_ParticleEditor::guiSync( %this )
{
   // Populate the selector with the particles assigned
   // to the current emitter.
   
   %containsCurrParticle = false;
   %popup = PEP_ParticleSelector;
   %popup.clear();
   
   foreach$( %particle in PE_EmitterEditor.currEmitter.particles )
   {
      if( %particle.getId() == PE_ParticleEditor.currParticle )
         %containsCurrParticle = true;
      
      %popup.add( %particle, %particle.getId() );
   }
   
   // Just in case the particle doesn't exist, fallback gracefully

   if( !%containsCurrParticle )
      PE_ParticleEditor.currParticle = getWord( PE_EmitterEditor.currEmitter.particles, 0 ).getId();

   %data = PE_ParticleEditor.currParticle;
   
   %popup.sort();
   %popup.setSelected( %data );
   
   for(%c = PE_ParticleEditor_Container.getCount() - 1; %c >= 0; %c--)
      if(PE_ParticleEditor_Container.getObject(%c) !$= PEP_ParticleClassSelector_Control)
         PE_ParticleEditor_Container.remove(PE_ParticleEditor_Container.getObject(%c));
   if(!%data)
   {
      PE_ParticleEditor_Container.add(PE_EmptyEditor);
      setWord(PE_ParticleEditor_Container.extent, 1, getWord(PE_EmptyEditor.extent, 1) - 25);
      PE_ParticleEditor_Container.computeSizes();
   }
   else if(%data.isMemberOfClass( "BillboardParticleData" ) )
   {
      PE_ParticleEditor_Container.add(PEP_BillboardParticleEditor);
      PEP_BillboardParticleEditor.guiSync();
   }
   else if(%data.isMemberOfClass( "TSShapeParticleData" ) )
   {
      PE_ParticleEditor_Container.add(PEP_TSShapeParticleEditor);
      PEP_TSShapeParticleEditor.guiSync();
   }
   else if(%data.isMemberOfClass( "EmitterParticleData" ) )
   {
      PE_ParticleEditor_Container.add(PEP_EmitterParticleEditor);
      PEP_EmitterParticleEditor.guiSync();
   }
   else if(%data.isMemberOfClass( "PointLightParticleData" ) )
   {
      PE_ParticleEditor_Container.add(PEP_PointLightParticleEditor);
      PEP_PointLightParticleEditor.guiSync();
   }
   else if(%data.isMemberOfClass( "EffectParticleData" ) )
   {
      PE_ParticleEditor_Container.add(PEP_EffectParticleEditor);
      PEP_EffectParticleEditor.guiSync();
   }
   else if(%data.isMemberOfClass( "BillboardRibbonParticleData" ) )
   {
      PE_ParticleEditor_Container.add(PEP_BillboardRibbonParticleEditor);
      PEP_BillboardRibbonParticleEditor.guiSync();
   }
   else
   {
   }
   PE_EmitterEditor_Container.setExtent(setWord(PE_EmitterEditor_Container.extent, 1, getWord(PE_Window.extent, 1) - 76));
   return;
}

//---------------------------------------------------------------------------------------------

// Generic updateParticle method
function PE_ParticleEditor::updateParticle(%this, %propertyField, %value, %isSlider, %onMouseUp)
{
   PE_ParticleEditor.setParticleDirty();
   %particle = PE_ParticleEditor.currParticle;
      
   %last = Editor.getUndoManager().getUndoAction(Editor.getUndoManager().getUndoCount() - 1);
   if( (%isSlider) && (%last.isSlider) && (!%last.onMouseUp) )
   {
      %last.field = %propertyField;
      %last.isSlider = %isSlider;
      %last.onMouseUp = %onMouseUp;
      %last.newValue = %value;
   }
   else
   {
      %action = ParticleEditor.createUndo(ActionUpdateActiveParticle, "Update Active Particle");
      %action.particle = %particle;
      %action.field = %propertyField;
      %action.isSlider = %isSlider;
      %action.onMouseUp = %onMouseUp;
      %action.newValue = %value;
      %action.oldValue = %particle.getFieldValue( %propertyField );
      
      ParticleEditor.submitUndo( %action );
   }
   
   %particle.setFieldValue( %propertyField, %value );
   %particle.reload();
}

//---------------------------------------------------------------------------------------------

// Special case updateEmitter methods
function PE_ParticleEditor::updateParticleTexture( %this, %action )
{
   if( %action )
   {
      %texture = MaterialEditorGui.openFile("texture");
      if( %texture !$= "" )
      {
         PE_ParticleEditor-->PEP_previewImage.setBitmap(%texture);
         PE_ParticleEditor-->PEP_previewImageName.setText(%texture);
         PE_ParticleEditor-->PEP_previewImageName.tooltip = %texture;
         
         PE_ParticleEditor.updateParticle( "textureName", %texture );
      }
   }
   else
   {
      PE_ParticleEditor-->PEP_previewImage.setBitmap("");
      PE_ParticleEditor-->PEP_previewImageName.setText("");
      PE_ParticleEditor-->PEP_previewImageName.tooltip = "";
      
      PE_ParticleEditor.updateParticle( "textureName", "" );
   }
}

//---------------------------------------------------------------------------------------------

function PE_ParticleEditor::updateLifeFields( %this, %isRandom, %value, %isSlider, %onMouseUp )
{
   PE_ParticleEditor.setParticleDirty();
   %particle = PE_ParticleEditor.currParticle;
      
   //Transfer values over to gui controls.
   
   if( %isRandom )
   {
      %value ++;
      if( %value > PE_ParticleEditor-->PEP_lifetimeMS_slider.getValue() )
      {
         PE_ParticleEditor-->PEP_lifetimeMS_textEdit.setText( %value );
         PE_ParticleEditor-->PEP_lifetimeMS_slider.setValue( %value );
      }
   }
   else
   {
      %value --;
      if( %value < PE_ParticleEditor-->PEP_lifetimeVarianceMS_slider.getValue() )
      {
         PE_ParticleEditor-->PEP_lifetimeVarianceMS_textEdit.setText( %value );
         PE_ParticleEditor-->PEP_lifetimeVarianceMS_slider.setValue( %value );
      }
   }
   
   // Submit undo.
   
   %last = Editor.getUndoManager().getUndoAction(Editor.getUndoManager().getUndoCount() - 1);
   if( (%isSlider) && (%last.isSlider) && (!%last.onMouseUp) )
   {
      %last.isSlider = %isSlider;
      %last.onMouseUp = %onMouseUp;
      %last.newValueLifetimeMS = PE_ParticleEditor-->PEP_lifetimeMS_textEdit.getText();
      %last.newValueLifetimeVarianceMS = PE_ParticleEditor-->PEP_lifetimeVarianceMS_textEdit.getText();
   }
   else
   {
      %action = ParticleEditor.createUndo(ActionUpdateActiveParticleLifeFields, "Update Active Particle");
      %action.particle = %particle;
      %action.isSlider = %isSlider;
      %action.onMouseUp = %onMouseUp;
      
      %action.newValueLifetimeMS = PE_ParticleEditor-->PEP_lifetimeMS_textEdit.getText();
      %action.oldValueLifetimeMS = %particle.lifetimeMS;
      
      %action.newValueLifetimeVarianceMS = PE_ParticleEditor-->PEP_lifetimeVarianceMS_textEdit.getText();
      %action.oldValueLifetimeVarianceMS = %particle.lifetimeVarianceMS;
      
      ParticleEditor.submitUndo( %action );
   }
   
   %particle.lifetimeMS = PE_ParticleEditor-->PEP_lifetimeMS_textEdit.getText();
   %particle.lifetimeVarianceMS = PE_ParticleEditor-->PEP_lifetimeVarianceMS_textEdit.getText();
   %particle.reload();   
}

//---------------------------------------------------------------------------------------------

function PE_ParticleEditor::updateSpinFields( %this, %isMax, %value, %isSlider, %onMouseUp )
{
   PE_ParticleEditor.setParticleDirty();
   %particle = PE_ParticleEditor.currParticle;
      
   // Transfer values over to gui controls.
   if( %isMax )
   {
      %value ++;
      if( %value > PE_ParticleEditor-->PEP_spinRandomMax_slider.getValue() )
      {
         PE_ParticleEditor-->PEP_spinRandomMax_textEdit.setText( %value );
         PE_ParticleEditor-->PEP_spinRandomMax_slider.setValue( %value );
      }
   }
   else
   {
      %value --;
      if( %value < PE_ParticleEditor-->PEP_spinRandomMin_slider.getValue() )
      {
         PE_ParticleEditor-->PEP_spinRandomMin_textEdit.setText( %value );
         PE_ParticleEditor-->PEP_spinRandomMin_slider.setValue( %value );
      }
   }
   
   // Submit undo.
   
   %last = Editor.getUndoManager().getUndoAction(Editor.getUndoManager().getUndoCount() - 1);
   if( (%isSlider) && (%last.isSlider) && (!%last.onMouseUp) )
   {
      %last.isSlider = %isSlider;
      %last.onMouseUp = %onMouseUp;
      %last.newValueSpinRandomMax = PE_ParticleEditor-->PEP_spinRandomMax_textEdit.getText();
      %last.newValueSpinRandomMin = PE_ParticleEditor-->PEP_spinRandomMin_textEdit.getText();
   }
   else
   {
      %action = ParticleEditor.createUndo(ActionUpdateActiveParticleSpinFields, "Update Active Particle");
      %action.particle = %particle;
      %action.isSlider = %isSlider;
      %action.onMouseUp = %onMouseUp;
      
      %action.newValueSpinRandomMax = PE_ParticleEditor-->PEP_spinRandomMax_textEdit.getText();
      %action.oldValueSpinRandomMax = %particle.spinRandomMax;
      
      %action.newValueSpinRandomMin = PE_ParticleEditor-->PEP_spinRandomMin_textEdit.getText();
      %action.oldValueSpinRandomMin = %particle.spinRandomMin;
      
      ParticleEditor.submitUndo( %action );
   }
   
   %particle.spinRandomMax = PE_ParticleEditor-->PEP_spinRandomMax_textEdit.getText();
   %particle.spinRandomMin = PE_ParticleEditor-->PEP_spinRandomMin_textEdit.getText();

   %particle.reload();   
}

//---------------------------------------------------------------------------------------------

function PE_ParticleEditor::onNewParticle( %this )
{
   // Bail if the user selected the same particle.
   
   %id = PEP_ParticleSelector.getSelected();
   if( %id == PE_ParticleEditor.currParticle )
      return;
   
   // Load new particle if we're not in a dirty state
   if( PE_ParticleEditor.dirty )
   {         
      MessageBoxYesNoCancel("Save Existing Particle?", 
         "Do you want to save changes to <br><br>" @ PE_ParticleEditor.currParticle.getName(), 
         "PE_ParticleEditor.saveParticle(" @ PE_ParticleEditor.currParticle @ ");", 
         "PE_ParticleEditor.saveParticleDialogDontSave(" @ PE_ParticleEditor.currParticle @ "); PE_ParticleEditor.loadNewParticle();"
      );
   }
   else
   {
      PE_ParticleEditor.loadNewParticle();
   }
}

//---------------------------------------------------------------------------------------------

function PE_ParticleEditor::loadNewParticle( %this, %particle )
{
   if( isObject( %particle ) )
      %particle = %particle.getId();
   else
      %particle = PEP_ParticleSelector.getSelected();
      
   PE_ParticleEditor.currParticle = %particle;
   
   %particle.reload();
   
   PE_ParticleEditor_NotDirtyParticle.assignFieldsFrom( %particle );
   PE_ParticleEditor_NotDirtyParticle.originalName = %particle.getName();

   PE_ParticleEditor.guiSync();
   PE_ParticleEditor.setParticleNotDirty();
}

//---------------------------------------------------------------------------------------------

function PE_ParticleEditor::setParticleDirty( %this )
{
   PE_ParticleEditor.text = "Particle *";
   PE_ParticleEditor.dirty = true;
   
   %particle = PE_ParticleEditor.currParticle;
   
   if( %particle.getFilename() $= "" || %particle.getFilename() $= "tools/particleEditor/particleParticleEditor.ed.cs" )
      PE_ParticleSaver.setDirty( %particle, $PE_PARTICLEEDITOR_DEFAULT_FILENAME );
   else
      PE_ParticleSaver.setDirty( %particle );
}

//---------------------------------------------------------------------------------------------

function PE_ParticleEditor::setParticleNotDirty( %this )
{
   PE_ParticleEditor.text = "Particle";
   PE_ParticleEditor.dirty = false;
   
   PE_ParticleSaver.clearAll();
}

//---------------------------------------------------------------------------------------------

function PE_ParticleEditor::showNewDialog( %this, %replaceSlot )
{
   // Open a dialog if the current Particle is dirty
   if( PE_ParticleEditor.dirty ) 
   {
      MessageBoxYesNoCancel("Save Particle Changes?", 
         "Do you wish to save the changes made to the <br>current particle before changing the particle?", 
         "PE_ParticleEditor.saveParticle( " @ PE_ParticleEditor.currParticle.getName() @ " ); PE_ParticleEditor.createParticle( " @ %replaceSlot @ " );", 
         "PE_ParticleEditor.saveParticleDialogDontSave( " @ PE_ParticleEditor.currParticle.getName() @ " ); PE_ParticleEditor.createParticle( " @ %replaceSlot @ " );"
      );
   }
   else
   {
      PE_ParticleEditor.createParticle( %replaceSlot );
   }
}

//---------------------------------------------------------------------------------------------

function PE_ParticleEditor::createParticle( %this, %replaceSlot )
{
   // Make sure we have a spare slot on the current emitter.
   
   if( !%replaceSlot )
   {
      %numExistingParticles = getWordCount( PE_EmitterEditor.currEmitter.particles );
      if( %numExistingParticles > 3 )
      {
         MessageBoxOK( "Error", "An emitter cannot have more than 4 particles assigned to it." );
         return;
      }
      
      %particleIndex = %numExistingParticles;
   }
   else
      %particleIndex = %replaceSlot - 1;
   
   // Create the particle datablock and add to the emitter.
   
   %newParticle = getUniqueName( "newParticle" );
   switch$(PEP_ParticleClassSelector.getSelected())
   {
      case "0":
         datablock BillboardParticleData( %newParticle : DefaultParticle )
         {
         };
      break;
      case "1":
         datablock TSShapeParticleData( %newParticle : DefaultParticle )
         {
         };
      break;
      case "2":
         datablock EmitterParticleData( %newParticle : DefaultParticle )
         {
         };
      break;
      case "3":
         datablock PointLightParticleData( %newParticle : DefaultParticle )
         {
         };
      break;
      case "4":
         datablock EffectParticleData( %newParticle : DefaultParticle )
         {
         };
      break;
      case "5":
         datablock BillboardRibbonParticleData( %newParticle : DefaultParticle )
         {
         };
      break;
      default:
         datablock BillboardParticleData( %newParticle : DefaultParticle )
         {
         };
      break;
   }
         
   // Submit undo.
   
   %action = ParticleEditor.createUndo( ActionCreateNewParticle, "Create New Particle" );
   %action.particle = %newParticle.getId();
   %action.particleIndex = %particleIndex;
   %action.prevParticle = ( "PEE_EmitterParticleSelector" @ ( %particleIndex + 1 ) ).getSelected();
   %action.emitter = PE_EmitterEditor.currEmitter;
   
   ParticleEditor.submitUndo( %action );
   
   // Execute action.
   
   %action.redo();
}

//---------------------------------------------------------------------------------------------

function PE_ParticleEditor::showDeleteDialog( %this )
{
   // Don't allow deleting DefaultParticle.
   
   if( PE_ParticleEditor.currParticle.getName() $= "DefaultParticle" )
   {
      MessageBoxOK( "Error", "Cannot delete DefaultParticle");
      return;
   }
   
   // Check to see if the particle emitter has more than 1 particle on it.
   
   if( getWordCount( PE_EmitterEditor.currEmitter.particles ) == 1 )
   {
      MessageBoxOK( "Error", "At least one particle must remain on the particle emitter.");
      return;
   }
   
   // Bring up requester for confirmation.
   
   if( isObject( PE_ParticleEditor.currParticle ) )
   {
      MessageBoxYesNoCancel( "Delete Particle?", 
         "Are you sure you want to delete<br><br>" @ PE_ParticleEditor.currParticle.getName() @ "<br><br> Particle deletion won't take affect until the engine is quit.", 
         "PE_ParticleEditor.saveParticleDialogDontSave( " @ PE_ParticleEditor.currParticle.getName() @ " ); PE_ParticleEditor.deleteParticle();", 
         "", 
         ""
      );
   }
}

//---------------------------------------------------------------------------------------------

function PE_ParticleEditor::deleteParticle( %this )
{
   %particle = PE_ParticleEditor.currParticle;
   
   // Submit undo.
   
   %action = ParticleEditor.createUndo( ActionDeleteParticle, "Delete Particle" );
   %action.particle = %particle;
   %action.emitter = PE_EmitterEditor.currEmitter;

   ParticleEditor.submitUndo( %action );
   
   // Execute action.
   
   %action.redo();
}

//---------------------------------------------------------------------------------------------

function PE_ParticleEditor::saveParticle( %this, %particle )
{
   %particle.setName( PEP_ParticleSelector.getText() );
   
   PE_ParticleEditor_NotDirtyParticle.assignFieldsFrom( %particle );
   PE_ParticleEditor_NotDirtyParticle.originalName = %particle.getName();
   
   PE_ParticleSaver.saveDirty(); 
   PE_ParticleEditor.setParticleNotDirty();
   
   ParticleEditor.createParticleList(); 
}

//---------------------------------------------------------------------------------------------

function PE_ParticleEditor::saveParticleDialogDontSave( %this, %particle )
{
   %particle.setName( PE_ParticleEditor_NotDirtyParticle.originalName );
   %particle.assignFieldsFrom( PE_ParticleEditor_NotDirtyParticle );
   
   PE_ParticleEditor.setParticleNotDirty();
}

//=============================================================================================
//    PE_ColorTintSwatch.
//=============================================================================================

//---------------------------------------------------------------------------------------------

function PE_ColorTintSwatch::updateParticleColor( %this, %color )
{
   %arrayNum = %this.arrayNum;
   
   %r = getWord( %color, 0 );
   %g = getWord( %color, 1 );
   %b = getWord( %color, 2 );
   %a = getWord( %color, 3 );
   
   %color = %r SPC %g SPC %b SPC %a;   
   %this.color = %color;
   
	PE_ParticleEditor.updateParticle( "colors[" @ %arrayNum @ "]", %color );
}

//=============================================================================================
//    PEP_ParticleSelector_Control.
//=============================================================================================

//---------------------------------------------------------------------------------------------

function PEP_ParticleSelector_Control::onRenameItem( %this )
{
   Parent::onRenameItem( %this );
   
   //FIXME: need to check for validity of name and name clashes
   
   PE_ParticleEditor.setParticleDirty();
   
   // Resort menu.
   
   %this-->PopupMenu.sort();
}

//=============================================================================================
//    PEP_NewParticleButton.
//=============================================================================================

//---------------------------------------------------------------------------------------------

function PEP_NewParticleButton::onDefaultClick( %this )
{
   PE_ParticleEditor.showNewDialog();
}

//---------------------------------------------------------------------------------------------

function PEP_NewParticleButton::onCtrlClick( %this )
{
   for( %i = 1; %i < 5; %i ++ )
   {
      %popup = "PEE_EmitterParticleSelector" @ %i;
      if( %popup.getSelected() == PEP_ParticleSelector.getSelected() )
      {
         %replaceSlot = %i;
         break;
      }
   }

   PE_ParticleEditor.showNewDialog( %replaceSlot );
}
