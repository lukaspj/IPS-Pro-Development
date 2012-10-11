datablock GraphEmitterNodeData(g_wavesNode : g_DefaultNode)
{
   xFunc = "cos(t)*t*0.02";
   yFunc = "sin(t)*t*0.02";
   zFunc = "0.1"; // Offset it just a bit over the ground so it is not partially hidden
};

datablock GraphEmitterNodeData(g_squareSpiralNode : g_DefaultNode)
{
   xFunc = "cos(t/50)*t*0.02";
   yFunc = "sin(t)*t*0.02";
   zFunc = "0";
};

datablock GraphEmitterNodeData(g_spiralNode : g_DefaultNode)
{
   xFunc = "cos(t/50)*t*0.02";
   yFunc = "sin(t/50)*t*0.02";
   zFunc = "0";
};

datablock GraphEmitterNodeData(g_upWavesNode : g_DefaultNode)
{
   xFunc = "cos(t)*t*0.02";
   yFunc = "sin(t)*t*0.02";
   zFunc = "t/20";
};

datablock GraphEmitterNodeData(g_upSpiralNode : g_DefaultNode)
{
   xFunc = "cos(t/50)*t*0.02";
   yFunc = "sin(t/50)*t*0.02";
   zFunc = "t/20";
};

datablock GraphEmitterNodeData(g_compSpiralNode : g_DefaultNode)
{
   xFunc = "t<500 ? cos(t/50)*t*0.02 : t<1000 ? cos(t/50)*t*0.1 : t<1500 ? cos(t/50)*t*0.02 : cos(t/50)*t*0.1";
   yFunc = "t<500 ? sin(t/50)*t*0.02 : t<1000 ? sin(t/50)*t*0.1 : t<1500 ? sin(t/50)*t*0.02 : sin(t/50)*t*0.1";
   zFunc = "t/20";
};

datablock GraphEmitterNodeData(g_downSpiralNode : g_DefaultNode)
{
   xFunc = "cos(t/50)*t*0.02";
   yFunc = "sin(t/50)*t*0.02";
   zFunc = "t/20";
   Reverse = true;
   Loop = false;
};

datablock GraphEmitterNodeData(g_backburner : g_DefaultNode)
{
   xFunc = "cos(t*c)/4";
   yFunc = "sin(t*c)/4";
   zFunc = "(1+(t^2/(v<6?0.4:(v/15))))/15000";
   xVar1c = "1";
   yVar1c = "1";
   zVar1v = "0";
   funcMax = 100;
   timeScale = 8;
   sa_ejectionOffset = 10;
};

datablock GraphEmitterData(g_burnerEmitter : g_DefaultEmitter)
{
   ejectionOffset = "1";
   particles = "backburnerParticles";
};

datablock GraphEmitterData(g_attrEmitter : g_DefaultEmitter)
{
   ejectionOffset = "1";
   particles = "attrParticles";
   ejectionPeriodMS = "20";
   blendStyle = "PREMULTALPHA";
};

datablock GraphEmitterNodeData(advSp_1 : g_DefaultNode)
{
   xFunc = "t<1 ? cos(o*1*2*_pi/5)*r : "@
            "t<2 ? cos(o*2*2*_pi/5)*r : "@
            "t<3 ? cos(o*3*2*_pi/5)*r : "@
            "t<4 ? cos(o*4*2*_pi/5)*r : "@
            "cos(o*5*2*_pi/5)*r";
   yFunc = "t<1 ? sin(o*1*2*_pi/5)*r : "@
            "t<2 ? sin(o*2*2*_pi/5)*r : "@
            "t<3 ? sin(o*3*2*_pi/5)*r : "@
            "t<4 ? sin(o*4*2*_pi/5)*r : "@
            "sin(o*5*2*_pi/5)*r";
   zFunc = "h+0.1"; // Offset it just a bit over the ground so it is not partially hidden
   Reverse = true;
   Loop = false;
   xVar1r = 2;
   yVar1r = 2;
   xVar2o = 1;
   yVar2o = 1;
   zVar1h = 0;
   funcMax = 4;
   funcMin = 0;
   timeScale = 1;
   active = true;
   Loop = true;
   sa_ejectionOffset = 1;
};
