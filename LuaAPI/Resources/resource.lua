---@meta

---@class Resource
---@field public isValid boolean READ-ONLY
---@field public type string READ-ONLY
---@field public path string READ-ONLY
---@field public fullPath string READ-ONLY
---@field public self userdata
Resource = {}

---Loads the resource of the given type at the given path
---@param type string
---@param path string
---@return Resource
---@overload fun()
---@overload fun(other: Resource)
function Resource.new(type, path) end