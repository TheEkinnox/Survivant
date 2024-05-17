---@meta

---@class CollisionInfo
---@field public colliders table<number, Component|Collider>
---@field public contacts table<number, Contact>
CollisionInfo = {}

---@class Contact
---@field public position Vector3
---@field public impulse Vector3
---@field public normal Vector3
---@field public distance number
Contact = {}