--TODO: Entity lua api
---@meta

---@class EComponentSearchOrigin
EComponentSearchOrigin = {
    ROOT = 0,
    PARENT = 1,
    CHILDREN = 2
}

---@class Entity
---@field public componentCount number
---@field public components table<number, Component>
Entity = {}

---Checks whether the entity has the script with the specified name.
---@param name string
function Entity.HasScript(name) end

---Gets the script with the specified name.
---@param name string
---@return Script The found script on success. An invalid script otherwise.
function Entity.GetScript(name) end
