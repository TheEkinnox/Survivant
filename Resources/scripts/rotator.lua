local Rotator = {
    rotation_speed = {
        x = Degree.new(0),
        y = Degree.new(36),
        z = Degree.new(0)
    }
}

local transform

function Rotator:OnInit()
    transform = self.owner:GetOrCreate("Transform")
end

function Rotator:OnUpdate(deltaTime)
    if deltaTime == 0 then
        return
    end

    local x, y, z = transform.rotation:ToEuler(ERotationOrder.YXZ)
    x = x + self.rotation_speed.x * deltaTime
    y = y + self.rotation_speed.y * deltaTime
    z = z + self.rotation_speed.z * deltaTime

    transform.rotation = Quaternion.FromEuler(x, y, z, ERotationOrder.YXZ)
end

return Rotator