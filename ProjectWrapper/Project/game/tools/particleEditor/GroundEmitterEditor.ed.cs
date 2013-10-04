function PE_GroundEmitterEditor::guiSync( %this, %new )
{   
   %data = PE_EmitterEditor.currEmitter;
   // Sync up sliders and number boxes.
      
   if( PE_GroundEmitterEditor-->PEE_infiniteLoop.isStateOn() )
   {
      PE_GroundEmitterEditor-->PEE_lifetimeMS_slider.setActive( false );
      PE_GroundEmitterEditor-->PEE_lifetimeMS_textEdit.setActive( false );
      PE_GroundEmitterEditor-->PEE_lifetimeVarianceMS_slider.setActive( false );
      PE_GroundEmitterEditor-->PEE_lifetimeVarianceMS_textEdit.setActive( false );
   }
   else
   {
      PE_GroundEmitterEditor-->PEE_lifetimeMS_slider.setActive( true );
      PE_GroundEmitterEditor-->PEE_lifetimeMS_textEdit.setActive( true );
      PE_GroundEmitterEditor-->PEE_lifetimeVarianceMS_slider.setActive( true );
      PE_GroundEmitterEditor-->PEE_lifetimeVarianceMS_textEdit.setActive( true );
      
      PE_GroundEmitterEditor-->PEE_lifetimeMS_slider.setValue( %data.lifetimeMS );
      PE_GroundEmitterEditor-->PEE_lifetimeMS_textEdit.setText( %data.lifetimeMS );
      PE_GroundEmitterEditor-->PEE_lifetimeVarianceMS_slider.setValue( %data.lifetimeVarianceMS );
      PE_GroundEmitterEditor-->PEE_lifetimeVarianceMS_textEdit.setText( %data.lifetimeVarianceMS );
   }
         
   PE_GroundEmitterEditor-->PEE_ejectionPeriodMS_slider.setValue( %data.ejectionPeriodMS );
   PE_GroundEmitterEditor-->PEE_ejectionPeriodMS_textEdit.setText( %data.ejectionPeriodMS );

   PE_GroundEmitterEditor-->PEE_periodVarianceMS_slider.setValue( %data.periodVarianceMS );
   PE_GroundEmitterEditor-->PEE_periodVarianceMS_textEdit.setText( %data.periodVarianceMS );
   
   PE_GroundEmitterEditor-->PEE_ejectionVelocity_slider.setValue( %data.ejectionVelocity );
   PE_GroundEmitterEditor-->PEE_ejectionVelocity_textEdit.setText( %data.ejectionVelocity );
   
   PE_GroundEmitterEditor-->PEE_velocityVariance_slider.setValue( %data.velocityVariance );
   PE_GroundEmitterEditor-->PEE_velocityVariance_textEdit.setText( %data.velocityVariance );
   
   PE_GroundEmitterEditor-->PEE_orientParticles.setValue( %data.orientParticles );
   PE_GroundEmitterEditor-->PEE_alignParticles.setValue( %data.alignParticles );
   PE_GroundEmitterEditor-->PEE_alignDirection.setText( %data.alignDirection );
   
   PE_GroundEmitterEditor-->PEE_ejectionOffset_slider.setValue( %data.ejectionOffset );
   PE_GroundEmitterEditor-->PEE_ejectionOffset_textEdit.setText( %data.ejectionOffset );
   
   %blendTypeId = PE_GroundEmitterEditor-->PEE_blendType.findText( %data.blendStyle );
   PE_GroundEmitterEditor-->PEE_blendType.setSelected( %blendTypeId, false );
   
   PE_GroundEmitterEditor-->PEE_softnessDistance_slider.setValue( %data.softnessDistance );
   PE_GroundEmitterEditor-->PEE_softnessDistance_textEdit.setText( %data.softnessDistance );
   
   PE_GroundEmitterEditor-->PEE_ambientFactor_slider.setValue( %data.ambientFactor );
   PE_GroundEmitterEditor-->PEE_ambientFactor_textEdit.setText( %data.ambientFactor );
   
   PE_GroundEmitterEditor-->PEE_softParticles.setValue( %data.softParticles );
   PE_GroundEmitterEditor-->PEE_reverseOrder.setValue( %data.reverseOrder );
   PE_GroundEmitterEditor-->PEE_useEmitterSizes.setValue( %data.useEmitterSizes );
   PE_GroundEmitterEditor-->PEE_useEmitterColors.setValue( %data.useEmitterColors );
   
   PE_GroundEmitterEditor-->PEE_Radius_slider.setValue( %data.radius );
   PE_GroundEmitterEditor-->PEE_Radius_textEdit.setText( %data.radius );
   

   // Sync up particle selectors.
   
   for( %index = 0; %index < 4; %index ++ )
   {
      %ctrl = "GroundEE_EmitterParticle" @ ( %index + 1 );
      %popup = %ctrl-->PopUpMenu;

      %particle = getWord( %data.particles, %index );
      if( isObject( %particle ) )
         %popup.setSelected( %particle.getId(), false );
      else
         %popup.setSelected( 0, false ); // Select "None".
   }
   
   for(%idx = 0; %idx < $IPS::GroundEmitterLayers; %idx++)
   {
      %textEdit = GroundEE_LayersContainer.findObjectByInternalName("PEE_Layer"@%idx@"_textEdit", true);
      %textEdit.setText( %data.layers[%idx] );
   }
   
   if(%new)
      PE_GroundEmitterEditor-->PEE_infiniteLoop.setStateOn( PE_EmitterEditor.currEmitter.lifetimeMS == 0 );
   
   PE_EmitterEditor.NotDirtyEmitter = PE_GroundEmitterEditor_NotDirtyEmitter;   
}

function PE_GroundEmitterEditor::initEditor( %this )
{   
   datablock GroundEmitterData(PE_GroundEmitterEditor_NotDirtyEmitter)
   {
      particles = "DefaultParticle";
   };
   
   PE_UnlistedEmitters.add( PE_GroundEmitterEditor_NotDirtyEmitter );
   
   GroundEE_EmitterParticleSelector1.clear();
   GroundEE_EmitterParticleSelector2.clear();
   GroundEE_EmitterParticleSelector3.clear();
   GroundEE_EmitterParticleSelector4.clear();
   
   PEP_ParticleSelector.clear();
   
   for(%idx = 0; %idx < $IPS::GroundEmitterLayers; %idx++)
   {
      %guicontrol = new GuiControl(){ // Layers
            class = "AggregateControl";
            isContainer = "1";
            HorizSizing = "width";
            VertSizing = "bottom";
            Position = $PE_guielement_pos_single_container ;
            Extent = $PE_guielement_ext_single_container ;
            
            new GuiTextCtrl() {
                  Profile = "ToolsGuiTextProfile";
                  HorizSizing = "width";
                  VertSizing = "bottom";
                  position = $PE_guielement_pos_name;
                  Extent = $PE_guielement_ext_name;
                  text = "Layer"@%idx;
               };
               
               new GuiTextEditCtrl() {
                  internalName = "PEE_Layer"@%idx@"_textEdit";
                  Profile = "ToolsGuiTextEditProfile";
                  HorizSizing = "left";
                  VertSizing = "bottom";
                  position = "56 1";
                  Extent = "90 17";
                  altCommand = "$ThisControl.getParent().updateFromChild($ThisControl); PE_EmitterEditor.updateEmitter( \"layers["@%idx@"]\", $ThisControl.getText());";
               }; 
               
               new GuiBitmapButtonCtrl() {
                  canSaveDynamicFields = "0";
                  Enabled = "1";
                  isContainer = "0";
                  Profile = "ToolsGuiButtonProfile";
                  HorizSizing = "left";
                  VertSizing = "bottom";
                  position = "159 1";
                  Extent = "17 17";
                  MinExtent = "8 2";
                  canSave = "1";
                  Visible = "1";
                  Command = "MaterialSelector.showTerrainDialog( \"PE_GroundEmitterEditor.setLayer\", \"name\"); PE_GroundEmitterEditor.selectedLayer = "@%idx@";"; 
                  hovertime = "1000";
                  tooltip = "Find a layer";
                  text = "";
                  bitmap = "tools/gui/images/layers-btn";
                  groupNum = "-1";
                  buttonType = "PushButton";
                  useMouseEvents = "0";
               }; 
      };
      GroundEE_LayersContainer.add(%guicontrol);
   }
   
   PE_GroundEmitterEditor.createParticleList();
         
   GroundEE_EmitterParticleSelector2.add( "None", 0 );
   GroundEE_EmitterParticleSelector3.add( "None", 0 );
   GroundEE_EmitterParticleSelector4.add( "None", 0 );
   
   GroundEE_EmitterParticleSelector1.sort();
   GroundEE_EmitterParticleSelector2.sort();
   GroundEE_EmitterParticleSelector3.sort();
   GroundEE_EmitterParticleSelector4.sort();
         
   PE_GroundEmitterEditor-->PEE_blendType.clear();
   PE_GroundEmitterEditor-->PEE_blendType.add( "NORMAL", 0 );
   PE_GroundEmitterEditor-->PEE_blendType.add( "ADDITIVE", 1 );
   PE_GroundEmitterEditor-->PEE_blendType.add( "SUBTRACTIVE", 2 );
   PE_GroundEmitterEditor-->PEE_blendType.add( "PREMULTALPHA", 3 );
}

function PE_GroundEmitterEditor::createParticleList( %this )
{
   %particleCount = 0;
   
   foreach( %obj in DatablockGroup )
   {
      if( %obj.isMemberOfClass( "BillboardParticleData" ) ||
          %obj.isMemberOfClass( "BillboardRibbonParticleData" ) ||
          %obj.isMemberOfClass( "EmitterParticleData" ) ||
          %obj.isMemberOfClass( "EffectParticleData" ) ||
          %obj.isMemberOfClass( "TSShapeParticleData" ) ||
          %obj.isMemberOfClass( "PointLightParticleData" ) )
      {
         %unlistedFound = false;
         foreach( %unlisted in PE_UnlistedParticles )
            if( %unlisted.getId() == %obj.getId() )
            {
               %unlistedFound = true;
               break;
            }
      
         if( %unlistedFound )
            continue;
            
         %name = %obj.getName();
         %id = %obj.getId();
         
         //if ( %name $= "DefaultParticle")
         //   continue;

         // Add to particle dropdown selectors.
         
         GroundEE_EmitterParticleSelector1.add( %name, %id );
         GroundEE_EmitterParticleSelector2.add( %name, %id );
         GroundEE_EmitterParticleSelector3.add( %name, %id );
         GroundEE_EmitterParticleSelector4.add( %name, %id );
                  
         %particleCount ++;
      }
   }
   
   GroundEE_EmitterParticleSelector1.sort();   
   GroundEE_EmitterParticleSelector2.sort();
   GroundEE_EmitterParticleSelector3.sort();
   GroundEE_EmitterParticleSelector4.sort();   
}

function PE_GroundEmitterEditor::setLayer(%this, %name)
{
   %textEdit = GroundEE_LayersContainer.findObjectByInternalName("PEE_Layer"@%this.selectedLayer@"_textEdit", true);
   %textEdit.setText(%name);
   PE_EmitterEditor.updateEmitter( "layers["@%this.selectedLayer@"]", %textEdit.getText());
   %this.selectedLayer = "";
}