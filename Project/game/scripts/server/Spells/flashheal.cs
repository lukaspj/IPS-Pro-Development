// SpellSpecific datablocks ----------------------------------------------------
datablock SpellData(FlashHeal : DefaultTargetSpell)
{
   CooldownMS = 1500;
   Logo = "art/particles/fireball.png";
   Cost = 20;
   
   HealAmount = 10;
};

function FlashHeal::onInitializeCast(%this, %spell)
{
   if(%spell.getTarget().isDisabled())
      %this.interrupt();
}

function FlashHeal::onCast(%this, %spell)
{
   %spell.getTarget().applyHeal(%this.HealAmount);
}