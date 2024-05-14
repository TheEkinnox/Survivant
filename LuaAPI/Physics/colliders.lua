---@meta

---@class Collider
---@field public isTrigger boolean
---@field public material Resource|PhysicsMaterial
---@field public offset Vector3
---@field public boundingBox BoundingBox
---@field public boundingSphere BoundingSphere
Collider = {}

---@class BoxCollider : Collider
---@field public size Vector3
BoxCollider = {}

---@class SphereCollider : Collider
---@field public radius number
SphereCollider = {}

---@class CapsuleCollider : Collider
---@field public axis Vector3
---@field public height number
---@field public radius number
CapsuleCollider = {}
