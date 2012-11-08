datablock GraphEmitterNodeData(g_DefaultNode)
{
   timeMultiple = 1;
   
   funcMax = 2000;
   funcMin = 0;
   timeScale = 1;
   ProgressMode = 0;
   
   sa_ejectionPeriodMS = "1";
   sa_periodVarianceMS = "0";
   sa_ejectionVelocity = "0";
   sa_velocityVariance = "0";
   sa_ejectionOffset = "0.08";
   sa_thetaMax = "0";
   sa_thetaMin = "0";
   sa_phiReferenceVel = "0";
   sa_phiVariance = "0";
   particles = "DefaultParticle";
   blendStyle = "ADDITIVE";
   softnessDistance = "0.01";
};

datablock MeshEmitterData(m_DefaultEmitter)
{
   ejectionPeriodMS = "1";
   periodVarianceMS = "0";
   ejectionVelocity = "0";
   velocityVariance = "0";
   ejectionOffset = "1";
   thetaMax = "0";
   thetaMin = "0";
   phiReferenceVel = "0";
   phiVariance = "0";
   particles = "DefaultParticle";
   blendStyle = "ADDITIVE";
   softnessDistance = "0.01";
};

datablock RadiusMeshEmitterData(rm_DefaultEmitter)
{
   ejectionPeriodMS = "1";
   periodVarianceMS = "0";
   ejectionVelocity = "0";
   velocityVariance = "0";
   ejectionOffset = "1";
   thetaMax = "0";
   thetaMin = "0";
   phiReferenceVel = "0";
   phiVariance = "0";
   particles = "DefaultParticle";
   blendStyle = "ADDITIVE";
   softnessDistance = "0.01";
};

datablock GroundEmitterNodeData(gr_DefaultNode)
{
   timeMultiple = 1;
   
   ejectionPeriodMS = "1";
   periodVarianceMS = "0";
   ejectionVelocity = "0";
   velocityVariance = "0";
   ejectionOffset = "1";
   thetaMax = "0";
   thetaMin = "0";
   blendStyle = "ADDITIVE";
   softnessDistance = "0.01";
};

datablock GroundEmitterData(gr_DefaultEmitter)
{
   ejectionPeriodMS = "1";
   periodVarianceMS = "0";
   ejectionVelocity = "0";
   velocityVariance = "0";
   ejectionOffset = "0";
   thetaMax = "0";
   thetaMin = "0";
   particles = "DefaultParticle";
   blendStyle = "ADDITIVE";
   softParticles = "0";
   softnessDistance = "0.1";
   radius = "1";
};

datablock GraphEmitterData(g_DefaultEmitter)
{
   ejectionPeriodMS = "1";
   periodVarianceMS = "0";
   ejectionVelocity = "0";
   velocityVariance = "0";
   ejectionOffset = "1";
   thetaMax = "0";
   thetaMin = "0";
   phiReferenceVel = "0";
   phiVariance = "0";
   particles = "DefaultParticle";
   blendStyle = "ADDITIVE";
   softParticles = "0";
   softnessDistance = "0.1";
};

datablock PixelMask(pMask1)
{
	MaskPath = "./IPS.png";
	Treshold = 125;
};

datablock MaskEmitterData(msk_DefaultEmitter)
{
   ejectionPeriodMS = "500";
   periodVarianceMS = "0";
   ejectionVelocity = "0";
   velocityVariance = "0";
   ejectionOffset = "1";
   thetaMax = "0";
   thetaMin = "0";
   phiReferenceVel = "0";
   phiVariance = "0";
   particles = "DefaultParticle";
   blendStyle = "ADDITIVE";
   softParticles = "0";
   softnessDistance = "0.1";
   PixelMask = "pMask1";
};

datablock MaskEmitterNodeData(msk_DefaultNode)
{
   timeMultiple = 1;
   
   ejectionPeriodMS = "1";
   periodVarianceMS = "0";
   ejectionVelocity = "0";
   velocityVariance = "0";
   ejectionOffset = "1";
   thetaMax = "0";
   thetaMin = "0";
   phiReferenceVel = "0";
   phiVariance = "0";
   blendStyle = "ADDITIVE";
   softnessDistance = "0.01";
};

datablock ParticleEmitterData(SampleEmitter)
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
};

//---------------GRAPH EMITTER SAMPLES---------------------

datablock GraphEmitterData(g_sampleEmitter : g_DefaultEmitter)
{
   ejectionOffset = "0.08";
   softnessDistance = "0.01";
};


datablock ParticleEmitterNodeData( pEffecTest : broadNode )
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