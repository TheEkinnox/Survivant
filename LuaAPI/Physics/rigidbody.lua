---@meta

---@class RigidBody
---@field public mass number
---@field public velocity Vector3
---@field public angularVelocity Vector3
---@field public isKinematic boolean
---@field public useGravity boolean
---@field public collisionDetectionMode ECollisionDetectionMode
---@field public isSleeping boolean
RigidBody = {}

---Forces the rigid body to sleep
function RigidBody:Sleep() end

---Forces the rigid body to wake up
function RigidBody:WakeUp() end

---Adds the given force to the rigid body
---@param force Vector3
---@param mode EForceMode Default is EForceMode.FORCE
---@param autoWake boolean Default is true
---@overload fun(force: Vector3)
---@overload fun(force: Vector3, mode: EForceMode)
function RigidBody:AddForce(force, mode, autoWake) end

---Adds the given torque to the rigid body
---@param torque Vector3
---@param mode EForceMode Default is EForceMode.FORCE
---@param autoWake boolean Default is true
---@overload fun(torque: Vector3)
---@overload fun(torque: Vector3, mode: EForceMode)
function RigidBody:AddTorque(torque, mode, autoWake) end
