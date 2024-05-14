---@class DeathTrigger : Script
local DeathTrigger = {}

local deferredDestroy = require("scripts.deferredDestroy")

---@param collider Component|Collider
local function PrintCollider(collider)
    if BoxCollider.__type.is(collider.self) then
        Debug.Log("[DEATH_ZONE] Box collider offset = " .. collider.offset .. " | size = " .. collider.size)
    elseif SphereCollider.__type.is(collider.self) then
        Debug.Log("[DEATH_ZONE] Sphere collider offset = " .. collider.offset .. " | radius = " .. collider.radius)
    elseif CapsuleCollider.__type.is(collider.self) then
        Debug.Log("[DEATH_ZONE] Capsule collider offset = " .. collider.offset .. " | radius = " .. collider.radius ..
                " | axis = " .. collider.axis .. " | height = " .. collider.height)
    end
end

function DeathTrigger:OnCollisionEnter(collisionInfo)
    ---@type Collider
    local target

    if collisionInfo.colliders[1].owner ~= self.owner then
        target = collisionInfo.colliders[1]
    else
        target = collisionInfo.colliders[2]
    end

    Debug.Log("Destroying entity " .. target.owner .. " on collision")
    PrintCollider(target)

    deferredDestroy:Destroy(target.owner)
end

function DeathTrigger:OnTriggerEnter(triggerInfo)
    Debug.Log("Destroying entity " .. triggerInfo.otherCollider.owner .. " on trigger")
    PrintCollider(triggerInfo.otherCollider)

    deferredDestroy:Destroy(triggerInfo.otherCollider.owner)
end

function DeathTrigger:OnUpdate()
    deferredDestroy:Process()
end

return DeathTrigger