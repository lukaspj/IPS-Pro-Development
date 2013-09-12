datablock SpellData(Haste : DefaultSelfSpell)
{
   CooldownMS = 2000;   
   Logo = "art/particles/fire.png";
   Cost = 10;
   Duration = 2000;
};

function Haste::onCast(%this, %spell)
{
   %spell.getTarget().applyHaste(60);
   new DOTImpact() {
      TickCount = 1;
      TickMS = %this.Duration;
      
      Callback = "HasteCallback";
      sourceObject = %spell.getSource();
      
      // Dynamic variables
      Target = %spell.getTarget();
   };
}

function DOTImpact::HasteCallback(%this, %src)
{
   %this.Target.removeHaste(60);
}