---@class Button : Script
local Button = {
    onActiveYMove = 0.1
}

local numCollision = 0
local transform
local originalPos

function Button:OnStart()
    transform = self.owner:GetOrCreate(Transform)
    originalPos = transform.position
end

function Button.IsActive()
    return numCollision ~= 0
end

function Button:OnTriggerEnter()
    if numCollision == 0 then
        transform.position = Vector3.new(originalPos.x, originalPos.y - self.onActiveYMove, originalPos.z)
    end
    
    print("ON triggered")
    numCollision = numCollision + 1
end

function Button:OnTriggerExit()
    numCollision = numCollision - 1
    
    print("ON exit")
    if numCollision == 0 then
        transform.position = originalPos
    end
end

return Button