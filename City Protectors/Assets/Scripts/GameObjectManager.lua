--========================================================================
-- GameObjectManager.lua : Defines the GameObjectManager class
--
-- Part of the GameCode4 Application
--
-- GameCode4 is the sample application that encapsulates much of the source code
-- discussed in "Game Coding Complete - 4th Edition" by Mike McShaffry and David
-- "Rez" Graham, published by Charles River Media. 
-- ISBN-10: 1133776574 | ISBN-13: 978-1133776574
--
-- If this source code has found it's way to you, and you think it has helped you
-- in any way, do the authors a favor and buy a new copy of the book - there are 
-- detailed explanations in it that compliment this code well. Buy a copy at Amazon.com
-- by clicking here: 
--    http://www.amazon.com/gp/product/1133776574/ref=olp_product_details?ie=UTF8&me=&seller=
--
-- There's a companion web site at http://www.mcshaffry.com/GameCode/
-- 
-- The source code is managed and maintained through Google Code: 
--    http://code.google.com/p/gamecode4/
--
-- (c) Copyright 2012 Michael L. McShaffry and David Graham
--
-- This program is free software; you can redistribute it and/or
-- modify it under the terms of the GNU Lesser GPL v3
-- as published by the Free Software Foundation.
--
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See 
-- http://www.gnu.org/licenses/lgpl-3.0.txt for more details.
--
-- You should have received a copy of the GNU Lesser GPL v3
-- along with this program; if not, write to the Free Software
-- Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
--
--========================================================================

require("scripts\\TeapotAi.lua");

-- brains
require("scripts\\HardCodedBrain.lua");
require("scripts\\DecisionTreeBrain.lua");
require("scripts\\DebugBrain.lua");

-- [rez] Set this constant to change brains for teapots.  Note that there's no visual difference between 
-- the hard-coded brain and the decision tree brain, they just have different implementations under the 
-- covers.  This is a good example of how to keep the AI decision making nice and decoupled from the rest 
-- of your engine.
TEAPOT_BRAIN = DecisionTreeBrain;
--TEAPOT_BRAIN = HardCodedBrain;

-----------------------------------------------------------------------------------------------------------------------
-- Internal Processes that operate on the GameObjectManager.
-----------------------------------------------------------------------------------------------------------------------
GameObjectManagerProcess = class(ScriptProcess,
{
	_enemies = nil,  -- this points to the same table as GameObjectManager._enemies
});

--------------------

EnemyHealer = class(GameObjectManagerProcess,
{
	--
});

function EnemyHealer:OnUpdate(deltaMs)
	print("Healing all enemies");
	for id, object in pairs(self._enemies) do
		object.hitPoints = object.maxHitPoints;
	end
	g_GameObjectMgr:UpdateUi();
end

--------------------
-- Chapter 21, page 744

EnemyThinker = class(GameObjectManagerProcess,
{
	--
});

function EnemyThinker:OnUpdate(deltaMs)
	print("Running AI update for enemies");
	for id, object in pairs(self._enemies) do
		object.stateMachine:ChooseBestState();
	end
end

--------------------

EnemyUpdater = class(GameObjectManagerProcess,
{
	--
});

function EnemyUpdater:OnUpdate(deltaMs)
	for id, object in pairs(self._enemies) do
		object.stateMachine:Update(deltaMs);
	end
end


-----------------------------------------------------------------------------------------------------------------------
-- GameObjectManager													- Chapter 21, page 739
-- This class manages the script representation of all objects in the game.
-----------------------------------------------------------------------------------------------------------------------
GameObjectManager = class(nil,
{
	_player = nil,  -- this will be filled automatically when player_teapot.xml is loaded
	_enemies = {},  -- a map of enemy teapots; key = objects id
	_spheres = {},  -- a map of spheres; key = objects id
	
	-- processes
	_enemyProcesses = nil;
	
	_enemyHealer = nil,  -- process that periodically heals all enemies
	_enemyThinker = nil,  -- process that causes enemies to make a new decision
	_enemyUpdater = nil,  -- process that updates all enemy states
});

function GameObjectManager:AddPlayer(scriptObject)
	if (self._player ~= nil) then
		print("Attempting to add player to GameObjectManager when one already exists; id = " .. self._player:GetObjectId());
	end

	-- add the new player regardless
	self._player = scriptObject;
	
	-- tell the human view that this is the controlled object
	QueueEvent(EventType.Event_SetControlledObject, self._player:GetObjectId());
end

function GameObjectManager:RemovePlayer(scriptObject)
	self._player = nil;
end

function GameObjectManager:GetPlayer()
	return self._player;
end

function GameObjectManager:AddEnemy(scriptObject)
	print("Adding Enemy");
	
	-- add the enemy to the list of enemies
	local objectId = scriptObject:GetObjectId();
	if (self._enemies[objectId] ~= nil) then
		print("Overwriting enemy object; id = " .. objectId);
	end
	self._enemies[objectId] = scriptObject;
	
	-- set up some sample game data
	scriptObject.maxHitPoints = 3;
	scriptObject.hitPoints = scriptObject.maxHitPoints;

	-- create the teapot brain
	local brain = nil;
	if (TEAPOT_BRAIN) then
		brain = TEAPOT_BRAIN:Create({_teapot = scriptObject});
		if (not brain:Init()) then
			print("Failed to initialize brain");
			brain = nil;
		end
	end

	-- set up the state machine
	scriptObject.stateMachine = TeapotStateMachine:Create({_teapot = scriptObject, _brain = brain});
	
	-- set the initial state
	scriptObject.stateMachine:SetState(PatrolState);
	
	-- increment the enemy count and create the enemy processes if necessary
	if (self._enemyProcesses == nil) then
		self:_CreateEnemyProcesses();
	end
	
	-- make sure the UI is up to date
	self:UpdateUi();
end

function GameObjectManager:RemoveEnemy(scriptObject)
	-- destroy the state machine
	if (scriptObject.stateMachine) then
		scriptObject.stateMachine:Destroy();
		scriptObject.stateMachine = nil;
	end

	-- remove the teapot from the enemy list
	local objectId = scriptObject:GetObjectId();
	self._enemies[objectId] = nil;
	
	-- update the UI
	self:UpdateUi();
end

function GameObjectManager:GetEnemy(objectId)
	return self._enemies[objectId];
end

function GameObjectManager:AddSphere(scriptObject)
	local objectId = scriptObject:GetObjectId();
	if (self._spheres[objectId] ~= nil) then
		print("Overwriting sphere object; id = " .. objectId);
	end
	self._spheres[objectId] = scriptObject;
end

function GameObjectManager:RemoveSphere(scriptObject)
	local objectId = scriptObject:GetObjectId();
	self._spheres[objectId] = nil;
end

function GameObjectManager:GetSphere(objectId)
	return self._spheres[objectId];
end

function GameObjectManager:OnFireWeapon(eventData)
	local aggressor = self:GetObjectId(eventData.id);
	
	if (aggressor == nil) then
		print("FireWeapon from noone?");
		return;
	end;
	
	print("FireWeapon!");
	local pos = Vec3:Create(aggressor:GetPos());
	local lookAt = Vec3:Create(aggressor:GetLookAt());
	lookAt.y = lookAt.y + 1;
	local dir = lookAt * 2;
	pos = pos + dir;
	local ypr = Vec3:Create({x=0, y=0, z=0});
	local ball = CreateObject("gameobjects\\sphere.xml", pos, ypr);
	if (ball ~= -1) then
		dir:Normalize();
		ApplyForce(dir, .3, ball);
	end
end

function GameObjectManager:OnPhysicsCollision(eventData)
	local objectA = self:GetObjectById(eventData.objectA);
	local objectB = self:GetObjectById(eventData.objectB);
	
	-- one of the objects isn't in the script manager
	if (objectA == nil or objectB == nil) then
		return;
	end
	
	local teapot = nil;
	local sphere = nil;
	
	if (objectA.objectType == "Teapot" and objectB.objectType == "Sphere") then
		teapot = objectA;
		sphere = objectB;
	elseif (objectA.objectType == "Sphere" and objectB.objectType == "Teapot") then
		teapot = objectB;
		sphere = objectA;
	end
	
	-- needs to be a teapot and sphere collision for us to care
	if (teapot == nil or sphere == nil) then
		return;
	end
	
	-- If we get here, there was a collision between a teapot and a sphere.  Damage the teapot.
	self:_DamageTeapot(teapot);
	
	-- destroy the sphere
	self:RemoveSphere(sphere);
	QueueEvent(EventType.Event_RequestDestroyGameObject, sphere:GetObjectId());
	
	-- play the hit sound
	QueueEvent(EventType.Event_PlaySound, "audio\\computerbeep3.wav");
end

function GameObjectManager:GetObjectById(objectId)
	local object = nil;

	if (self._player and self._player:GetObjectId() == objectId) then
		object = self._player;
	end
	
	if (not object) then
		object = self:GetEnemy(objectId);
	end
	
	if (not object) then
		object = self:GetSphere(objectId);
	end
	
	return object;
end

function GameObjectManager:_CreateEnemyProcesses()
	self._enemyProcesses = {};

	-- create all enemy processes
	-- [rez] Note: The frequency values probably look a little weird.  I chose these number because they are prime 
	-- numbers, ensuring that these two processes will rarely ever update on the same frame.
	self._enemyProcesses[#self._enemyProcesses+1] = EnemyUpdater:Create({_enemies = self._enemies});
	self._enemyProcesses[#self._enemyProcesses+1] = EnemyHealer:Create({_enemies = self._enemies, frequency = 15013});  -- ~10 seconds
	self._enemyProcesses[#self._enemyProcesses+1] = EnemyThinker:Create({_enemies = self._enemies, frequency = 3499});  -- ~ 3.5 seconds

	-- attach all the processes
	for i, proc in ipairs(self._enemyProcesses) do
		AttachProcess(proc);
	end
end

function GameObjectManager:UpdateUi()
	-- Build up the UI text string for the human view
	local uiText = "";
	if (self._enemies ~= nil) then
		for id, teapot in pairs(self._enemies) do
			uiText = uiText .. "Teapot " .. id .. " HP: " .. teapot.hitPoints .. "\n";
		end
	end
	
	QueueEvent(EventType.Event_GameplayUIUpdate, uiText);
end

function GameObjectManager:_DamageTeapot(teapot)
	-- [rez] DEBUG: player is immune for now....
	if (teapot == self._player) then
		return;
	end

	-- subtract a hitpoint
	teapot.hitPoints = teapot.hitPoints - 1;

	-- if the teapot is dead, destroy it, otherwise update the UI
	if (teapot.hitPoints <= 0) then
		self:RemoveEnemy(teapot);
		QueueEvent(EventType.Event_RequestDestroyGameObject, teapot:GetObjectId());
	else
		self:UpdateUi();
	end
end

