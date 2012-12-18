datablock SpellData(FireballSpell)
{
   CastType = ScreenCenterCastType;
   ChannelAnimation = "ChannelBolt";
   ChannelDuration = 2000; // MS
   FireDuration = 200; // MS
   TransmissionDuration = 2000; // MS
   AftermathTickMS = 100; // 100 ms between ticks
   AftermathTickCount = 10; // 
   impact[0] = new DamageImpact;
   impact[1] = new AOEImpact;
};

function CastAFireball()
{
   SpellSystem::BeginCast(FireballSpell);
}

function FireballSpell::onChannel(%this, %player){
  %this.PlayAnimation(SpellData.ChannelAnimation);
}

function FireballSpell::onFire(%this){
   %projectile = new AdvancedProjectile(){
      // Set up the projectile
      Spell = %this;
   };
}

function FireballSpell::onHit(%this, %projectile){
   %this.impact[0].doImpact();
}

function FireballSpell::AftermathTick(%this, %pos){
   %this.impact[1].doImpact();
}