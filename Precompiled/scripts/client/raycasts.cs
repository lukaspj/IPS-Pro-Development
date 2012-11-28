//-----------------------------------------------------------------------------
// Simple Spellcasting resource
// Copyright Lukas Jørgensen, 2012
//-----------------------------------------------------------------------------

// This is the clientside raycasts.
// These are very similar to the serverside except for few differences.

// Notice the additional parameter in the ContainerRayCast.
// The last parameter is a boolean specifying whether to raycast on the client
//  or on the server. If it isn't set to true, raycasting as often as we do
//  will create a lot of server traffic

function castClientRayfromClient(%client, %return)
{   
   %player = %client.player;    
   %start = %client.player.getEyePoint();
   %eyeVector = %client.player.getEyeVector();  
   
   %start = getWords(%start, 0, 2);
   
   %rayLength = 100000;  
   %rayDirection = VectorScale(%eyeVector, %rayLength);
   %end = VectorAdd(%start, %rayDirection);
    %rayInfo = ContainerRayCast(%start, %end,   $TypeMasks::StaticObjectType | 
   $TypeMasks::InteriorObjectType | 
   $TypeMask::WaterObjectType | 
   $TypeMasks::ShapeBaseObjectType | 
   $TypeMasks::StaticShapeObjectType | 
   $TypeMasks::ItemObjectType | 
   $TypeMasks::TerrainObjectType |
   $TypeMasks::StaticTSObjectType , %player, true);
  if (getWord(%rayInfo, 1) != 0)  
    {  
    %object = getWord(%rayInfo, 0);  
        %point  = getWords(%rayInfo, 1, 3);  
        %normal = getWords(%rayInfo, 4, 6);  
        %distance = %end + %start;
         switch$(%return)
         {
            case "object":
               return %object;
            case "distance":
               return %distance;
            case "point":
               return %point;
            case "normal":
               return %normal;
         }
    }
     else
     {
     return false;     
     }
}

// If %collision is set to all, the raycast will collide with anything.
// If %collision is set to statics it will collide with any statics
//  including terrain.
// Lastly, if the %typemasks haven't been said, we will try to parse %collision
//  as a typemask.
function castClientRayFromOBJ(%obj, %return, %collision)
{  
   %start = %obj.getEyePoint();
   %eyeVector = %obj.getEyeVector();  
      
   %start = getWords(%start, 0, 2);      
      
   %rayLength = 100000;  
   %rayDirection = VectorScale(%eyeVector, %rayLength);
   %end = VectorAdd(%start, %rayDirection);

   if(%collision $= "all" || %collision $= true)
   {
      %typemasks = $TypeMasks::StaticObjectType | 
                  $TypeMasks::InteriorObjectType | 
                  $TypeMask::WaterObjectType | 
                  $TypeMasks::ShapeBaseObjectType | 
                  $TypeMasks::StaticShapeObjectType | 
                  $TypeMasks::ItemObjectType | 
                  $TypeMasks::TerrainObjectType |
                  $TypeMasks::StaticTSObjectType;
   }
   if(%collision $= "statics")
   {
      %typemasks = $TypeMasks::StaticObjectType | 
                  $TypeMasks::InteriorObjectType | 
                  $TypeMask::WaterObjectType |  
                  $TypeMasks::StaticShapeObjectType | 
                  $TypeMasks::TerrainObjectType |
                  $TypeMasks::StaticTSObjectType;
   }
   if(%typemasks $= "")
   {
      %typemasks = %collision;
   }

   %rayInfo = ContainerRayCast(%start, %end, %typemasks, %obj, true);
  if (getWord(%rayInfo, 1) != 0)  
    {  
    %object = getWord(%rayInfo, 0);  
        %point  = getWords(%rayInfo, 1, 3);  
        %normal = getWords(%rayInfo, 4, 6);  
        %distance = %end + %start;
         switch$(%return)
         {
            case "object":
               return %object;
            case "distance":
               return %distance;
            case "point":
               return %point;
            case "normal":
               return %normal;
         }
    }
     else
     {
     return false;     
     }
}