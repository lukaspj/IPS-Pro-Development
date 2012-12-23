singleton TSShapeConstructor(modeldts)
{
	baseShape = "./StaticShapes/models/soldier_rigged.cached/soldier_rigged.cached.dts";
	loadLights = "0";
	unit = "0";
	upAxis = "DEFAULT";
	lodType = "TrailingNumber";
	ignoreNodeScale = "0";
	adjustCenter = "0";
	adjustFloor = "0";
	forceUpdateMaterials = "0";
};
function modeldts::onLoad(%this)
{
	%this.addSequence("./StaticShapes/sequences/PlayerAnim_Lurker_Celebrate_01.cached.dts", "Seq0", "2020", "2090");
	%this.addSequence("./StaticShapes/sequences/run.dts", "Seq1", "150", "169");
}
datablock StaticShapeData(model)
{
	category = "none";
	shapeFile = "./StaticShapes/models/soldier_rigged.cached/soldier_rigged.cached.dts";
};