---@meta

---@class Degree
---@field public raw number READ-ONLY
Degree = {}

---Creates an angle in degrees
---@param angle number
---@return Degree
---@overload fun() : Degree
---@overload fun(other : Degree) : Degree
---@overload fun(radian : Radian) : Degree
function Degree.new(angle) end

---Wraps the angle in the range [-180, 180[ if useNegative or to range [0, 360[
---@param useNegative boolean
function Degree:Wrap(useNegative) end

---@class Radian
---@field public raw number READ-ONLY
Radian = {}

---Creates an angle in radian
---@param angle number
---@return Radian
---@overload fun() : Radian
---@overload fun(other : Radian) : Radian
---@overload fun(degree : Degree) : Radian
function Radian.new(angle) end

---Wraps the angle in the range [-180, 180[ if useNegative or to range [0, 360[
---@param useNegative boolean
function Radian:Wrap(useNegative) end