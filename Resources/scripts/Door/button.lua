---@class Button : Script
local Button = {
    onActiveYScaleMult = 0.5
}

local numCollision = 0
local transform
local originalYScale

function Button:OnStart()
    transform = self.owner:GetOrCreate(Transform)
    originalYScale = transform.scale.y
end

function Button.IsActive()
    return numCollision ~= 0
end

function Button:OnTriggerEnter()
    if numCollision == 0 then
        transform.scale.y = originalYScale * self.onActiveYScaleMult
    end
    
    print("ON triggered")
    numCollision = numCollision + 1
end

function Button:OnTriggerExit()
    numCollision = numCollision - 1
    print("EXIT triggered")

    if numCollision == 0 then
        transform.scale.y = originalYScale
    end
end

return Button