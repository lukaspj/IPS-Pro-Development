// SpellSpecific datablocks ----------------------------------------------------
datablock SpellData(Fireball : DefaultTargetSpell)
{
   ChannelTimesMS[1] = 1600;
   CooldownMS = 3000;
   Logo = "art/particles/fire.png";
   Cost = 20;
};
datablock BezierProjectileData(FireballProjectile : FireballProjectile) { };

function Fireball::onChannelBegin(%this, %spell)
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
   %spell.baseEmitter = new ParticleEmitterNode(){
      dataBlock = DefaultEmitterNodeData;
      emitter = FireballChannelEmitterBASE;
      position = %spell.getSource().position;
   };
   %spell.baseEmitter.schedule(32, "addParticleBehaviour", %bhv1, false);
}

function Fireball::onChannelEnd(%this, %spell)
{
   // Optional: This line releases the player from any forced animation
   // %spell.getSource().ForceAnimation(0);
   %bhv1 = %spell.baseEmitter.getEmitter().ParticleBehaviour[0];
   %spell.baseEmitter.delete();
   %bhv1.schedule(100, "delete");
}

function Fireball::onCast(%this, %spell)
{
   %src = %spell.getSource();
   %tgt = %spell.getTarget();
   %projectile = FireballProjectile;
   ThrowHomingProjectile(%src, %tgt, %projectile);
}