//-----------------------------------------------------------------------------
// Simple Spellcasting resource
// Copyright Lukas Jørgensen, 2012
//-----------------------------------------------------------------------------

// Some serverside raycast functions, simply casts a ray from the object to 
//  whereever the object is looking and returns the specified information.
// One takes a client the other takes an object.
function castrayfromplayer(%client, %return)
{  
   %player = %client.getControlObject();    
   %start = %player.getEyePoint();
   %eyeVector = %player.getEyeVector();  
   
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
   $TypeMasks::StaticTSObjectType , %player);
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

function castrayfromNPC(%obj, %return)
{  
   %start = %obj.getEyePoint();
   %eyeVector = %obj.getEyeVector();  
      
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
   $TypeMasks::StaticTSObjectType, %obj);
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