---@class Cursor : Script
local Cursor = {}

function Cursor:OnInit()
    self.owner:GetOrCreate(SphereCollider)
    local rigidBody = self.owner:GetOrCreate(RigidBody)

    rigidBody.isKinematic = true
end

return Cursor
