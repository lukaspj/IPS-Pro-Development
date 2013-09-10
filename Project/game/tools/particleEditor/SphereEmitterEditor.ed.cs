function PE_SphereEmitterEditor::guiSync( %this, %new )
{   
   %data = PE_EmitterEditor.currEmitter;
   // Sync up sliders and number boxes.
      
   if( PE_SphereEmitterEditor-->PEE_infiniteLoop.isStateOn() )
   {
      PE_SphereEmitterEditor-->PEE_lifetimeMS_slider.setActive( false );
      PE_SphereEmitterEditor-->PEE_lifetimeMS_textEdit.setActive( false );
      PE_SphereEmitterEditor-->PEE_lifetimeVarianceMS_slider.setActive( false );
      PE_SphereEmitterEditor-->PEE_lifetimeVarianceMS_textEdit.setActive( false );
   }
   else
   {
      PE_SphereEmitterEditor-->PEE_lifetimeMS_slider.setActive( true );
      PE_SphereEmitterEditor-->PEE_lifetimeMS_textEdit.setActive( true );
      PE_SphereEmitterEditor-->PEE_lifetimeVarianceMS_slider.setActive( true );
      PE_SphereEmitterEditor-->PEE_lifetimeVarianceMS_textEdit.setActive( true );
      
      PE_SphereEmitterEditor-->PEE_lifetimeMS_slider.setValue( %data.lifetimeMS );
      PE_SphereEmitterEditor-->PEE_lifetimeMS_textEdit.setText( %data.lifetimeMS );
      PE_SphereEmitterEditor-->PEE_lifetimeVarianceMS_slider.setValue( %data.lifetimeVarianceMS );
      PE_SphereEmitterEditor-->PEE_lifetimeVarianceMS_textEdit.setText( %data.lifetimeVarianceMS );
   }
         
   PE_SphereEmitterEditor-->PEE_ejectionPeriodMS_slider.setValue( %data.ejectionPeriodMS );
   PE_SphereEmitterEditor-->PEE_ejectionPeriodMS_textEdit.setText( %data.ejectionPeriodMS );

   PE_SphereEmitterEditor-->PEE_periodVarianceMS_slider.setValue( %data.periodVarianceMS );
   PE_SphereEmitterEditor-->PEE_periodVarianceMS_textEdit.setText( %data.periodVarianceMS );
   
   PE_SphereEmitterEditor-->PEE_ejectionVelocity_slider.setValue( %data.ejectionVelocity );
   PE_SphereEmitterEditor-->PEE_ejectionVelocity_textEdit.setText( %data.ejectionVelocity );
   
   PE_SphereEmitterEditor-->PEE_velocityVariance_slider.setValue( %data.velocityVariance );
   PE_SphereEmitterEditor-->PEE_velocityVariance_textEdit.setText( %data.velocityVariance );
   
   PE_SphereEmitterEditor-->PEE_orientParticles.setValue( %data.orientParticles );
   PE_SphereEmitterEditor-->PEE_alignParticles.setValue( %data.alignParticles );
   PE_SphereEmitterEditor-->PEE_alignDirection.setText( %data.alignDirection );
   
   PE_SphereEmitterEditor-->PEE_thetaMin_slider.setValue( %data.thetaMin );
   PE_SphereEmitterEditor-->PEE_thetaMin_textEdit.setText( %data.thetaMin );
   
   PE_SphereEmitterEditor-->PEE_thetaMax_slider.setValue( %data.thetaMax );
   PE_SphereEmitterEditor-->PEE_thetaMax_textEdit.setText( %data.thetaMax );
   
   PE_SphereEmitterEditor-->PEE_phiVariance_slider.setValue( %data.phiVariance );
   PE_SphereEmitterEditor-->PEE_phiVariance_textEdit.setText( %data.phiVariance );
   
   PE_SphereEmitterEditor-->PEE_ejectionOffset_slider.setValue( %data.ejectionOffset );
   PE_SphereEmitterEditor-->PEE_ejectionOffset_textEdit.setText( %data.ejectionOffset );
   
   %blendTypeId = PE_SphereEmitterEditor-->PEE_blendType.findText( %data.blendStyle );
   PE_SphereEmitterEditor-->PEE_blendType.setSelected( %blendTypeId, false );
   
   PE_SphereEmitterEditor-->PEE_softnessDistance_slider.setValue( %data.softnessDistance );
   PE_SphereEmitterEditor-->PEE_softnessDistance_textEdit.setText( %data.softnessDistance );
   
   PE_SphereEmitterEditor-->PEE_ambientFactor_slider.setValue( %data.ambientFactor );
   PE_SphereEmitterEditor-->PEE_ambientFactor_textEdit.setText( %data.ambientFactor );
   
   PE_SphereEmitterEditor-->PEE_softParticles.setValue( %data.softParticles );
   PE_SphereEmitterEditor-->PEE_reverseOrder.setValue( %data.reverseOrder );
   PE_SphereEmitterEditor-->PEE_useEmitterSizes.setValue( %data.useEmitterSizes );
   PE_SphereEmitterEditor-->PEE_useEmitterColors.setValue( %data.useEmitterColors );

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
   
   if(%new)
      PE_SphereEmitterEditor-->PEE_infiniteLoop.setStateOn( PE_EmitterEditor.currEmitter.lifetimeMS == 0 );   
   
   PE_EmitterEditor.NotDirtyEmitter = PE_SphereEmitterEditor_NotDirtyEmitter; 
}

function PE_SphereEmitterEditor::initEditor( %this )
{
   datablock SphereEmitterData(PE_SphereEmitterEditor_NotDirtyEmitter)
   {
      particles = "DefaultParticle";
   };
   
   PE_UnlistedEmitters.add( PE_SphereEmitterEditor_NotDirtyEmitter );
   
   PEE_EmitterParticleSelector1.clear();
   PEE_EmitterParticleSelector2.clear();
   PEE_EmitterParticleSelector3.clear();
   PEE_EmitterParticleSelector4.clear();
   
   PE_SphereEmitterEditor.createParticleList();
         
   PEE_EmitterParticleSelector2.add( "None", 0 );
   PEE_EmitterParticleSelector3.add( "None", 0 );
   PEE_EmitterParticleSelector4.add( "None", 0 );
   
   PEE_EmitterParticleSelector1.sort();
   PEE_EmitterParticleSelector2.sort();
   PEE_EmitterParticleSelector3.sort();
   PEE_EmitterParticleSelector4.sort();
         
   PE_SphereEmitterEditor-->PEE_blendType.clear();
   PE_SphereEmitterEditor-->PEE_blendType.add( "NORMAL", 0 );
   PE_SphereEmitterEditor-->PEE_blendType.add( "ADDITIVE", 1 );
   PE_SphereEmitterEditor-->PEE_blendType.add( "SUBTRACTIVE", 2 );
   PE_SphereEmitterEditor-->PEE_blendType.add( "PREMULTALPHA", 3 );
}

function PE_SphereEmitterEditor::createParticleList( %this )
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
         
         PEE_EmitterParticleSelector1.add( %name, %id );
         PEE_EmitterParticleSelector2.add( %name, %id );
         PEE_EmitterParticleSelector3.add( %name, %id );
         PEE_EmitterParticleSelector4.add( %name, %id );
                  
         %particleCount ++;
      }
   }
   
   PEE_EmitterParticleSelector1.sort();   
   PEE_EmitterParticleSelector2.sort();
   PEE_EmitterParticleSelector3.sort();
   PEE_EmitterParticleSelector4.sort();   
}