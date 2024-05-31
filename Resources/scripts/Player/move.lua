---@class Move : Script
local Move = {
    mouse_sensitivity = 0.005,
    max_speed = 3,
    max_acceleration = 3,
    rotation_speed = Degree.new(90),
    camera = Entity.new(),
    min_x_angle = Degree.new(-85),
    max_x_angle = Degree.new(85)
}

---@type Component|Transform
local transform
---@type Component|Transform
local camTransform
local rigidbody
local last_mouse_pos

function Move:OnInit()
end

function Move:OnStart()
    last_mouse_pos = Input.mousePos
    transform = self.owner:GetOrCreate(Transform)
    rigidbody = self.owner:GetOrCreate(RigidBody)
    camTransform = self.camera:Get(Transform)
end

local function UpdatePosition(self, deltaTime)
    local moveInput = Vector2.zero

    if Input.IsKeyDown(EKey.W) then
        moveInput.y = moveInput.y + 1
    end

    if Input.IsKeyDown(EKey.S) then
        moveInput.y = moveInput.y - 1
    end

    if Input.IsKeyDown(EKey.A) then
        moveInput.x = moveInput.x - 1
    end

    if Input.IsKeyDown(EKey.D) then
        moveInput.x = moveInput.x + 1
    end

    local desiredVelocity = Vector3.zero
    local maxSpeed = math.max(0, self.max_speed)

    if moveInput.magnitudeSquared > 0 and (rigidbody.velocity.magnitudeSquared < (maxSpeed * maxSpeed)) then
        local moveDir = ((transform.right * moveInput.x) + (transform.back * moveInput.y)).normalized
        local force = moveDir * self.max_acceleration * rigidbody.mass * deltaTime

        rigidbody.self:AddForce(force, EForceMode.IMPULSE)
    end
end

local function UpdateCamRotation(self, deltaTime)
    if not camTransform.isValid then
        return
    end

    local rotateInput = 0

    if Input.IsKeyDown(EKey.UP) then
        rotateInput = rotateInput + 1
    end

    if Input.IsKeyDown(EKey.DOWN) then
        rotateInput = rotateInput - 1
    end

    if math.abs(rotateInput) > 0 then
        local x, y, z = camTransform.rotation:ToEuler(ERotationOrder.YXZ)

        x = Degree.new(x + self.rotation_speed * rotateInput * deltaTime)
        x:Wrap(true)
        x = Radian.new(math.clamp(x, self.min_x_angle, self.max_x_angle))

        camTransform.rotation = Quaternion.FromEuler(x, y, z, ERotationOrder.YXZ)
    end
end

local function UpdatePlayerRotation(self, deltaTime)
    local rotateInput = 0

    if Input.IsKeyDown(EKey.LEFT) then
        rotateInput = rotateInput + 1
    end

    if Input.IsKeyDown(EKey.RIGHT) then
        rotateInput = rotateInput - 1
    end

    if math.abs(rotateInput) > 0 then
        local spin = self.rotation_speed * rotateInput * deltaTime

        rigidbody.angularVelocity = Vector3.new(0, spin.rawDegree, 0)
    else
        rigidbody.angularVelocity = Vector3.new(0, 0, 0)
    end
end

function Move:OnUpdate(deltaTime)
    UpdatePosition(self, deltaTime)
    UpdatePlayerRotation(self, deltaTime)
    UpdateCamRotation(self, deltaTime)
end

function Move:OnStop()
    transform = nil
end

return Move