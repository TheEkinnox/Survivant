---@meta

---@class Input
---@field public mousePos Vector2
Input = {}

---Checks whether the given key is currently up or not
---@param key EKey
---@return boolean
---@overload fun(key: EKey, modifier: EInputModifer)
function Input.IsKeyUp(key) end

---Checks whether the given key is currently down or not
---@param key EKey
---@return boolean
---@overload fun(key: EKey, modifier: EInputModifer)
function Input.IsKeyDown(key) end

---Checks whether the given mouse button is currently up or not
---@param button EMouseButton
---@return boolean
---@overload fun(button: EMouseButton, modifier: EInputModifer)
function Input.IsMouseButtonUp(button) end

---Checks whether the given mouse button is currently down or not
---@param button EMouseButton
---@return boolean
---@overload fun(button: EMouseButton, modifier: EInputModifer)
function Input.IsMouseButtonDown(button) end
