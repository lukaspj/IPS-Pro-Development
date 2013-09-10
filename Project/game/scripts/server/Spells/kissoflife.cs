// SpellSpecific datablocks ----------------------------------------------------
datablock SpellData(KissOfLife : DefaultTargetSpell)
{
   CooldownMS = 1500;
   Logo = "art/particles/fireball.png";
   Cost = 20;
   
   CastTimesMS[1] = 1000;
   
   InitialHealth = 50;
};

function KissOfLife::onInitializeCast(%this, %spell)
{
   if(%spell.getTarget().isEnabled())
      %spell.interrupt();
}

function KissOfLife::onPreCast(%this, %spell)
{
   //%spell.getTarget().ForceAnimation(1,"resurrect");
}

function KissOfLife::onCast(%this, %spell)
{
   %spell.getTarget().applyHeal(%this.InitialHealth);
}

function KissOfLife::onPostCast(%this, %spell)
{
   //%spell.getTarget().ForceAnimation(0);
}