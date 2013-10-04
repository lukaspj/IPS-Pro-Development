function PEP_BillboardRibbonParticleEditor::guiSync( %this, %new )
{   
   %data = PE_ParticleEditor.currParticle;
   
   %bitmap = MaterialEditorGui.searchForTexture( %data.getName(), %data.textureName );
   if( %bitmap !$= "" )
   {
      PEP_BillboardRibbonParticleEditor-->PEP_previewImage.setBitmap( %bitmap );
      PEP_BillboardRibbonParticleEditor-->PEP_previewImageName.setText( %bitmap );
      PEP_BillboardRibbonParticleEditor-->PEP_previewImageName.tooltip = %bitmap;
   }
   else
   {
      PEP_BillboardRibbonParticleEditor-->PEP_previewImage.setBitmap( "" );
      PEP_BillboardRibbonParticleEditor-->PEP_previewImageName.setText( "None" );
      PEP_BillboardRibbonParticleEditor-->PEP_previewImageName.tooltip = "None";
   }
   
   PEP_BillboardRibbonParticleEditor-->PEP_inverseAlpha.setValue( %data.useInvAlpha );
   
   PEP_BillboardRibbonParticleEditor-->PEP_lifetimeMS_slider.setValue( %data.lifetimeMS );
   PEP_BillboardRibbonParticleEditor-->PEP_lifetimeMS_textEdit.setText( %data.lifetimeMS );
   
   PEP_BillboardRibbonParticleEditor-->PEP_lifetimeVarianceMS_slider.setValue( %data.lifetimeVarianceMS );
   PEP_BillboardRibbonParticleEditor-->PEP_lifetimeVarianceMS_textEdit.setText( %data.lifetimeVarianceMS );
   
   PEP_BillboardRibbonParticleEditor-->PEP_inheritedVelFactor_slider.setValue( %data.inheritedVelFactor );
   PEP_BillboardRibbonParticleEditor-->PEP_inheritedVelFactor_textEdit.setText( %data.inheritedVelFactor );
   
   PEP_BillboardRibbonParticleEditor-->PEP_constantAcceleration_slider.setValue( %data.constantAcceleration );
   PEP_BillboardRibbonParticleEditor-->PEP_constantAcceleration_textEdit.setText( %data.constantAcceleration );
   
   PEP_BillboardRibbonParticleEditor-->PEP_gravityCoefficient_slider.setValue( %data.gravityCoefficient );
   PEP_BillboardRibbonParticleEditor-->PEP_gravityCoefficient_textEdit.setText( %data.gravityCoefficient );
   
   PEP_BillboardRibbonParticleEditor-->PEP_dragCoefficient_slider.setValue( %data.dragCoefficient );
   PEP_BillboardRibbonParticleEditor-->PEP_dragCoefficient_textEdit.setText( %data.dragCoefficient );
   
   PEP_BillboardRibbonParticleEditor-->PEP_spinRandomMin_slider.setValue( %data.spinRandomMin );
   PEP_BillboardRibbonParticleEditor-->PEP_spinRandomMin_textEdit.setText( %data.spinRandomMin );
   
   PEP_BillboardRibbonParticleEditor-->PEP_spinRandomMax_slider.setValue( %data.spinRandomMax );
   PEP_BillboardRibbonParticleEditor-->PEP_spinRandomMax_textEdit.setText( %data.spinRandomMax  );
   
   PEP_BillboardRibbonParticleEditor-->PEP_spinRandomMax_slider.setValue( %data.spinRandomMax );
   PEP_BillboardRibbonParticleEditor-->PEP_spinRandomMax_textEdit.setText( %data.spinRandomMax  );
   
   PEP_BillboardRibbonParticleEditor-->PEP_spinSpeed_slider.setValue( %data.spinSpeed );
   PEP_BillboardRibbonParticleEditor-->PEP_spinSpeed_textEdit.setText( %data.spinSpeed );
   
   PEP_BillboardRibbonParticleEditor-->PE_ColorTintSwatch0.color = %data.colors[ 0 ];
   PEP_BillboardRibbonParticleEditor-->PE_ColorTintSwatch1.color = %data.colors[ 1 ];
   PEP_BillboardRibbonParticleEditor-->PE_ColorTintSwatch2.color = %data.colors[ 2 ];
   PEP_BillboardRibbonParticleEditor-->PE_ColorTintSwatch3.color = %data.colors[ 3 ];
   
   PEP_BillboardRibbonParticleEditor-->PEP_pointSize_slider0.setValue( %data.sizes[ 0 ] );
   PEP_BillboardRibbonParticleEditor-->PEP_pointSize_textEdit0.setText( %data.sizes[ 0 ] );
   
   PEP_BillboardRibbonParticleEditor-->PEP_pointSize_slider1.setValue( %data.sizes[ 1 ] );
   PEP_BillboardRibbonParticleEditor-->PEP_pointSize_textEdit1.setText( %data.sizes[ 1 ] );
   
   PEP_BillboardRibbonParticleEditor-->PEP_pointSize_slider2.setValue( %data.sizes[ 2 ] );
   PEP_BillboardRibbonParticleEditor-->PEP_pointSize_textEdit2.setText( %data.sizes[ 2 ] );
   
   PEP_BillboardRibbonParticleEditor-->PEP_pointSize_slider3.setValue( %data.sizes[ 3 ] );
   PEP_BillboardRibbonParticleEditor-->PEP_pointSize_textEdit3.setText( %data.sizes[ 3 ] );
   
   PEP_BillboardRibbonParticleEditor-->PEP_pointTime_slider0.setValue( %data.times[ 0 ] );
   PEP_BillboardRibbonParticleEditor-->PEP_pointTime_textEdit0.setText( %data.times[ 0 ] );
   
   PEP_BillboardRibbonParticleEditor-->PEP_pointTime_slider1.setValue( %data.times[ 1 ] );
   PEP_BillboardRibbonParticleEditor-->PEP_pointTime_textEdit1.setText( %data.times[ 1 ] );
   
   PEP_BillboardRibbonParticleEditor-->PEP_pointTime_slider2.setValue( %data.times[ 2 ] );
   PEP_BillboardRibbonParticleEditor-->PEP_pointTime_textEdit2.setText( %data.times[ 2 ] );
   
   PEP_BillboardRibbonParticleEditor-->PEP_pointTime_slider3.setValue( %data.times[ 3 ] );
   PEP_BillboardRibbonParticleEditor-->PEP_pointTime_textEdit3.setText( %data.times[ 3 ] );
   
   PEP_ParticleClassSelector.setSelected(0);
}