// SpellSpecific datablocks ----------------------------------------------------
datablock SpellData(ManaInfusion : DefaultTargetSpell)
{
   CooldownMS = 1500;
   Logo = "art/particles/fireball.png";
   Cost = 20;
   
   HealAmount = 10;
};

function ManaInfusion::onInitializeCast(%this, %spell)
{
   if(%spell.getTarget().isEnabled())
      %spell.interrupt();
}

function ManaInfusion::onCast(%this, %spell)
{
   %spell.getTarget().setRechargeRate(%spell.getTarget().getRechargeRate() + 20);
   new DOTImpact(){
      TickCount = 1;
      TickMS = 5000;
      
      Callback = "ManaInfusionEnd";
      Target = %spell.getTarget();
   };
}

function DOTImpact::ManaInfusionEnd(%this)
{
   %this.Target.setRechargeRate(%this.Target.getRechargeRate() - 20);
}