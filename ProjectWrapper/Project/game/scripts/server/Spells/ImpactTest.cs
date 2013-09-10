// Spell specific datablocks ---------------------------------------------------
datablock SpellData(ImpactTest : DefaultSelfSpell)
{
   Logo = "art/particles/fire.png";
   Cost = 0;
};
datablock BezierProjectileData(ImpactProjectile : FireballProjectile) {};

// Spell specific callbacks ----------------------------------------------------
function ImpactTest::onCast(%this, %spell)
{
   %src = %spell.getSource();
   
   %eye = %src.getEyeVector();
   %eye = VectorNormalize(%eye);
   %pos = %src.getWorldBoxCenter();
   
   %aoe = new ConeImpact(){       
      SourceObject = %src;       
      Start = %pos;
      End = VectorAdd(%pos, VectorScale(%eye, 10));     
      Radius = 2;
      TypeMask =  $TypeMasks::StaticShapeObjectType |                    
                  $TypeMasks::StaticTSObjectType;       
      CallBack = "ConeCallback";    
   }; 
} 

// Impact callbacks---------------------------------------------------------- 
function ConeImpact::ConeCallback(%this, %src, %tgt) 
{    
   %projectile = ImpactProjectile;    
   ThrowHomingBezierProjectile(%src,%tgt,%projectile,true,"0 0 12"); 
} 