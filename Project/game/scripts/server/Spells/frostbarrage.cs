// SpellSpecific datablocks ----------------------------------------------------
datablock SpellData(FrostBarrage : DefaultTargetSpell)
{
   CooldownMS = 3000;
   Logo = "art/particles/fire.png";
   Cost = 20;
   
   Radius = 3;
};

function FrostBarrage::onChannel(%this, %spell)
{
   %dotimpact = new DOTImpact(){
      TickMS = 100;
      TickCount = 15;
      
      CallBack = "FrostBarrageCB";
      sourceObject = %spell.getSource();
      
      // Dynamic variables
      Target = %spell.getTarget();
   };
}

function DOTImpact::FrostBarrageCB(%this, %src)
{
   %weights = GetRandomVector(-3, 3, -3, 3, 0, 3);
   ThrowHomingBezierProjectile(%src, %this.Target, FrostShardProjectile, true, %weights);
}

function GetRandomVector(%xfrom, %xto, %yfrom, %yto, %zfrom, %zto)
{
   return getRandom(%xfrom, %xto) SPC getRandom(%yfrom, %yto) SPC getRandom(%zfrom, %zto);
}