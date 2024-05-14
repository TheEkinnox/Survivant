---@class MathTest : Script
local MathTest = {}

local function TestAngles()
    local deg = Degree.new(180)

    assert(type(deg) == "userdata", "Expected \"userdata\" - Received \"" .. type(deg) .. "\"")
    assert(deg.__type.name == Degree.__type.name, "Expected: " .. Degree.__type.name)

    assert(deg.raw == 180)
    assert(deg + deg == Degree.new(360))
    assert(deg - deg == Degree.new(0))
    assert(deg * 2 == Degree.new(360))
    assert(deg / 2 == Degree.new(90))
    assert(deg / 2 < deg)
    assert(deg * 2 > deg)

    local rad = Radian.new(math.pi)

    assert(type(rad) == "userdata", "Expected \"userdata\" - Received \"" .. type(rad) .. "\"")
    assert(rad.__type.name == Radian.__type.name, "Expected: " .. Radian.__type.name)

    assert(math.isNear(rad.raw, math.pi))
    assert(rad + rad == Radian.new(math.pi * 2))
    assert(rad - rad == Radian.new(0))
    assert(rad * 2 == Radian.new(math.pi * 2))
    assert(rad / 2 == Radian.new(math.pi / 2))
    assert(rad / 2 < rad)
    assert(rad * 2 > rad)

    assert(deg == rad)
    assert(deg == Radian.new(deg))
    assert(rad == Degree.new(rad))
end

local function TestQuaternion()
    local quat = Quaternion.new(0, 0, 1, 0)

    assert(type(quat) == "userdata", "Expected \"userdata\" - Received \"" .. type(quat) .. "\"")
    assert(quat.__type.name == Quaternion.__type.name, "Expected: " .. Quaternion.__type.name)

    assert(quat.w == 0)
    assert(quat.x == 0)
    assert(quat.y == 1)
    assert(quat.z == 0)

    assert(Quaternion.identity == Quaternion.new(1, 0, 0, 0))
    assert(Quaternion.identity.isIdentity)

    local tmp = Quaternion.identity
    tmp.w = 6

    assert(tmp ~= Quaternion.identity)
    assert(tmp == Quaternion.new(6, 0, 0, 0))
    assert(Quaternion.identity == Quaternion.new(1, 0, 0, 0))

    assert(quat == Quaternion.new(0, Vector3.new(0, 1, 0)))
    assert(quat == Quaternion.new(Degree.new(180), Vector3.up))
    assert(quat == Quaternion.new(Radian.new(math.pi), Vector3.up))
    assert(quat == Quaternion.FromEuler(Radian.new(), Radian.new(math.pi), Radian.new(), ERotationOrder.YXZ))
    assert(quat == Quaternion.FromEuler(Degree.new(), Degree.new(180), Degree.new(), ERotationOrder.YXZ))

    assert(Vector3.up:Rotated(Quaternion.FromTo(Vector3.up, Vector3.up)) == Vector3.up)
    assert(Vector3.up:Rotated(Quaternion.FromTo(Vector3.up, Vector3.down)) == Vector3.down)
    assert(Vector3.up:Rotated(Quaternion.FromTo(Vector3.up, Vector3.left)) == Vector3.left)

    local xRad, yRad, zRad = quat:ToEuler(ERotationOrder.YXZ)

    assert(xRad == Radian.new(0))
    assert(yRad == Radian.new(math.pi))
    assert(zRad == Radian.new(0))

    assert("" .. quat == tostring(quat))
    assert(quat .. "" == tostring(quat))

    local quat90deg = Quaternion.new(Degree.new(90), Vector3.up)

    assert(quat * quat.inverse == Quaternion.identity)
    assert(quat * quat90deg == Quaternion.new(Degree.new(270), Vector3.up))
    assert(Quaternion.new(1, 2, 3, 4) * 2 == Quaternion.new(2, 4, 6, 8))
    assert(2 * Quaternion.new(1, 2, 3, 4) == Quaternion.new(2, 4, 6, 8))

    assert(quat / quat90deg == quat90deg)
    assert(quat / quat90deg == quat * quat90deg.inverse)
    assert(Quaternion.new(2, 4, 6, 8) / 2 == Quaternion.new(1, 2, 3, 4))
end

local function TestMatrix()
    -- TODO: Lua matrix tests
    local testMat = Matrix4.identity
    assert(testMat.__type.name == Matrix4.__type.name, "Expected: " .. Matrix4.__type.name)
    assert(testMat == Matrix4.new(1))
    assert(testMat.isIdentity)

    testMat:Set(3, 2, 8)
    assert(not testMat.isIdentity)
    assert(testMat(3, 2) == 8)

    local radRotMat = Matrix4.Rotation(Radian.new(math.pi), Vector3.up)
    local degRotMat = Matrix4.Rotation(Degree.new(180), Vector3.up)

    assert(degRotMat == radRotMat)
end

local function TestVectors()
    -- TODO: Lua vector tests
    local zero = Vector2.zero
    local ctor = Vector2.new(5, 8)

    assert(zero.x == 0 and zero.y == 0)
    assert(ctor.x == 5 and ctor.y == 8)

    ctor.x = 6
    ctor.y = 9
    assert(ctor.x == 6 and ctor.y == 9)
    assert(Vector3.new(6, 9, 2).xy == ctor)
    assert(Vector3.new(ctor) == Vector3.new(6, 9, 0))
end

function MathTest:OnInit()
    if MATH_TEST then
        Debug.LogWarning("Math test already exists in E" .. MATH_TEST.owner .. " - Removing from E" .. self.owner)
        self.owner:RemoveScript("scripts.tests.math")
        return
    end

    MATH_TEST = self
    TestAngles()
    TestQuaternion()
    TestMatrix()
    TestVectors()

    print("= LUA MATH TESTS COMPLETED SUCCESSFULLY =")
end

return MathTest