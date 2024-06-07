---@class Button : Script
local Button = {
    onActiveYMove = 0.1,
    onActiveSound = Resource.new("AudioClip", "sounds/click-on.wav"),
    onInactiveSound = Resource.new("AudioClip", "sounds/click-off.wav")
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
        Audio:Play(self.onActiveSound)
    end

    print("ON triggered")
    numCollision = numCollision + 1
end

function Button:OnTriggerExit()
    numCollision = numCollision - 1

    print("ON exit")
    if numCollision == 0 then
        transform.position = originalPos
        Audio:Play(self.onInactiveSound)
    end
end

return Button