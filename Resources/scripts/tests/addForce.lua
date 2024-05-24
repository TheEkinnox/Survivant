---@class UtilityTest : Script
local UtilityTest = {
    direction = Vector3.new(1, 0, 0),
    force = 5
}

function UtilityTest:OnStart()
    local rigidbody = self.owner:GetOrCreate(RigidBody)

    print(self.direction.normalized * self.force)
    rigidbody.self:AddForce(self.direction.normalized * self.force, EForceMode.IMPULSE)
end

return UtilityTest