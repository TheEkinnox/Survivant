---@meta

---@class ResourceManager
---@field public searchPaths table<number, string>
ResourceManager = {}

---Loads the resource of the given type at the given path
---@param type string
---@param path string
---@return Resource
function ResourceManager.Load(type, path) end

---Gets the loaded resource of the given type at the given path
---@param type string
---@param path string
---@return Resource
function ResourceManager.Get(type, path) end

---Gets or loads the resource of the given type at the given path
---@param type string
---@param path string
---@return Resource
function ResourceManager.GetOrCreate(type, path) end

---Gets all the loaded resources of the given type
---@param type string
---@return table<number, Resource>
function ResourceManager.GetAll(type, path) end

---Removes the resource at the given path from the resource manager
---@param path string
function ResourceManager.Remove(path) end

---Removes all the resources of the given type from the resource manager
---@param type string
function ResourceManager.RemoveAll(type) end

---Unloads all the currently loaded resources
function ResourceManager.Clear() end

---Adds the given path to the resource manager's search paths
---@param path string
function ResourceManager.AddSearchPath(path) end

---Removes the given path from the resource manager's search paths
---@param path string
function ResourceManager.RemoveSearchPath(path) end

---Gets the full path of a resource from its relative path
---@param path string
---@return string
function ResourceManager.GetFullPath(path) end

---Gets the relative path of a resource from its full path
---@param path string
---@return string
function ResourceManager.GetRelativePath(path) end