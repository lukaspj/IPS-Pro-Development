// SpellSpecific datablocks ----------------------------------------------------
datablock SpellData(Fireball1 : DefaultTargetSpell)
{
   ChannelTimesMS[1] = 1600;
   CooldownMS = 3000;
   Logo = "art/particles/fire.png";
   Cost = 20;
};
datablock BezierProjectileData(Fireball1Projectile : FireballProjectile) { };

function Fireball1::onChannelBegin(%this, %spell)
{
   // Optional: This line forces the player into an animation and
   // roots it.
   // %spell.getSource().ForceAnimation(1,"spell_1");
   %bhv1 = new AttractionBehaviour(){
      attractionrange = 10;
      Attraction_offset[0] = "0 0.3 1.5";
      Amount[0] = 2;
      AttractionMode[0] = Attract;
      attractedObjectID[0] = %spell.getSource();
   };
   %spell.baseEmitter = new GraphEmitterNode(){
      dataBlock = g_defaultNode;
      emitter = FireballChannelEmitterBASE;
      ParticleBehaviour[0] = %bhv1;
      standAloneEmitter = true;
      position = %spell.getSource().position;
      Grounded = true;
   };
}

function Fireball1::onChannelEnd(%this, %spell)
{
   // Optional: This line releases the player from any forced animation
   // %spell.getSource().ForceAnimation(0);
   %bhv1 = %spell.baseEmitter.ParticleBehaviour[0];
   %spell.baseEmitter.delete();
   %bhv1.schedule(100, "delete");
}

function Fireball1::onCast(%this, %spell)
{
   %src = %spell.getSource();
   %tgt = %spell.getTarget();
   %projectile = Fireball1Projectile;
   ThrowHomingProjectile(%src, %tgt, %projectile);
}