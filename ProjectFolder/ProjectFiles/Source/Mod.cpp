#include "GameAPI.h"

/************************************************************
	Config Variables (Set these to whatever you need. They are automatically read by the game.)
*************************************************************/

UniqueID ThisModUniqueIDs[] = { 0 }; // All the UniqueIDs this mod manages. Functions like Event_BlockPlaced are only called for blocks of IDs mentioned here. 

float TickRate = 4;							 // Set how many times per second Event_Tick() is called. 0 means the Event_Tick() function is never called.

/************************************************************* 
//	Functions (Run automatically by the game, you can put any code you want into them)
*************************************************************/

// Run every time a block is placed
void Event_BlockPlaced(CoordinateInBlocks At, UniqueID CustomBlockID, bool Moved)
{
	
}




// Run every time a block is destroyed
void Event_BlockDestroyed(CoordinateInBlocks At, UniqueID CustomBlockID, bool Moved)
{

}


/*
*	Run every time a block is hit by a tool
*	All the exact possible ToolName are: T_Stick, T_Arrow, T_Pickaxe_Stone, T_Axe_Stone, T_Shovel_Stone, T_Pickaxe_Copper, T_Axe_Copper, T_Shovel_Copper, T_Sledgehammer_Copper, T_Pickaxe_Iron, T_Axe_Iron, T_Shovel_Iron, T_Sledgehammer_Iron
*/ 
void Event_BlockHitByTool(CoordinateInBlocks At, UniqueID CustomBlockID, wString ToolName, CoordinateInCentimeters ExactHitLocation, bool ToolHeldByHandLeft)
{

	if (ToolName == L"T_Stick") {

	}
	else if (ToolName == L"T_Pickaxe_Stone") {

	}
	else if (ToolName == L"T_Arrow") {

	}
	else {

	}
}


bool timeSetEnabled = false;
bool menuSpawned = false;
bool handsToHead = false;
CoordinateInCentimeters lastTickLeftHandPos;
CoordinateInCentimeters hourSignsPos[24];
HANDLE hourSigns[24];

CoordinateInCentimeters timeSignOrigin;
CoordinateInCentimeters pauseTimeSignPos;
HANDLE pauseTimeSign;
bool fingerInTimePause = false;
bool timePaused = false;

float setTime = 0;

//Degree to radian
float angle = 7.5f * 0.01745f;
int distance = 15;

CoordinateInCentimeters CalculateRelativeCoord(CoordinateInCentimeters relativeTo, CoordinateInCentimeters relative, DirectionVectorInCentimeters headDir)
{
	CoordinateInCentimeters adjustedPos = relativeTo + CoordinateInCentimeters((headDir.X * relative.X) + (-headDir.Y * relative.Y), (headDir.Y * relative.X) + (headDir.X * relative.Y), relative.Z);
	return adjustedPos;
}

float GetDistance(CoordinateInCentimeters pos1, CoordinateInCentimeters pos2)
{
	return sqrt((pos2.X - pos1.X) * (pos2.X - pos1.X) + (pos2.Y - pos1.Y) * (pos2.Y - pos1.Y) + (pos2.Z - pos1.Z) * (pos2.Z - pos1.Z));
}

CoordinateInCentimeters GetAverageCoord(CoordinateInCentimeters pos1, CoordinateInCentimeters pos2)
{
	return CoordinateInCentimeters((pos1.X + pos2.X) / 2, (pos1.Y + pos2.Y) / 2, (pos1.Z + pos2.Z) / 2);
}

// Run X times per second, as specified in the TickRate variable at the top
void Event_Tick()
{
	//Align with hand relative to headdir
	DirectionVectorInCentimeters headDir = GetPlayerViewDirection();
	CoordinateInCentimeters headPos = GetPlayerLocationHead();
	CoordinateInCentimeters leftHandPos = GetHandLocation(true);
	CoordinateInCentimeters leftIndexPos = GetIndexFingerTipLocation(true);
	CoordinateInCentimeters rightHandPos = GetHandLocation(false);
	CoordinateInCentimeters rightIndexPos = GetIndexFingerTipLocation(false);

	if (abs(GetDistance(headPos, leftHandPos)) < 15 && abs(GetDistance(headPos, rightHandPos)) < 15)
	{
		if (!handsToHead)
		{
			PlayHapticFeedbackOnHand(true, 0.1f, 20, 0.25f);
			if (!timeSetEnabled)
				timeSetEnabled = true;
			else
			{
				timeSetEnabled = false;
				menuSpawned = false;
				for (int i = 0; i < 24; i++)
				{
					DestroyHintText(hourSigns[i]);
				}
				DestroyHintText(pauseTimeSign);
			}
			handsToHead = true;
		}
	}
	else
	{
		handsToHead = false;
	}

	if (timeSetEnabled)
	{
		if (!menuSpawned)
		{
			for (int i = 0; i < 24; i++)
			{
				float spread = 5;
				hourSignsPos[i] = CalculateRelativeCoord(headPos + headDir * 45, CoordinateInCentimeters(0, (i-12) * spread, sin(angle * i) * distance * 1.25f), headDir);
				hourSigns[i] = SpawnHintTextAdvanced(hourSignsPos[i], std::to_wstring(i), -1, 0.125f, 1, 3);
			}

			timeSignOrigin = headPos + headDir * 45;

			pauseTimeSignPos = CalculateRelativeCoord(headPos + headDir * 45, CoordinateInCentimeters(0, 0, 0), headDir);
			if (timePaused)
				pauseTimeSign = SpawnHintTextAdvanced(pauseTimeSignPos, L"Time Paused", -1, 0.5f, 1, 1.5f);
			else
				pauseTimeSign = SpawnHintTextAdvanced(pauseTimeSignPos, L"Time Flowing", -1, 0.5f, 1, 1.5f);

			menuSpawned = true;
		}
		else if(GetDistance(timeSignOrigin, headPos) > 300)
		{
			timeSetEnabled = false;
			menuSpawned = false;
			for (int i = 0; i < 24; i++)
			{
				DestroyHintText(hourSigns[i]);
			}
			DestroyHintText(pauseTimeSign);
		}

		for (int i = 0; i < 24; i++)
		{
			if (abs(GetDistance(rightIndexPos, hourSignsPos[i])) < 1.75f)
			{
				setTime = i * 100;
				SetTimeOfDay(setTime);
				break;
			}
		}

		if (abs(GetDistance(rightIndexPos, pauseTimeSignPos)) < 4)
		{
			if (!fingerInTimePause)
			{
				timePaused = !timePaused;
				DestroyHintText(pauseTimeSign);
				if (timePaused)
				{
					pauseTimeSign = SpawnHintTextAdvanced(pauseTimeSignPos, L"Time Paused", -1, 0.5f, 1, 1.5f);
					setTime = GetTimeOfDay();
				}
				else
					pauseTimeSign = SpawnHintTextAdvanced(pauseTimeSignPos, L"Time Flowing", -1, 0.5f, 1, 1.5f);
			}
			fingerInTimePause = true;
		}
		else
		{
			fingerInTimePause = false;
		}
	}

	if (timePaused)
	{
		SetTimeOfDay(setTime);
	}
}

// Run once when the world is loaded
void Event_OnLoad(bool CreatedNewWorld)
{

}

// Run once when the world is exited
void Event_OnExit()
{
	
}

/*******************************************************

	Advanced functions

*******************************************************/


// Run every time any block is placed by the player
void Event_AnyBlockPlaced(CoordinateInBlocks At, BlockInfo Type, bool Moved)
{

}

// Run every time any block is destroyed by the player
void Event_AnyBlockDestroyed(CoordinateInBlocks At, BlockInfo Type, bool Moved)
{

}

/*
*	Run every time any block is hit by a tool
*	All the exact possible ToolName are: T_Stick, T_Arrow, T_Pickaxe_Stone, T_Axe_Stone, T_Shovel_Stone, T_Pickaxe_Copper, T_Axe_Copper, T_Shovel_Copper, T_Sledgehammer_Copper, T_Pickaxe_Iron, T_Axe_Iron, T_Shovel_Iron, T_Sledgehammer_Iron
*/
void Event_AnyBlockHitByTool(CoordinateInBlocks At, BlockInfo Type, wString ToolName, CoordinateInCentimeters ExactHitLocation, bool ToolHeldByHandLeft)
{

}


/*******************************************************

	For all the available game functions you can call, look at the GameAPI.h file

*******************************************************/