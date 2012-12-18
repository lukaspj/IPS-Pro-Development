//-----------------------------------------------------------------------------
// Simple Spellcasting resource
// Copyright Lukas Jørgensen, 2012
//-----------------------------------------------------------------------------

// Instantiate the SimGroup which holds all the spells during runtime
new SimGroup( spells );

//Load all spells in the spells folder.
//This could optionally be put into a function and called manually.
for(%spellFile = findFirstFile("./spells/*.cs"); %spellFile !$= ""; %spellFile = findNextFile())
{
   exec(%spellFile);
}

//The 'spell' class is the superclass from which the spells derive.
function Spell::Cast(%this, %client, %pos, %trans, %path, %team)
{
   %obj = Parent::Cast(%pos, %trans, %path, %team, %client);
   if(%obj $= "")
      echo("No spell set!");
   if(%obj == false)
      echo("Error when casting spell!");
}
//This is another kind of 'cast' this is for AI scripts.
//If you want a spell that summons a mob ( a neutral monster ) you would call this.
//This shows one of the advantages of using a superclass to call the spell functions.
function Spell::SpawnMob(%this, %client, %pos, %trans, %path)
{
   //%this.type is a dynamic variable we set when instancing the spell.
   if(%this.type $= "summon")
   {
      %obj = Parent::Cast(%pos, %trans, %path, "mob", %client);
      if(%obj $= "")
         echo("No spell set!");
      if(%obj == false)
         echo("Error when casting spell!");
   }
   else
      echo("Tried to cast non-summon spell at SpawnMob");
}

//This Cast function first casts a ray from the center of the screen,
//Then it casts the spell from the point caught by the raycast.
function Spell::CastAtScreenCenter(%this, %client)
{
   //%this.raytype is a dynamic variable we set when instancing the spell.
   // it defines what type of result the raycast should bring.
   // A point, a normal, an object or the distance to the raycollision.
   if(%this.raytype !$= false)
   {
      %rayresult = castrayfromplayer(%client, %this.raytype);
	  %player = %client.getControlObject();
      if(%rayresult == false)
      {
         //The rayresult returned nothing, decide how you handle this error
         //Here the Spell sends the rayresult error further through the system and lets
         //The individual spells handle the error.
         %trans = %client.getControlObject().getTransform();
         echo("Out of range");
         %this.Cast(%rayresult, %trans, "", %client.getControlObject().team, %client);
      }
      if(%rayresult != false)
      {
         //The ray result returned something! If the raytype tells the raycast
         // to return a point, we combine the point with our players transform and
         // resumes the 'cast' call.
         if(%this.raytype $= "point")
         {
            %trans = %rayresult SPC getWords(%client.getControlObject().getTransform(),2);
            %this.Cast(%rayresult, %trans, "", %client.getControlObject().team, %client);
         }
      }
      
   }
   else{
      %this.Cast(null, null , null, %client.getControlObject().team, %client);
   }
}

//A combination of the SpawnMob and CastAtScreenCenter methods.
function Spell::SpawnMobAtScreenCenter(%this, %client)
{
   if(%this.type $= "summon")
   {
      if(%this.raytype !$= false)
      {
         %rayresult = castrayfromplayer(%client, %this.raytype);
         if(%rayresult == false)
            echo("Out of range");
         if(%rayresult != false)
            %this.Cast(%rayresult, %rayresult SPC getWords(%client.getControlObject().getTransform(),2) , "", "mob", %client);
      }
      else{
         %this.Cast(null, null , null, "mob", %client);
      }
   }
   else
      echo("Tried to cast non-summon spell at SpawnMobAtScreenScenter");
}

//Get the spell scriptobject from an id
function getSpellFromID(%id)
{
   return spells.getObject(%id);
}

//Get the spell scriptobject from a name
function getSpellFromName(%name)
{
   %count = spells.getCount();

	for(%i=0; %i < %count; %i++)
	{
		%spell = spells.getobject(%i);
		if (%spell.getName() $= %name)	
		 {
		    return %spell;
		 }
	}
}
