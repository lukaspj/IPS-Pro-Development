function PE_MaskEmitterEditor::guiSync( %this, %new )
{   
   %data = PE_EmitterEditor.currEmitter;
   // Sync up sliders and number boxes.
      
   if( PE_MaskEmitterEditor-->PEE_infiniteLoop.isStateOn() )
   {
      PE_MaskEmitterEditor-->PEE_lifetimeMS_slider.setActive( false );
      PE_MaskEmitterEditor-->PEE_lifetimeMS_textEdit.setActive( false );
      PE_MaskEmitterEditor-->PEE_lifetimeVarianceMS_slider.setActive( false );
      PE_MaskEmitterEditor-->PEE_lifetimeVarianceMS_textEdit.setActive( false );
   }
   else
   {
      PE_MaskEmitterEditor-->PEE_lifetimeMS_slider.setActive( true );
      PE_MaskEmitterEditor-->PEE_lifetimeMS_textEdit.setActive( true );
      PE_MaskEmitterEditor-->PEE_lifetimeVarianceMS_slider.setActive( true );
      PE_MaskEmitterEditor-->PEE_lifetimeVarianceMS_textEdit.setActive( true );
      
      PE_MaskEmitterEditor-->PEE_lifetimeMS_slider.setValue( %data.lifetimeMS );
      PE_MaskEmitterEditor-->PEE_lifetimeMS_textEdit.setText( %data.lifetimeMS );
      PE_MaskEmitterEditor-->PEE_lifetimeVarianceMS_slider.setValue( %data.lifetimeVarianceMS );
      PE_MaskEmitterEditor-->PEE_lifetimeVarianceMS_textEdit.setText( %data.lifetimeVarianceMS );
   }
         
   PE_MaskEmitterEditor-->PEE_ejectionPeriodMS_slider.setValue( %data.ejectionPeriodMS );
   PE_MaskEmitterEditor-->PEE_ejectionPeriodMS_textEdit.setText( %data.ejectionPeriodMS );

   PE_MaskEmitterEditor-->PEE_periodVarianceMS_slider.setValue( %data.periodVarianceMS );
   PE_MaskEmitterEditor-->PEE_periodVarianceMS_textEdit.setText( %data.periodVarianceMS );
   
   PE_MaskEmitterEditor-->PEE_ejectionVelocity_slider.setValue( %data.ejectionVelocity );
   PE_MaskEmitterEditor-->PEE_ejectionVelocity_textEdit.setText( %data.ejectionVelocity );
   
   PE_MaskEmitterEditor-->PEE_velocityVariance_slider.setValue( %data.velocityVariance );
   PE_MaskEmitterEditor-->PEE_velocityVariance_textEdit.setText( %data.velocityVariance );
   
   PE_MaskEmitterEditor-->PEE_orientParticles.setValue( %data.orientParticles );
   PE_MaskEmitterEditor-->PEE_alignParticles.setValue( %data.alignParticles );
   PE_MaskEmitterEditor-->PEE_alignDirection.setText( %data.alignDirection );
   
   PE_MaskEmitterEditor-->PEE_ejectionOffset_slider.setValue( %data.ejectionOffset );
   PE_MaskEmitterEditor-->PEE_ejectionOffset_textEdit.setText( %data.ejectionOffset );
   
   %blendTypeId = PE_MaskEmitterEditor-->PEE_blendType.findText( %data.blendStyle );
   PE_MaskEmitterEditor-->PEE_blendType.setSelected( %blendTypeId, false );
   
   PE_MaskEmitterEditor-->PEE_softnessDistance_slider.setValue( %data.softnessDistance );
   PE_MaskEmitterEditor-->PEE_softnessDistance_textEdit.setText( %data.softnessDistance );
   
   PE_MaskEmitterEditor-->PEE_ambientFactor_slider.setValue( %data.ambientFactor );
   PE_MaskEmitterEditor-->PEE_ambientFactor_textEdit.setText( %data.ambientFactor );
   
   PE_MaskEmitterEditor-->PEE_softParticles.setValue( %data.softParticles );
   PE_MaskEmitterEditor-->PEE_reverseOrder.setValue( %data.reverseOrder );
   PE_MaskEmitterEditor-->PEE_useEmitterSizes.setValue( %data.useEmitterSizes );
   PE_MaskEmitterEditor-->PEE_useEmitterColors.setValue( %data.useEmitterColors );
   
   PE_MaskEmitterEditor-->PEE_Radius_slider.setValue( %data.Radius );
   PE_MaskEmitterEditor-->PEE_Radius_textEdit.setText( %data.Radius );
   PE_MaskEmitterEditor-->PEE_ThresholdMax_slider.setValue( %data.Threshold_max );
   PE_MaskEmitterEditor-->PEE_ThresholdMax_textEdit.setText( %data.Threshold_max );
   PE_MaskEmitterEditor-->PEE_ThresholdMin_slider.setValue( %data.Threshold_min );
   PE_MaskEmitterEditor-->PEE_ThresholdMin_textEdit.setText( %data.Threshold_min );
   
   MaskEE_PixelMaskSelector.setSelected( %data.PixelMask.getId(), false );

   // Sync up particle selectors.
   
   for( %index = 0; %index < 4; %index ++ )
   {
      %ctrl = "PEE_EmitterParticle" @ ( %index + 1 );
      %popup = %ctrl-->PopUpMenu;

      %particle = getWord( %data.particles, %index );
      if( isObject( %particle ) )
         %popup.setSelected( %particle.getId(), false );
      else
         %popup.setSelected( 0, false ); // Select "None".
   }
   
   foreach( %obj in DatablockGroup )
   {
      if( %obj.isMemberOfClass( "PixelMask" ) )
      {
         %name = %obj.getName();
         %id = %obj.getId();
         MaskEE_PixelMaskSelector.add( %name, %id );
      }
   }
   if(%new)
      PE_MaskEmitterEditor-->PEE_infiniteLoop.setStateOn( PE_EmitterEditor.currEmitter.lifetimeMS == 0 );   
   
   PE_EmitterEditor.NotDirtyEmitter = PE_MaskEmitterEditor_NotDirtyEmitter; 
}

function PE_MaskEmitterEditor::initEditor( %this )
{
   datablock MaskEmitterData(PE_MaskEmitterEditor_NotDirtyEmitter)
   {
      particles = "DefaultParticle";
   };
   
   PE_UnlistedEmitters.add( PE_MaskEmitterEditor_NotDirtyEmitter );
   
   MaskEE_EmitterParticleSelector1.clear();
   MaskEE_EmitterParticleSelector2.clear();
   MaskEE_EmitterParticleSelector3.clear();
   MaskEE_EmitterParticleSelector4.clear();
   
   PE_MaskEmitterEditor.createParticleList();
         
   MaskEE_EmitterParticleSelector2.add( "None", 0 );
   MaskEE_EmitterParticleSelector3.add( "None", 0 );
   MaskEE_EmitterParticleSelector4.add( "None", 0 );
   
   MaskEE_EmitterParticleSelector1.sort();
   MaskEE_EmitterParticleSelector2.sort();
   MaskEE_EmitterParticleSelector3.sort();
   MaskEE_EmitterParticleSelector4.sort();
         
   PE_MaskEmitterEditor-->PEE_blendType.clear();
   PE_MaskEmitterEditor-->PEE_blendType.add( "NORMAL", 0 );
   PE_MaskEmitterEditor-->PEE_blendType.add( "ADDITIVE", 1 );
   PE_MaskEmitterEditor-->PEE_blendType.add( "SUBTRACTIVE", 2 );
   PE_MaskEmitterEditor-->PEE_blendType.add( "PREMULTALPHA", 3 );
}

function PE_MaskEmitterEditor::createParticleList( %this )
{
   %particleCount = 0;
   
   foreach( %obj in DatablockGroup )
   {
      if( %obj.isMemberOfClass( "BillboardParticleData" ) )
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
         
         if ( %name $= "DefaultParticle")
            continue;

         // Add to particle dropdown selectors.
         
         MaskEE_EmitterParticleSelector1.add( %name, %id );
         MaskEE_EmitterParticleSelector2.add( %name, %id );
         MaskEE_EmitterParticleSelector3.add( %name, %id );
         MaskEE_EmitterParticleSelector4.add( %name, %id );
                  
         %particleCount ++;
      }
   }
   
   MaskEE_EmitterParticleSelector1.sort();   
   MaskEE_EmitterParticleSelector2.sort();
   MaskEE_EmitterParticleSelector3.sort();
   MaskEE_EmitterParticleSelector4.sort();   
}

function MaskEE_PixelMask::SelectionChanged(%this)
{
   %this.getParent().updateFromChild(%this); 
   PE_EmitterEditor.updateEmitter( "PixelMask", %this.getValue(), true, false );
}