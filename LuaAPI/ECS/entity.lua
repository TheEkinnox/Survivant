--TODO: Entity lua api
---@meta

---@class EComponentSearchOrigin
EComponentSearchOrigin = {
    ROOT = 0,
    PARENT = 1,
    CHILDREN = 2
}

---@class Entity
---@field public isValid boolean READ_ONLY
---@field public scene Scene READ_ONLY
---@field public root Entity READ_ONLY
---@field public parent Entity
---@field public nextSibling Entity READ_ONLY
---@field public previousSibling Entity READ_ONLY
---@field public childCount number READ_ONLY
---@field public children number READ_ONLY | Note: Heavy engine call, to iterate over all children use childCount and GetChild
---@field public componentCount number READ_ONLY
---@field public components table<number, Component> READ_ONLY
Entity = {}

---Creates a copy of the entity
---@return Entity
function Entity:Copy() end

---Removes the entity from its scene
function Entity:Destroy() end

---Gets the child at the given index if it exists or an invalid entity otherwise
---@param index number Note: Child indices start at 0
---@return Entity
function Entity:GetChild(index) end

---Adds a new child entity to this one
---@return Entity
function Entity:AddChild() end

---Sets the entity's parent
---@param parent Entity
---@return Entity
---@overload fun(parent: Entity, keepWorld: boolean): Entity
function Entity:SetParent(parent) end

---Checks whether the entity has the script with the specified name
---@param name string
---@return boolean
function Entity:HasScript(name) end

---Gets the script with the specified name on the entity
---@param name string
---@return Script The found script on success. An invalid script otherwise
function Entity:GetScript(name) end

---Adds the script with the specified name to the entity
---@param name string
---@return Script The added script on success. An invalid script otherwise
function Entity:AddScript(name) end

---Gets or adds the script with the specified name to the entity
---@param name string
---@return Script The found or added script on success. An invalid script otherwise
function Entity:RequireScript(name) end

---Removes the script with the specified name from the entity
---@param name string
function Entity:RemoveScript(name) end

---Checks whether the entity has a component of the given type
---@param type string|table
---@return boolean
function Entity:Has(type) end

---Gets the component of the given type on the entity
---@param type string|table
---@return Component The found component on success. An invalid component otherwise
function Entity:Get(type) end

---Gets or adds the component of the given type to the entity
---@param type string|table
---@return Component The found or added component on success. An invalid component otherwise
function Entity:GetOrCreate(type) end

---Gets the component of the given type on the entity or one of its parents
---@param type string|table
---@return Component The found component on success. An invalid component otherwise
function Entity:GetInParent(type) end

---Gets the component of the given type on the entity or in one of its child hierarchies
---@param type string|table
---@return Component The found component on success. An invalid component otherwise
function Entity:GetInChildren(type) end

---Gets the component of the given type on the entity's hierarchy starting at the given origin
---@param type string|table
---@param searchOrigin EComponentSearchOrigin
---@return Component The found component on success. An invalid component otherwise
function Entity:GetInHierarchy(type, searchOrigin) end

---Removes the given component from the entity
---@param type string|table
---@overload fun(component: Component)
function Entity:Remove(type) end
