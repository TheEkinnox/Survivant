---@meta

---@class Color
---@field public vec3 Vector3 READ-ONLY
---@field public vec4 Vector4 READ-ONLY
---@field public r number
---@field public g number
---@field public b number
---@field public a number
---@field public red Color READ-ONLY
---@field public green Color READ-ONLY
---@field public blue Color READ-ONLY
---@field public cyan Color READ-ONLY
---@field public magenta Color READ-ONLY
---@field public yellow Color READ-ONLY
---@field public lime Color READ-ONLY
---@field public skyBlue Color READ-ONLY
---@field public orange Color READ-ONLY
---@field public black Color READ-ONLY
---@field public darkGray Color READ-ONLY
---@field public gray Color READ-ONLY
---@field public lightGray Color READ-ONLY
---@field public white Color READ-ONLY
Color = {}

---Creates a new color
---@param r number
---@param g number
---@param b number
---@param a number
---@return Color
---@overload fun(): Color
---@overload fun(r: number, g: number, b: number): Color
---@overload fun(vec3: Vector3): Color
---@overload fun(vec3: Vector3, alpha: number): Color
---@overload fun(vec4: Vector4): Color
---@overload fun(other: Color): Color
function Color.new(r, g, b, a) end