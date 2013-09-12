// SpellSpecific datablocks ----------------------------------------------------
datablock SpellData(Shockwave : DefaultSelfSpell)
{
   CooldownMS = 1500;
   Logo = "art/particles/fireball.png";
   Cost = 20;
   
   Length = 5;
   Width = 1;
   Damage = 10;
};

function Shockwave::onCast(%this, %spell)
{
   %hWidth = %this.Width / 2;
   %Start = VectorAdd(%spell.getSource().position, "0 0" SPC %this.Width);
   %End = VectorScale(%Start, %spell.getSource().getForwardVector() * %this.Length);
   %rVec = VectorScale(%spell.getSource().getRightVector(), %hWidth);
   %End = VectorSub(VectorSub(%End, %rVec), "0 0" SPC %hWidth);
   %Start = VectorAdd(VectorAdd(%Start, %rVec), "0 0" SPC %hWidth);
   new BoxImpact(){
      sourceObject = %spell.getSource();
      Start = %Start;
      End = %End;
      TypeMask = $TypeMasks::PlayerObjectType;
      Callback = "ShockwaveHit";
      
      //Dynamic
      Damage = %this.Damage;
   };
   new DOTImpact(){
      TickCount = %this.Length;
      TickMS = 50;
      
      Callback = "ShockwaveRockEmission";
      Position = VectorAdd(%spell.getSource().position, VectorScale(%spell.getSource().getForwardVector(), 1));
      Delta = VectorScale(%spell.getSource().getForwardVector(), 0.8);
      Rotation = %spell.getSource().rotation;
   };
}

function DOTImpact::ShockwaveRockEmission(%this)
{
   %rInfo = containerRayCast(VectorAdd(%this.Position, "0 0 2"), VectorSub(%this.Position, "0 0 4"), $TypeMasks::StaticObjectType);
   %pos = %this.Position;
   if(%rInfo)
      %pos = getWords(%rInfo, 1 ,3);
   %prefab = new Prefab(){
      fileName = "art/SpellSystem/Prefabs/ShockwaveRocks.prefab";
      position = setWord(%pos, 2, %pos.z - 1.5);
      rotation = %this.Rotation;
   };
   %this.Position = VectorAdd(%pos, %this.Delta);
   //schedule(1000, 0, ShockwaveRemovePrefab, %prefab);
   //%prefab.schedule(1500, "delete");
   %tween = new Tween(){
      Duration = 0.5;
      ValueName = "$test";
      ValueTarget = %prefab.position.z + 1.5;
      EaseDirection = $Ease::Out;
      EaseType = $Ease::Circular;
   };
   %prefab.zTween = %tween;
   //%tween.Play();
}

function BoxImpact::ShockwaveHit(%this, %src, %tgt)
{
   if(%tgt != %src)
      %tgt.applyDamage(%this.Damage);
}

function ShockwaveRemovePrefab(%prefab)
{
   %prefab.zTween.delete();
   %tween = new Tween(){
      Duration = 0.5;
      target = %prefab;
      ValueName = "z";
      ValueTarget = %prefab.position.z - 1.5;
      EaseDirection = $Ease::Out;
      EaseType = $Ease::Circular;  
   };
   %tween.Play();
   //%prefab.zTween.Reverse();
   //%prefab.schedule(600, "delete");
}