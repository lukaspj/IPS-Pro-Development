function PEP_TSShapeParticleEditor::guiSync( %this, %new )
{   
   %data = PE_ParticleEditor.currParticle;
   
   PEP_TSShapeParticleEditor-->PEP_lifetimeMS_slider.setValue( %data.lifetimeMS );
   PEP_TSShapeParticleEditor-->PEP_lifetimeMS_textEdit.setText( %data.lifetimeMS );
   
   PEP_TSShapeParticleEditor-->PEP_lifetimeVarianceMS_slider.setValue( %data.lifetimeVarianceMS );
   PEP_TSShapeParticleEditor-->PEP_lifetimeVarianceMS_textEdit.setText( %data.lifetimeVarianceMS );
   
   PEP_TSShapeParticleEditor-->PEP_inheritedVelFactor_slider.setValue( %data.inheritedVelFactor );
   PEP_TSShapeParticleEditor-->PEP_inheritedVelFactor_textEdit.setText( %data.inheritedVelFactor );
   
   PEP_TSShapeParticleEditor-->PEP_constantAcceleration_slider.setValue( %data.constantAcceleration );
   PEP_TSShapeParticleEditor-->PEP_constantAcceleration_textEdit.setText( %data.constantAcceleration );
   
   PEP_TSShapeParticleEditor-->PEP_gravityCoefficient_slider.setValue( %data.gravityCoefficient );
   PEP_TSShapeParticleEditor-->PEP_gravityCoefficient_textEdit.setText( %data.gravityCoefficient );
   
   PEP_TSShapeParticleEditor-->PEP_dragCoefficient_slider.setValue( %data.dragCoefficient );
   PEP_TSShapeParticleEditor-->PEP_dragCoefficient_textEdit.setText( %data.dragCoefficient );
   
   PEP_TSShapeParticleEditor-->PE_ColorTintSwatch0.color = %data.colors[ 0 ];
   PEP_TSShapeParticleEditor-->PE_ColorTintSwatch1.color = %data.colors[ 1 ];
   PEP_TSShapeParticleEditor-->PE_ColorTintSwatch2.color = %data.colors[ 2 ];
   PEP_TSShapeParticleEditor-->PE_ColorTintSwatch3.color = %data.colors[ 3 ];
   
   PEP_TSShapeParticleEditor-->PEP_pointTime_slider0.setValue( %data.times[ 0 ] );
   PEP_TSShapeParticleEditor-->PEP_pointTime_textEdit0.setText( %data.times[ 0 ] );
   
   PEP_TSShapeParticleEditor-->PEP_pointTime_slider1.setValue( %data.times[ 1 ] );
   PEP_TSShapeParticleEditor-->PEP_pointTime_textEdit1.setText( %data.times[ 1 ] );
   
   PEP_TSShapeParticleEditor-->PEP_pointTime_slider2.setValue( %data.times[ 2 ] );
   PEP_TSShapeParticleEditor-->PEP_pointTime_textEdit2.setText( %data.times[ 2 ] );
   
   PEP_TSShapeParticleEditor-->PEP_pointTime_slider3.setValue( %data.times[ 3 ] );
   PEP_TSShapeParticleEditor-->PEP_pointTime_textEdit3.setText( %data.times[ 3 ] );
   
   PEP_ParticleClassSelector.setSelected(4);
   
   PEP_TSShapeParticleEditor-->PEP_Effect_PopUp.clear();
   foreach( %obj in DatablockGroup )
   {
      if( %obj.isMemberOfClass( "EffectParticleData" ) )
      {
         %name = %obj.getName();
         %id = %obj.getId();
         PEP_TSShapeParticleEditor-->PEP_Effect_PopUp.add( %name, %id );
      }
   }
   PEP_TSShapeParticleEditor-->PEP_Effect_PopUp.setSelected(%data.getId());
}