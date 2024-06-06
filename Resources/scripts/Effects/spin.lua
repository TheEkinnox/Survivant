---@class Spin : Script
local Spin = {
    spin = 1
}

local rigidbody

function Spin:OnInit()
    self.owner:GetOrCreate(Transform)
    self.owner:GetOrCreate(RigidBody)
end

function Spin:OnStart()
    self.owner:GetOrCreate(Transform)
    rigidbody = self.owner:GetOrCreate(RigidBody)
    rigidbody.angularVelocity = Vector3.new(0, spin, 0)
end

local function UpdatePosition(self, deltaTime)
end

function Spin:OnUpdate(deltaTime)
    UpdatePosition(self, deltaTime)
end

function Spin:OnStop()
end

return Spin