// SpellSpecific datablocks ----------------------------------------------------
datablock SpellData(RainOfFire : DefaultAOESpell)
{
   CooldownMS = 3000;
   Logo = "art/particles/fireball.png";
   Cost = 20;
   
   Radius = 3;
};
datablock BezierProjectileData(RainOfFireComet : FireballProjectile) { lifetime = 1000; };

function RainOfFire::onCast(%this, %spell)
{
   %dotimpact = new DOTImpact(){
      TickMS = 500;
      TickCount = 10;
      
      CallBack = "RainOfFireCB";
      
      // Dynamic variables
      TargetPos = %spell.getTargetPosition();
      Radius = %this.Radius;
   };
}

function DOTImpact::RainOfFireCB(%this, %src)
{
   %point1 = GetPointWithinArea(%this.TargetPos, %this.Radius, "0 0 7");
   %point2 = GetPointWithinArea(%this.TargetPos, %this.Radius, "0 0 7");
   
   %end = VectorSub(%point2, "0 0 20");
   %searchResult = containerRayCast(%point2, %end, $TypeMasks::TerrainObjectType);
   
   if(!%searchResult)
      return;
   
   %end = getWords( %searchResult, 1, 3 );
   
   %vel = VectorSub(%end, %point1);
   %nvel = VectorNormalize(%vel);
   %vel = VectorScale(%nvel, 3);
   
   %bullet = new BezierProjectile(){
      initialPosition = %point1;
      initialVelocity = %vel;
      sourceSlot = 0;
      Homing = true;
      dataBlock = RainOfFireComet;
      TargetPosition = %end;
      BezierWeights = "0 0 0";
   };
}

function GetPointWithinArea(%center, %radius, %offset)
{
   %rand = getRandom() * m2Pi();
   %relx = mCos(%rand) * %radius;
   %rely = mSin(%rand) * %radius;
   %pos = VectorAdd(%center, %relx SPC %rely SPC "0");
   %pos = VectorAdd(%pos, %offset);
   return %pos;
}