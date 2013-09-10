// SpellSpecific datablocks ----------------------------------------------------
datablock SpellData(Blink : DefaultSelfSpell)
{
   CooldownMS = 500;
   Logo = "art/particles/millsplash01.png";
   Cost = 20;
   
   testDensity = 5;
   range = 20;
   stepHeight = 0.7;
   slopeMin = -38;
   slopeMax = 38;
   stepBackCount = 8;
   debugRendering = true;
};

function Blink::onCast(%this, %spell)
{
   %start = %spell.getSource().position;
   %explosion = new Explosion(){
      dataBlock = EntranceEffect;
      position = %start;
   };
   %rotation = %spell.getSource().getForwardVector();
   %end = VectorAdd(%start, VectorScale(%rotation, %this.range));
   %vectorDelta = VectorScale(%rotation, 1/%this.testDensity);
   %curpos = %start;
   %resultArray = new ArrayObject();
   for(%i = 0; %i < %this.testDensity * %this.range; %i++)
   {
      %nextpos = VectorAdd(%curpos, %vectorDelta);
      %result = BlinktestPoint(%curpos, %nextpos, %this.stepHeight, %this.slopeMin, %this.slopeMax, %spell.getSource(), %this.debugRendering);
      if(%result $= "0")
         break;
      %resultArray.push_front(%result);
      if(%i >= %this.stepBackCount)
         %resultArray.pop_back();
      %curpos = VectorAdd(%result, "0 0 0.05");
   }
   if(%resultArray.count() == 0)
      return;
   %curpos = %resultArray.getKey(%resultArray.count() - 1);
   %resultArray.delete();
   new Explosion(){
      dataBlock = EntranceEffect;
      position = %curpos;   
   };
   %spell.getSource().position = %curpos;
}

function BlinktestPoint(%cur, %next, %stepHeight, %slopeMin, %slopeMax, %excempt, %debugRendering)
{
   %bottomsuccess = false;
   %topsuccess = false;
   %frontsuccess = false;
   %bottomresult = "";
   %result = containerRayCast(%cur, %next, $TypeMasks::ShapeBaseObjectType |
                                           $TypeMasks::StaticShapeObjectType,
                                             %excempt);
   if(!%result)      
      %frontsuccess = true;
   else
    {
       %fpos = getWords(%result, 1, 3);
       %angle = mRadToDeg(VectorAngleBetween("1 0 0", "0 0 1", getWords(%result, 4, 6)));
       if(%angle >= %slopeMin && %angle <= %slopeMax)
         %frontsuccess = true;
      if(%debugRendering)      
         DrawTimedDebugVector(%fpos, VectorAdd(%fpos, getWords(%result, 4, 6)), 5000, "125 125 0");
    }
    if(%debugRendering)
      DrawTimedDebugVector(%cur, %next, 5000, "0 255 0");
   //-----------------------BOTTOM-----------------------  
   %result = containerRayCast(%next, VectorSub(%next, "0 0" SPC %stepHeight),
                                    $TypeMasks::ShapeBaseObjectType |
                                    $TypeMasks::StaticShapeObjectType |
                                    $TypeMasks::TerrainObjectType, 
                                    %excempt);
   if(!%result)
      %bottomsuccess = false;
   else{
      %bottomsuccess = true;
      %bottomresult = %result;
      //DrawTimedDebugVector(%next, getWords(%bottomresult, 1, 3), 5000, "255 0 0");
   }
   ////-----------------------TOP-----------------------
   %result = containerRayCast(VectorAdd(%next, "0 0" SPC %stepHeight), %next,
                                    $TypeMasks::ShapeBaseObjectType |
                                    $TypeMasks::StaticShapeObjectType |
                                    $TypeMasks::TerrainObjectType, 
                                    %excempt);
   if(!%result)
      %topsuccess = false;
   else{
      %topsuccess = true;
      %topresult = %result;
      %resultpoint = getWords(%result, 1, 3);
      %zvalue = getWord(%resultpoint, 2);
      %zmodcur = setWord(%cur, 2, %zvalue);
      %newresult = containerRayCast(%zmodcur, %resultpoint, $TypeMasks::ShapeBaseObjectType |
                                                            $TypeMasks::StaticShapeObjectType,
                                                            %excempt);
      if(%debugRendering)
         DrawTimedDebugVector(%zmodcur, %resultpoint, 5000, "255 255 255");
      %frontsuccess = false;
      if(!%newresult)
         %frontsuccess = true;
      else
      {
         %fpos = getWords(%newresult, 1, 3);
         %angle = mRadToDeg(VectorAngleBetween("1 0 0", "0 0 1", getWords(%newresult, 4, 6)));
         if(%angle >= %slopeMin && %angle <= %slopeMax)
            %frontsuccess = true;
         if(%debugRendering)
            DrawTimedDebugVector(%fpos, VectorAdd(%fpos, getWords(%newresult, 4, 6)), 5000, "125 125 0");
      }
      //if(%frontsuccess)   
         //DrawTimedDebugVector(%next, getWords(%topresult, 1, 3), 5000, "0 0 255");
   }
   if(!%frontsuccess)
      return false;
   if(%bottomsuccess && %topsuccess)
   {
      if(%debugRendering)
         DrawTimedDebugVector(%next, getWords(%topresult, 1, 3), 5000, "0 0 255");
      return getWords(%topresult, 1, 3);
   }
   else if(%bottomsuccess)
   {
      if(%debugRendering)
         DrawTimedDebugVector(%next, getWords(%bottomresult, 1, 3), 5000, "0 0 255");
      return getWords(%bottomresult, 1, 3);
   }
   else if(%topsuccess)
   {
      if(%debugRendering)
         DrawTimedDebugVector(%next, getWords(%topresult, 1, 3), 5000, "0 0 255");
      return getWords(%topresult, 1, 3);
   }
   else
      return false;
}

function VectorAngleBetween(%N, %V1, %V2)
{
   return mAtan( VectorDot(%N, VectorCross(%V1, %V2)),
                 VectorDot(%V1, %V2));
}