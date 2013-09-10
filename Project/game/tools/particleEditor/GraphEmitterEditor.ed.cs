function PE_GraphEmitterEditor::guiSync( %this, %new )
{   
   %data = PE_EmitterEditor.currEmitter;
   // Sync up sliders and number boxes.
      
   if( PE_GraphEmitterEditor-->PEE_infiniteLoop.isStateOn() )
   {
      PE_GraphEmitterEditor-->PEE_lifetimeMS_slider.setActive( false );
      PE_GraphEmitterEditor-->PEE_lifetimeMS_textEdit.setActive( false );
      PE_GraphEmitterEditor-->PEE_lifetimeVarianceMS_slider.setActive( false );
      PE_GraphEmitterEditor-->PEE_lifetimeVarianceMS_textEdit.setActive( false );
   }
   else
   {
      PE_GraphEmitterEditor-->PEE_lifetimeMS_slider.setActive( true );
      PE_GraphEmitterEditor-->PEE_lifetimeMS_textEdit.setActive( true );
      PE_GraphEmitterEditor-->PEE_lifetimeVarianceMS_slider.setActive( true );
      PE_GraphEmitterEditor-->PEE_lifetimeVarianceMS_textEdit.setActive( true );
      
      PE_GraphEmitterEditor-->PEE_lifetimeMS_slider.setValue( %data.lifetimeMS );
      PE_GraphEmitterEditor-->PEE_lifetimeMS_textEdit.setText( %data.lifetimeMS );
      PE_GraphEmitterEditor-->PEE_lifetimeVarianceMS_slider.setValue( %data.lifetimeVarianceMS );
      PE_GraphEmitterEditor-->PEE_lifetimeVarianceMS_textEdit.setText( %data.lifetimeVarianceMS );
   }
         
   PE_GraphEmitterEditor-->PEE_ejectionPeriodMS_slider.setValue( %data.ejectionPeriodMS );
   PE_GraphEmitterEditor-->PEE_ejectionPeriodMS_textEdit.setText( %data.ejectionPeriodMS );

   PE_GraphEmitterEditor-->PEE_periodVarianceMS_slider.setValue( %data.periodVarianceMS );
   PE_GraphEmitterEditor-->PEE_periodVarianceMS_textEdit.setText( %data.periodVarianceMS );
   
   PE_GraphEmitterEditor-->PEE_ejectionVelocity_slider.setValue( %data.ejectionVelocity );
   PE_GraphEmitterEditor-->PEE_ejectionVelocity_textEdit.setText( %data.ejectionVelocity );
   
   PE_GraphEmitterEditor-->PEE_velocityVariance_slider.setValue( %data.velocityVariance );
   PE_GraphEmitterEditor-->PEE_velocityVariance_textEdit.setText( %data.velocityVariance );
   
   PE_GraphEmitterEditor-->PEE_orientParticles.setValue( %data.orientParticles );
   PE_GraphEmitterEditor-->PEE_alignParticles.setValue( %data.alignParticles );
   PE_GraphEmitterEditor-->PEE_alignDirection.setText( %data.alignDirection );
   
   PE_GraphEmitterEditor-->PEE_ejectionOffset_slider.setValue( %data.ejectionOffset );
   PE_GraphEmitterEditor-->PEE_ejectionOffset_textEdit.setText( %data.ejectionOffset );
   
   %blendTypeId = PE_GraphEmitterEditor-->PEE_blendType.findText( %data.blendStyle );
   PE_GraphEmitterEditor-->PEE_blendType.setSelected( %blendTypeId, false );
   
   PE_GraphEmitterEditor-->PEE_softnessDistance_slider.setValue( %data.softnessDistance );
   PE_GraphEmitterEditor-->PEE_softnessDistance_textEdit.setText( %data.softnessDistance );
   
   PE_GraphEmitterEditor-->PEE_ambientFactor_slider.setValue( %data.ambientFactor );
   PE_GraphEmitterEditor-->PEE_ambientFactor_textEdit.setText( %data.ambientFactor );
   
   PE_GraphEmitterEditor-->PEE_softParticles.setValue( %data.softParticles );
   PE_GraphEmitterEditor-->PEE_reverseOrder.setValue( %data.reverseOrder );
   PE_GraphEmitterEditor-->PEE_useEmitterSizes.setValue( %data.useEmitterSizes );
   PE_GraphEmitterEditor-->PEE_useEmitterColors.setValue( %data.useEmitterColors );
   
   PE_GraphEmitterEditor-->PEE_xFunc.setValue( %data.xFunc );
   PE_GraphEmitterEditor-->PEE_yFunc.setValue( %data.yFunc );
   PE_GraphEmitterEditor-->PEE_zFunc.setValue( %data.zFunc );
   PE_GraphEmitterEditor-->PEE_funcMin.setValue( %data.funcMin );
   PE_GraphEmitterEditor-->PEE_funcMax.setValue( %data.funcMax );
   PE_GraphEmitterEditor-->PEE_timeScale.setValue( %data.timeScale );
   PE_GraphEmitterEditor-->PEE_timeScale.setValue( %data.timeScale );
   PE_GraphEmitterEditor-->PEE_progressMode.setValue( %data.ProgressMode );
   %progModeId = PE_GraphEmitterEditor-->PEE_progressMode.findText( %data.ProgressMode );
   PE_GraphEmitterEditor-->PEE_progressMode.setSelected( %progModeId, false );
   PE_GraphEmitterEditor-->PEE_Reverse.setValue( %data.Reverse );
   PE_GraphEmitterEditor-->PEE_Grounded.setValue( %data.Grounded );

   // Sync up particle selectors.
   
   for( %index = 0; %index < 4; %index ++ )
   {
      %ctrl = "GraphEE_EmitterParticle" @ ( %index + 1 );
      %popup = %ctrl-->PopUpMenu;

      %particle = getWord( %data.particles, %index );
      if( isObject( %particle ) )
         %popup.setSelected( %particle.getId(), false );
      else
         %popup.setSelected( 0, false ); // Select "None".
   }
   
   if(%new)
      PE_GraphEmitterEditor-->PEE_infiniteLoop.setStateOn( PE_EmitterEditor.currEmitter.lifetimeMS == 0 );
   
   PE_EmitterEditor.NotDirtyEmitter = PE_GraphEmitterEditor_NotDirtyEmitter;   
}

function PE_GraphEmitterEditor::initEditor( %this )
{   
   datablock GraphEmitterData(PE_GraphEmitterEditor_NotDirtyEmitter)
   {
      particles = "DefaultParticle";
   };
   
   PE_UnlistedEmitters.add( PE_GraphEmitterEditor_NotDirtyEmitter );
   
   GraphEE_EmitterParticleSelector1.clear();
   GraphEE_EmitterParticleSelector2.clear();
   GraphEE_EmitterParticleSelector3.clear();
   GraphEE_EmitterParticleSelector4.clear();
   
   PEP_ParticleSelector.clear();
   
   PE_GraphEmitterEditor.createParticleList();
         
   GraphEE_EmitterParticleSelector2.add( "None", 0 );
   GraphEE_EmitterParticleSelector3.add( "None", 0 );
   GraphEE_EmitterParticleSelector4.add( "None", 0 );
   
   GraphEE_EmitterParticleSelector1.sort();
   GraphEE_EmitterParticleSelector2.sort();
   GraphEE_EmitterParticleSelector3.sort();
   GraphEE_EmitterParticleSelector4.sort();
         
   PE_GraphEmitterEditor-->PEE_blendType.clear();
   PE_GraphEmitterEditor-->PEE_blendType.add( "NORMAL", 0 );
   PE_GraphEmitterEditor-->PEE_blendType.add( "ADDITIVE", 1 );
   PE_GraphEmitterEditor-->PEE_blendType.add( "SUBTRACTIVE", 2 );
   PE_GraphEmitterEditor-->PEE_blendType.add( "PREMULTALPHA", 3 );
   
   PE_GraphEmitterEditor-->PEE_progressMode.clear();
   PE_GraphEmitterEditor-->PEE_progressMode.add( "byParticleCount", 0 );
   PE_GraphEmitterEditor-->PEE_progressMode.add( "byTime", 1 );
}

function PE_GraphEmitterEditor::createParticleList( %this )
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
         
         //if ( %name $= "DefaultParticle")
         //   continue;

         // Add to particle dropdown selectors.
         
         GraphEE_EmitterParticleSelector1.add( %name, %id );
         GraphEE_EmitterParticleSelector2.add( %name, %id );
         GraphEE_EmitterParticleSelector3.add( %name, %id );
         GraphEE_EmitterParticleSelector4.add( %name, %id );
                  
         %particleCount ++;
      }
   }
   
   GraphEE_EmitterParticleSelector1.sort();   
   GraphEE_EmitterParticleSelector2.sort();
   GraphEE_EmitterParticleSelector3.sort();
   GraphEE_EmitterParticleSelector4.sort();   
}