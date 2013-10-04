function PEP_TSShapeParticleEditor::guiSync( %this, %new )
{   
   %data = PE_ParticleEditor.currParticle;
   
   %bitmap = MaterialEditorGui.searchForTexture( %data.getName(), %data.textureName );
   if( %bitmap !$= "" )
   {
      PEP_TSShapeParticleEditor-->PEP_previewImage.setBitmap( %bitmap );
      PEP_TSShapeParticleEditor-->PEP_previewImageName.setText( %bitmap );
      PEP_TSShapeParticleEditor-->PEP_previewImageName.tooltip = %bitmap;
   }
   else
   {
      PEP_TSShapeParticleEditor-->PEP_previewImage.setBitmap( "" );
      PEP_TSShapeParticleEditor-->PEP_previewImageName.setText( "None" );
      PEP_TSShapeParticleEditor-->PEP_previewImageName.tooltip = "None";
   }
   
   PEP_TSShapeParticleEditor-->PEP_inverseAlpha.setValue( %data.useInvAlpha );
   
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
   
   PEP_TSShapeParticleEditor-->PEP_spinRandomMin_slider.setValue( %data.spinRandomMin );
   PEP_TSShapeParticleEditor-->PEP_spinRandomMin_textEdit.setText( %data.spinRandomMin );
   
   PEP_TSShapeParticleEditor-->PEP_spinRandomMax_slider.setValue( %data.spinRandomMax );
   PEP_TSShapeParticleEditor-->PEP_spinRandomMax_textEdit.setText( %data.spinRandomMax  );
   
   PEP_TSShapeParticleEditor-->PEP_spinRandomMax_slider.setValue( %data.spinRandomMax );
   PEP_TSShapeParticleEditor-->PEP_spinRandomMax_textEdit.setText( %data.spinRandomMax  );
   
   PEP_TSShapeParticleEditor-->PEP_spinSpeed_slider.setValue( %data.spinSpeed );
   PEP_TSShapeParticleEditor-->PEP_spinSpeed_textEdit.setText( %data.spinSpeed );
   
   PEP_TSShapeParticleEditor-->PE_ColorTintSwatch0.color = %data.colors[ 0 ];
   PEP_TSShapeParticleEditor-->PE_ColorTintSwatch1.color = %data.colors[ 1 ];
   PEP_TSShapeParticleEditor-->PE_ColorTintSwatch2.color = %data.colors[ 2 ];
   PEP_TSShapeParticleEditor-->PE_ColorTintSwatch3.color = %data.colors[ 3 ];
   
   PEP_TSShapeParticleEditor-->PEP_pointSize_slider0.setValue( %data.sizes[ 0 ] );
   PEP_TSShapeParticleEditor-->PEP_pointSize_textEdit0.setText( %data.sizes[ 0 ] );
   
   PEP_TSShapeParticleEditor-->PEP_pointSize_slider1.setValue( %data.sizes[ 1 ] );
   PEP_TSShapeParticleEditor-->PEP_pointSize_textEdit1.setText( %data.sizes[ 1 ] );
   
   PEP_TSShapeParticleEditor-->PEP_pointSize_slider2.setValue( %data.sizes[ 2 ] );
   PEP_TSShapeParticleEditor-->PEP_pointSize_textEdit2.setText( %data.sizes[ 2 ] );
   
   PEP_TSShapeParticleEditor-->PEP_pointSize_slider3.setValue( %data.sizes[ 3 ] );
   PEP_TSShapeParticleEditor-->PEP_pointSize_textEdit3.setText( %data.sizes[ 3 ] );
   
   PEP_TSShapeParticleEditor-->PEP_pointTime_slider0.setValue( %data.times[ 0 ] );
   PEP_TSShapeParticleEditor-->PEP_pointTime_textEdit0.setText( %data.times[ 0 ] );
   
   PEP_TSShapeParticleEditor-->PEP_pointTime_slider1.setValue( %data.times[ 1 ] );
   PEP_TSShapeParticleEditor-->PEP_pointTime_textEdit1.setText( %data.times[ 1 ] );
   
   PEP_TSShapeParticleEditor-->PEP_pointTime_slider2.setValue( %data.times[ 2 ] );
   PEP_TSShapeParticleEditor-->PEP_pointTime_textEdit2.setText( %data.times[ 2 ] );
   
   PEP_TSShapeParticleEditor-->PEP_pointTime_slider3.setValue( %data.times[ 3 ] );
   PEP_TSShapeParticleEditor-->PEP_pointTime_textEdit3.setText( %data.times[ 3 ] );
   
   PEP_ParticleClassSelector.setSelected(0);
}