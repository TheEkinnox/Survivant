---@meta

---@class Script
---@field public isValid boolean
---@field public owner Entity
---@field public table table
Script = {}

---OnInit is called when the script is added
function Script:OnInit()
end

---OnStart is called when the application starts
function Script:OnStart()
end

---OnUpdate is called once per frame
---@param deltaTime number
function Script:OnUpdate(deltaTime)
end

---OnCollisionEnter is called when a collision with one of the script owner's colliders occurs
---@param collisionInfo CollisionInfo
function Script:OnCollisionEnter(collisionInfo)
end

---OnCollisionStay is called when a collision with one of the script owner's colliders is repeated for more than one physics update
---@param collisionInfo CollisionInfo
function Script:OnCollisionStay(collisionInfo)
end

---OnCollisionExit is called when a collision with one of the script owner's colliders ends
---@param collisionInfo CollisionInfo
function Script:OnCollisionExit(collisionInfo)
end

---OnTriggerEnter is called when a physics object enters one of the script owner's triggers
---@param triggerInfo TriggerInfo
function Script:OnTriggerEnter(triggerInfo)
end

---OnTriggerExit is called when a physics object exits one of the script owner's triggers
---@param triggerInfo TriggerInfo
function Script:OnTriggerExit(triggerInfo)
end

---OnStop is called when the application stops
function Script:OnStop()
end

---OnDestroy is called when the script is removed
function Script:OnDestroy()
end