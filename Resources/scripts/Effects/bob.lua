---@class Bob : Script
local Bob = {
    direction = Vector3.new(0, 1, 0),
    speed = 1
}

local transform
local position
local timer = 0

function Bob:OnInit()
end

function Bob:OnStart()
    transform = self.owner:GetOrCreate(Transform)

    position = transform.position
end

local function UpdatePosition(self, deltaTime)
    timer = timer + deltaTime

    local t = self.speed * timer * 6.28318530718
    transform.position = position + (self.direction * math.sin(t)) 
end

function Bob:OnUpdate(deltaTime)
    UpdatePosition(self, deltaTime)
end

function Bob:OnStop()
end

return Bob