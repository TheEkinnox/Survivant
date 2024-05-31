---@meta

---@class Degree
---@field public raw number READ-ONLY
---@field public rawDegree number READ-ONLY
---@field public rawRadian number READ-ONLY
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

---Gets the angle in degrees wrapped in the range [-180, 180[ if useNegative or to range [0, 360[
---@param useNegative boolean
---@return number
function Degree:GetWrappedDegree(useNegative) end

---Gets the angle in radian wrapped in the range [-pi, pi[ if useNegative or to range [0, 2pi[
---@param useNegative boolean
---@return number
function Degree:GetWrappedRadian(useNegative) end

---@class Radian
---@field public raw number READ-ONLY
---@field public rawDegree number READ-ONLY
---@field public rawRadian number READ-ONLY
Radian = {}

---Creates an angle in radian
---@param angle number
---@return Radian
---@overload fun() : Radian
---@overload fun(other : Radian) : Radian
---@overload fun(degree : Degree) : Radian
function Radian.new(angle) end

---Wraps the angle in the range [-pi, pi[ if useNegative or to range [0, 2pi[
---@param useNegative boolean
function Radian:Wrap(useNegative) end

---Gets the angle in degrees wrapped in the range [-180, 180[ if useNegative or to range [0, 360[
---@param useNegative boolean
---@return number
function Radian:GetWrappedDegree(useNegative) end

---Gets the angle in radian wrapped in the range [-pi, pi[ if useNegative or to range [0, 2pi[
---@param useNegative boolean
---@return number
function Radian:GetWrappedRadian(useNegative) end