// SpellSpecific datablocks ----------------------------------------------------
datablock SpellData(Daze : DefaultTargetSpell)
{
   CooldownMS = 3000;
   Logo = "art/particles/fire.png";
   Cost = 20;
   Duration = 3000;
};

function Daze::onCast(%this, %spell)
{
   %spell.getTarget().applySlow(60);
   new DOTImpact() {
      TickCount = 1;
      TickMS = %this.Duration;
      
      Callback = "DazeCallback";
      sourceObject = %spell.getSource();
      
      // Dynamic variables
      Target = %spell.getTarget();
   };
}

function DOTImpact::DazeCallback(%this, %src)
{
   %this.Target.removeSlow(60);
}