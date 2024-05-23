---@class Move : Script
local Move = {
    mouse_sensitivity = 0.005,
    max_speed = 3,
    max_acceleration = 3,
    rotation_speed = Degree.new(90)
}

local transform
local rigidbody
local last_mouse_pos

function Move:OnInit()
end

function Move:OnStart()
    last_mouse_pos = Input.mousePos
    transform = self.owner:GetOrCreate(Transform)
    rigidbody = self.owner:GetOrCreate(RigidBody)

    if not PLAYER_CAM_TRANSFORM then
        PLAYER_CAM_TRANSFORM = transform
    end
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

local function UpdateKeyboardRotation(self, deltaTime)
    local rotateInput = Vector2.new()

    if Input.IsKeyDown(EKey.LEFT) then
        rotateInput.x = rotateInput.x + 1
    end
    
    if Input.IsKeyDown(EKey.RIGHT) then
        rotateInput.x = rotateInput.x - 1
    end

    if rotateInput.magnitudeSquared > 0 then
        local spin = self.rotation_speed * rotateInput.x * deltaTime

        rigidbody.angularVelocity = Vector3.new(0, spin.raw, 0)
    else
        rigidbody.angularVelocity = Vector3.new(0,0,0)
    end
end

function Move:OnUpdate(deltaTime)
    UpdatePosition(self, deltaTime)
    UpdateKeyboardRotation(self, deltaTime)
end

function Move:OnStop()
    transform = nil
end

return Move