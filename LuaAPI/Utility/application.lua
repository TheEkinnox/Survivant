---@meta

---@class Application
---@field public isEditor boolean Read-Only
---@field public currentScene Resource | Scene
Application = {}

---Switches to the given scene
---@param scene Resource | string
function Application.ChangeScene(scene) end