---@class Jump : Script
local Jump = {
    jump_force = 4,
    ground_steepness = 0.45,
    jump_sound = Resource.new("AudioClip", "sounds/jump-start.wav"),
    landing_sound = Resource.new("AudioClip", "sounds/jump-end.wav")
}

local rigidbody
local is_grounded
local holding_jump

function Jump:OnStart()
    rigidbody = self.owner:GetOrCreate(RigidBody)
    is_grounded = false
    holding_jump = false
end

local function UpdateJump(self, deltaTime)
    local jumpkey = Input.IsKeyDown(EKey.SPACE)

    if not holding_jump and jumpkey and is_grounded then
        Audio:Play(self.jump_sound)
        rigidbody.self:AddForce(Vector3.up * self.jump_force * rigidbody.mass, EForceMode.IMPULSE)
    end

    holding_jump = jumpkey
end

function Jump:OnUpdate(deltaTime)
    UpdateJump(self, deltaTime)
end

---@param collisionInfo CollisionInfo
local function EvaluateCollision(self, collisionInfo)
    local mult

    if collisionInfo.colliders[1].owner == self.owner then
        mult = 1
    else
        mult = -1
    end

    for _, contact in ipairs(collisionInfo.contacts) do
        local y = contact.normal.y * mult

        if y and y > self.ground_steepness then
            if not is_grounded then
                Audio:Play(self.landing_sound)
            end

            is_grounded = true
            break
        end
    end
end

function Jump:OnCollisionEnter(collisionInfo)
    EvaluateCollision(self, collisionInfo)
end

function Jump:OnCollisionStay(collisionInfo)
    EvaluateCollision(self, collisionInfo)
end

function Jump:OnCollisionExit(collisionInfo)
    is_grounded = false
end

return Jump