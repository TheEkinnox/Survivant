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

---Unloads the resource at the given path
---@param path string
function ResourceManager.Remove(path) end

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