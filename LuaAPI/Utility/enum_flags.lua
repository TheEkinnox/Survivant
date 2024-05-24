---@meta

---@class EnumFlags : number
---@field public raw number
EnumFlags = {}

---Sets the given flag's state
---@param flag any
---@param state boolean
---@return EnumFlags
function EnumFlags:Set(flag, state) end

---Checks whether the given flag is set
---@param flag any
---@return boolean
function EnumFlags:IsSet(flag) end