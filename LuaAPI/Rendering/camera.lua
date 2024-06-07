---@meta

---@class Camera
---@field public isActive boolean
---@field public order number
---@field public projectionType EProjectionType
---@field public clearColor Color
---@field public clearMask ClearMask
---@field public cullingMask LayerMask
---@field public aspect number READ-ONLY
---@field public fovY Radian
---@field public perspectiveNear number
---@field public perspectiveFar number
---@field public orthographicSize number
---@field public orthographicNear number
---@field public orthographicFar number
Camera = {}

---Sets the camera's clear color
---@param color Color
---@overload fun(r: number, g: number, b: number): Camera
---@overload fun(r: number, g: number, b: number, a: number): Camera
---@return Camera
function Camera:SetClearColor(color) end

---Clears the current frame buffer using the camera's clear information
function Camera:Clear() end

---Sets the camera's field of view and its perspective near and far clipping planes
---@param fovY Radian
---@param zNear number
---@param zFar number
function Camera:SetPerspective(fovY, zNear, zFar) end

---Sets the camera's orthographic size and near/far clipping planes
---@param size number
---@param zNear number
---@param zFar number
function Camera:SetOrthographic(size, zNear, zFar) end

---Breaks the buffer clearing mask into separate values
---@return (boolean, boolean, boolean)
function Camera.BreakClearMask() end

---Packs the given buffer clearing flags into a mask
---@param color boolean
---@param depth boolean
---@param stencil boolean
---@return ClearMask
function Camera.MakeClearMask(color, depth, stencil) end