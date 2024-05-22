---@meta

---@class Resource
---@field public isValid boolean Read-Only
---@field public type string Read-Only
---@field public path string Read-Only
---@field public fullPath string Read-Only
---@field public self userdata
Resource = {}

---Loads the resource of the given type at the given path
---@param type string
---@param path string
---@return Resource
---@overload fun()
---@overload fun(other: Resource)
function Resource.new(type, path) end