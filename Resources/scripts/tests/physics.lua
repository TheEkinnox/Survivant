---@class PhysicsTest : Script
local PhysicsTest = {
    scene_offset = Vector3.new(0, 2, -5),
    projectile_force = 100,
    projectile_mass = 5,
    projectile_offset = .5,
    projectile_delay = .1,
    projectile_lifetime = 3
}

local scene
local remaining_delay

local function AddRigidBody()
    local entity = scene:Create()
    local rb = entity:GetOrCreate(RigidBody)
    return entity, rb.self
end

local function AddBoxCollider(entity, material, size, offset)
    size = size or Vector3.one
    offset = offset or Vector3.zero

    local collider = entity:GetOrCreate(BoxCollider)
    collider.size = size
    collider.offset = offset

    local modelComponent = entity:GetOrCreate(Model)
    modelComponent.model = Resource.new("Model", "models/cube.obj")
    modelComponent.material = material or Resource.new("Material", "materials/white.mat")
    return collider
end

local function AddSphereCollider(entity, material, radius, offset)
    radius = radius or .5
    offset = offset or Vector3.zero

    local collider = entity:GetOrCreate(SphereCollider)

    collider.radius = radius
    collider.offset = offset

    local modelComponent = entity:GetOrCreate(Model)
    modelComponent.model = Resource.new("Model", "models/sphere.obj")
    modelComponent.material = material or Resource.new("Material", "materials/white.mat")
    return collider
end

local function AddDeathZone(self, transform, isTrigger)
    local deathZoneEntity = scene:Create()
    deathZoneEntity.parent = self.owner
    deathZoneEntity:AddScript("scripts.deathZone")

    local entityTransform = deathZoneEntity:GetOrCreate(Transform)
    entityTransform.self:SetAll(transform.position, transform.rotation, transform.scale)

    local material = isTrigger and Resource.new("Material", "materials/green.mat") or Resource.new("Material", "materials/red.mat")
    local deathCollider = AddBoxCollider(deathZoneEntity, material)
    deathCollider.isTrigger = isTrigger
end

local function MakeSim(self)
    local floorEntity = scene:Create()
    floorEntity.parent = self.owner
    local transform = floorEntity:GetOrCreate(Transform)
    transform.self:SetAll(Vector3.new(0, -.05, 0), Quaternion.identity, Vector3.new(15, .1, 15))
    AddBoxCollider(floorEntity)

    AddDeathZone(self, Transform.new(Vector3.new(5, 4, 0), Quaternion.identity, Vector3.new(.1, 4, 4)), true)
    AddDeathZone(self, Transform.new(Vector3.new(-5, 4, 0), Quaternion.identity, Vector3.new(.1, 4, 4)), false)

    local entity = AddRigidBody()
    entity:Get(Transform).position = Vector3.new(-1.5, 0, 0) + self.scene_offset
    AddBoxCollider(entity)

    entity = AddRigidBody()
    entity:Get(Transform).position = Vector3.new(-.5, 0, 0) + self.scene_offset
    AddBoxCollider(entity)

    entity = AddRigidBody()
    entity:Get(Transform).position = Vector3.new(.5, 0, 0) + self.scene_offset
    AddBoxCollider(entity)

    entity = AddRigidBody()
    entity:Get(Transform).position = Vector3.new(1.5, 0, 0) + self.scene_offset
    AddBoxCollider(entity)

    entity = AddRigidBody()
    entity:Get(Transform).position = Vector3.new(1, 2, 0) + self.scene_offset
    AddBoxCollider(entity)

    entity = AddRigidBody()
    entity:Get(Transform).position = Vector3.new(0, 2, 0) + self.scene_offset
    AddBoxCollider(entity)

    entity = AddRigidBody()
    entity:Get(Transform).position = Vector3.new(-1, 2, 0) + self.scene_offset
    AddBoxCollider(entity)

    entity = AddRigidBody()
    entity:Get(Transform).position = Vector3.new(-.5, 4, 0) + self.scene_offset
    AddBoxCollider(entity)

    entity = AddRigidBody()
    entity:Get(Transform).position = Vector3.new(.5, 4, 0) + self.scene_offset
    AddBoxCollider(entity)

    entity = AddRigidBody()
    entity:Get(Transform).position = Vector3.new(0, 6, 0) + self.scene_offset
    AddBoxCollider(entity)
end

function PhysicsTest:OnStart()
    if PHYSICS_TEST and PHYSICS_TEST ~= self then
        Debug.LogWarning("Physics test already exists in E" .. PHYSICS_TEST.owner .. " - Removing from E" .. self.owner)
        self.owner:RemoveScript("scripts.tests.physics")
        return
    end

    print("[PHYSICS] Start time: " .. Time.time)

    PHYSICS_TEST = self
    scene = self.owner.scene
    remaining_delay = 0
    MakeSim(self)
end

local function ShootProjectile(self)
    if not CAM_TRANSFORM then
        Debug.LogError("Missing cam transform. Make sure the freelook script has been added to at least one entity")
        return
    end

    local entity, rb = AddRigidBody()
    rb.mass = self.projectile_mass

    local tmp = entity:AddScript("scripts.temporary")
    tmp.lifetime_sec = self.projectile_lifetime

    local dir = CAM_TRANSFORM.back
    local projectilePos = CAM_TRANSFORM.position + (dir * self.projectile_offset)
    entity:Get(Transform).self:SetAll(projectilePos, Quaternion.identity, Vector3.new(.1))
    AddSphereCollider(entity)

    rb:AddForce(dir * self.projectile_force, EForceMode.IMPULSE)
end

function PhysicsTest:OnUpdate(deltaTime)
    if remaining_delay <= 0 and Input.IsMouseButtonDown(EMouseButton.LEFT) then
        ShootProjectile(self)
        remaining_delay = self.projectile_delay
    else
        remaining_delay = remaining_delay - deltaTime
    end
end

function PhysicsTest:OnDestroy()
    if instance == self then
        instance = nil
    end
end

return PhysicsTest