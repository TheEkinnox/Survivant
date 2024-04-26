local Test = {
    max_tick = 0,
    test_bool = false
}

local req = {
    [1] = require("scripts.testRequired"),
    [2] = require("scripts/testRequired"),
    [3] = require("assets.scripts.testRequired"),
    [4] = require("assets.scripts/testRequired"),
    [5] = require("assets/scripts.testRequired"),
    [6] = require("assets/scripts/testRequired")
}

local tick_count = 0
local time = 0

local function TestScene(scene)
    local typeName = Scene.__type.name

    assert(scene)
    assert(type(scene) == "userdata", "Expected \"userdata\" - Received \"" .. type(scene) .. "\"")
    assert(scene.__type.name == typeName, "Expected \"" .. typeName .. "\" - Received \"" .. scene.__type.name .. "\"")

    local entity = scene:Create()

    typeName = Entity.__type.name

    assert(type(entity) == "userdata", "Expected \"userdata\" - Received \"" .. type(entity) .. "\"")
    assert(entity.__type.name == typeName, "Expected \"" .. typeName .. "\" - Received \"" .. entity.__type.name .. "\"")
    assert(scene:IsValid(entity))
    assert(scene:Contains(entity))

    local found = false

    for _, sceneEntity in ipairs(scene.entities) do
        found = found or sceneEntity == entity
    end

    assert(found)

    local tag = entity:GetOrCreate("Tag")
    tag.value = "Entity " .. entity

    local copy = scene:Create(entity)

    assert(copy)
    assert(type(copy) == "userdata", "Expected \"userdata\" - Received \"" .. type(copy) .. "\"")
    assert(copy.__type.name == typeName, "Expected \"" .. typeName .. "\" - Received \"" .. copy.__type.name .. "\"")
    assert(copy.isValid)
    assert(copy ~= entity)

    assert(copy:Get("Tag").value == tag.value)

    scene:Destroy(copy)

    assert(copy)
    assert(not copy.isValid)

    scene:Destroy(entity)
end

local function TestComponents(entity)
    assert(entity.componentCount == 1)

    local componentType = "Transform"

    assert(not entity:Has(componentType))
    assert(not entity:Get(componentType).isValid)

    local transform = entity:GetOrCreate(componentType)

    assert(transform.isValid)
    assert(transform.__type.name == Transform.__type.name)
    assert(entity:Has(componentType))
    assert(entity.componentCount == 2)

    local get = entity:Get(componentType)
    assert(get.isValid)
    assert(get == transform)

    local startPos = transform.position
    local newPos = startPos + Vector3.new(1, 2, 3)

    transform.position = newPos

    assert(transform.position ~= startPos)
    assert(transform.position == newPos)
    assert(get.position == transform.position)
end

function Test.Ping()
    return "pong"
end

local function TestScripts(entity)
    assert(not entity:HasScript("scripts.test"))
    assert(not entity:GetScript("scripts.test").isValid)

    local script = entity:AddScript("scripts.test")

    assert(script.isValid)
    assert(script.owner == entity)
    assert(entity:HasScript("scripts.test"))
    assert(script.Ping() == "pong")

    local invalid = entity:AddScript("scripts.test")
    assert(not invalid.isValid)

    local getTest = entity:GetScript("scripts.test")

    assert(getTest.isValid)
    assert(getTest == script)

    assert(not script.test_bool)
    assert(not getTest.test_bool)

    getTest.test_bool = true

    assert(getTest.test_bool)
    assert(script.test_bool)

    entity:RemoveScript("scripts.test")
    assert(not script.isValid)
end

local function TestHierarchy(entity)
    assert(not entity.parent.isValid)
    assert(entity.root == entity)
    assert(not entity.nextSibling.isValid)
    assert(not entity.previousSibling.isValid)
    assert(entity.childCount == 0)
    assert(not entity:GetChild(5).isValid)
    assert(#entity.children == 0)

    local child1 = entity:Copy()
    local child2 = entity:Copy()

    child1.parent = entity

    assert(child1.parent.isValid)
    assert(child1.parent == entity)
    assert(child1.root == entity)
    assert(entity.childCount == 1)
    assert(#entity.children == 1)
    assert(entity:GetChild(0).isValid)
    assert(entity:GetChild(0) == child1)

    child2.parent = entity

    assert(child1.previousSibling.isValid)
    assert(child1.previousSibling == child2)
    assert(not child1.nextSibling.isValid)

    assert(child2.nextSibling.isValid)
    assert(child2.nextSibling == child1)
    assert(not child2.previousSibling.isValid)
end

local function TestECS(entity)
    local typeName = Entity.__type.name

    assert(type(entity) == "userdata", "Expected \"userdata\" - Received \"" .. type(entity) .. "\"")
    assert(entity.__type.name == typeName, "Expected \"" .. typeName .. "\" - Received \"" .. entity.__type.name .. "\"")
    assert(entity.isValid)

    TestScene(entity.scene)

    local copy = entity:Copy()

    assert(copy)
    assert(type(copy) == "userdata", "Expected \"userdata\" - Received \"" .. type(copy) .. "\"")
    assert(copy.__type.name == typeName, "Expected \"" .. typeName .. "\" - Received \"" .. copy.__type.name .. "\"")
    assert(copy.isValid)
    assert(copy ~= entity)

    TestComponents(copy)

    TestScripts(copy)

    TestHierarchy(copy)

    copy:Destroy()

    assert(copy)
    assert(not copy.isValid)
end

function Test:OnInit()
    tick_count = 0

    if not testCalled then
        testCalled = true
        TestECS(self.owner)
        self.owner:AddScript("scripts.mathTest")
        self.owner:AddScript("scripts.utilityTest")

        local inputEntity = self.owner.scene:Create() -- Create a new entity since this one will be destroyed
        inputEntity:AddScript("scripts.inputTest")
    end

    if self.max_tick ~= 0 then
        print("Entity " .. self.owner .. " already has a lifespan of " .. self.max_tick .. " frames...")
    else
        self.max_tick = math.random(1, 100)
        print("Entity " .. self.owner .. " now has " .. self.max_tick .. " frames to live. Enjoy!")
    end
end

function Test:OnStart()
    print("Entity " .. self.owner .. ", reporting for duty!")
end

function Test:OnUpdate(deltaTime)
    tick_count = tick_count + 1
    time = time + deltaTime

    if tick_count == self.max_tick then
        local avgFrameTime = time / tick_count
        print("It's entity " .. self.owner .. "'s last tick!!! " ..
                "Average frame time: " .. avgFrameTime .. "s (" .. math.round(1 / avgFrameTime) .. "fps)")

        for i = 1, #req do
            print("[require " .. i .. "] " .. req[i]:SomeFunc() .. " | Passed instance count: " .. req[i].instances)
        end

        self.owner:Destroy()
    elseif tick_count == self.max_tick + 1 then
        assert(false, "Not sur I want to give the user access to that...")
    end
end

function Test:OnStop()
    print("Entity " .. self.owner .. " ready to stop")
end

function Test:OnDestroy()
    local avgFrameTime = time / tick_count
    print("Destroyed test script of entity " .. self.owner .. " after " .. tick_count .. " ticks (" .. time .. "s) | " ..
            "Average frame time: " .. avgFrameTime .. "s (" .. math.round(1 / avgFrameTime) .. "fps)")
end

return Test