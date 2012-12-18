//-----------Behaviours-----------
//-----------Uniform behaviours-----------
datablock CollisionBehaviour(col_BHV){};
datablock StickyBehaviour(sticky_BHV){};
//-----------Variable behaviours-----------
datablock AttractionBehaviour(spawn_attr_BHV){
   attractedObjectID[0] = "spawn";
   Amount[0] = 0.2;
   AttractionMode[0] = "Attract";
};

//-----------GraphEmitters-----------
datablock GraphEmitterNodeData(g_DefaultNode)
{
   timeMultiple = 1;
   
   funcMax = 2000;
   funcMin = 0;
   timeScale = 1;
   ProgressMode = 0;
};

datablock GraphEmitterData(g_DefaultEmitter)
{
   ejectionPeriodMS = "1";
   periodVarianceMS = "0";
   ejectionVelocity = "0";
   velocityVariance = "0";
   ejectionOffset = "1";
   particles = "DefaultParticle";
   blendStyle = "ADDITIVE";
   softParticles = "0";
   softnessDistance = "0.1";
};

//-----------MeshEmitters-----------
datablock MeshEmitterData(m_DefaultEmitter)
{
   ejectionPeriodMS = "1";
   periodVarianceMS = "0";
   ejectionVelocity = "0";
   velocityVariance = "0";
   ejectionOffset = "1";
   particles = "DefaultParticle";
   blendStyle = "ADDITIVE";
   softnessDistance = "0.01";
};

//-----------RadiusMeshEmitters-----------
datablock RadiusMeshEmitterData(rm_DefaultEmitter)
{
   ejectionPeriodMS = "1";
   periodVarianceMS = "0";
   ejectionVelocity = "0";
   velocityVariance = "0";
   ejectionOffset = "1";
   particles = "DefaultParticle";
   blendStyle = "ADDITIVE";
   softnessDistance = "0.01";
};

//-----------GroundEmitters-----------
datablock GroundEmitterNodeData(gr_DefaultNode)
{
   timeMultiple = 1;
};

datablock GroundEmitterData(gr_DefaultEmitter)
{
   ejectionPeriodMS = "1";
   periodVarianceMS = "0";
   ejectionVelocity = "0";
   velocityVariance = "0";
   ejectionOffset = "0";
   particles = "DefaultParticle";
   blendStyle = "ADDITIVE";
   softParticles = "0";
   softnessDistance = "0.1";
   radius = "1";
};

//-----------PixelMasks-----------
datablock PixelMask(DefaultMask)
{
	MaskPath = "./IPS.png";
};

datablock PixelMask(TresholdMask)
{
	MaskPath = "./tresholdtest.png";
};

//-----------MaskEmitters-----------
datablock MaskEmitterNodeData(msk_DefaultNode)
{
   timeMultiple = 1;
};

datablock MaskEmitterData(msk_DefaultEmitter)
{
   ejectionPeriodMS = "2";
   periodVarianceMS = "0";
   ejectionVelocity = "0";
   velocityVariance = "0";
   ejectionOffset = "1";
   particles = "DefaultParticle";
   blendStyle = "ADDITIVE";
   softParticles = "0";
   softnessDistance = "0.1";
   PixelMask = "DefaultMask";
   radius = 3;
   Treshold_min = 0;
   Treshold_max = 255;
};

datablock MaskEmitterData(msk_TresholdEmitter)
{
   ejectionPeriodMS = "2";
   periodVarianceMS = "0";
   ejectionVelocity = "0";
   velocityVariance = "0";
   ejectionOffset = "1";
   particles = "DefaultParticle";
   blendStyle = "ADDITIVE";
   softParticles = "0";
   softnessDistance = "0.1";
   PixelMask = "TresholdMask";
   radius = 3;
   Treshold_min = 0;
   Treshold_max = 0;
};

//-----------SphereEmitters-----------
datablock SphereEmitterNodeData( DefaultNode )
{   
	timeMultiple = 1;
   ejectionPeriodMS = "2";
   periodVarianceMS = "0";
   ejectionVelocity = "0";
   velocityVariance = "0";
   sa_ejectionPeriodMS = "2";
   sa_periodVarianceMS = "0";
   sa_ejectionVelocity = "0";
   sa_velocityVariance = "0";
};

//-----------ParticleEffects-----------
//Childs--------------------
//-----SphereEmitters-----
datablock SphereEmitterData(SampleEmitter)
{
   ejectionPeriodMS = "50";
   ejectionVelocity = "0";
   velocityVariance = "0";
   ejectionOffset = "0.2";
   thetaMax = "40";
   particles = "DefaultParticle";
   blendStyle = "ADDITIVE";
   softParticles = "0";
   softnessDistance = "1";
   ParticleBehaviour[0] = stick_BHV;
};

//---------------GRAPH EMITTER SAMPLES---------------------

datablock GraphEmitterData(g_sampleEmitter : g_DefaultEmitter)
{
   ejectionOffset = "0.08";
   softnessDistance = "0.01";
};


datablock SphereEmitterNodeData( pEffecTest )
{   
   sa_ejectionVelocity = 0;
   sa_ejectionPeriodMS = "50";
   standAloneEmitter = 1;
   sa_periodVarianceMS = "0";
   sa_velocityVariance = "0";
   sa_ejectionOffset = "0";
   sa_thetaMin         = "0";
   sa_thetaMax         = "60";
   sa_phiReferenceVel = "0";
   sa_phiVariance = "360";
};

//Effects--------------------
datablock ParticleEffectData(DefaultEffect)
{
   pEffect = "./testEffect.pEffect";
   lifeTimeMS = 20000;
};

datablock ParticleEffectData(DefaultEffect2)
{
   pEffect = "./testEffect_2.pEffect";
   lifeTimeMS = 20000;
};

datablock ParticleEffectData(TresholdEffect)
{
   pEffect = "./tresholdtest.pEffect";
   lifeTimeMS = 20000;
};
