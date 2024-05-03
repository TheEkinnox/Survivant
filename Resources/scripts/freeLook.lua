local FreeLook = {
    move_speed = 3,
    rotation_speed = Degree.new(90),
    mouse_sensitivity = 0.8
}

local transform
local cam

local last_mouse_pos

function FreeLook:RequireComponent(type)
    local comp = self.owner:Get(type)
    return comp.isValid and comp or self.owner:Add(type)
end

function FreeLook:OnInit()
    transform = self:RequireComponent("Transform")
    cam = self:RequireComponent("Camera")
end

function FreeLook:OnStart()
    last_mouse_pos = Input.mousePos
end

local function UpdatePosition(self, deltaTime)
    local moveInput = Vector2.new()

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

    moveInput:Normalize()

    if moveInput.magnitudeSquared > 0 then
        local moveDir = ((transform.right * moveInput.x) + (transform.back * moveInput.y)).normalized
        transform.position = transform.position + (moveDir * self.move_speed * deltaTime), true
    end
end

local function UpdateKeyboardRotation(self, deltaTime)
    local rotateInput = Vector2.new()

    if Input.IsKeyDown(EKey.UP) then
        rotateInput.y = rotateInput.y + 1
    end

    if Input.IsKeyDown(EKey.DOWN) then
        rotateInput.y = rotateInput.y - 1
    end

    if Input.IsKeyDown(EKey.LEFT) then
        rotateInput.x = rotateInput.x - 1
    end

    if Input.IsKeyDown(EKey.RIGHT) then
        rotateInput.x = rotateInput.x + 1
    end

    rotateInput:Normalize()

    if rotateInput.magnitudeSquared > 0 then
        local x, y, z = transform.rotation:ToEuler(ERotationOrder.YXZ)

        y = y - self.rotation_speed * rotateInput.x * deltaTime
        x = Degree.new(x + self.rotation_speed * rotateInput.y * deltaTime)
        x:Wrap(true)
        x = Radian.new(Degree.new(math.clamp(x.raw, -85, 85)))

        transform.rotation = Quaternion.FromEuler(x, y, z, ERotationOrder.YXZ)
    end
end

local function UpdateMouseRotation(self, deltaTime)
    local mouseDelta = Input.mousePos - last_mouse_pos
    last_mouse_pos = Input.mousePos

    if not (Input.IsMouseButtonDown(EMouseButton.LEFT) or Input.IsMouseButtonDown(EMouseButton.RIGHT)) then
        return
    end

    local rotationSpeed = self.rotation_speed * self.mouse_sensitivity * deltaTime

    if math.isNear(rotationSpeed.raw, 0) or math.isNear(mouseDelta.magnitudeSquared, 0) then
        return
    end

    local x, y, z = transform.rotation:ToEuler(ERotationOrder.YXZ)

    y = y - rotationSpeed * mouseDelta.x
    x = Degree.new(x - rotationSpeed * mouseDelta.y)
    x:Wrap(true)
    x = Radian.new(Degree.new(math.clamp(x.raw, -85, 85)))

    transform.rotation = Quaternion.FromEuler(x, y, z, ERotationOrder.YXZ)
end

function FreeLook:OnUpdate(deltaTime)
    UpdatePosition(self, deltaTime)
    UpdateMouseRotation(self, deltaTime)
    UpdateKeyboardRotation(self, deltaTime)
end

function FreeLook:OnStop()
    transform = nil
    cam = nil
end

return FreeLook