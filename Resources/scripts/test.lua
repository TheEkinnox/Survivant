local Test = {
    m_maxTick = 0
}

local testRequires = {
    [1] = require("testRequired"),
    [2] = require("scripts.testRequired"),
    [3] = require("scripts/testRequired"),
    [4] = require("assets.scripts.testRequired"),
    [5] = require("assets.scripts/testRequired"),
    [6] = require("assets/scripts.testRequired"),
    [7] = require("assets/scripts/testRequired")
}

local tickCount = 0
local time = 0

local function Round(p_value)
    return p_value >= 0 and math.floor(p_value + 0.5) or math.ceil(p_value - 0.5)
end

function Test:OnInit()
    tickCount = 0
    self.m_maxTick = math.random(1, 100)
    print("Entity " .. self.owner .. " now has " .. self.m_maxTick .. " frames to live. Enjoy!")
end

function Test:OnStart()
    print("Entity " .. self.owner .. ", reporting for duty!")
end

function Test:OnUpdate(p_deltaTime)
    tickCount = tickCount + 1
    time = time + p_deltaTime

    if tickCount == self.m_maxTick then
        local avgFrameTime = time / tickCount
        print("It's entity " .. self.owner .. "'s last tick!!!" ..
                "Average frame time: " .. avgFrameTime .. "s (" .. Round(1 / avgFrameTime) .. "fps)")

        for i = 1, #testRequires do
            print(testRequires[i]:SomeFunc() .. " | Passed instance count: " .. testRequires[i].m_instances)
        end
    end
end

function Test:OnStop()
    print("Entity " .. self.owner .. " ready to stop")
end

function Test:OnDestroy()
    local avgFrameTime = time / tickCount
    print("Destroyed test script of entity " .. self.owner .. " after " .. tickCount .. " ticks (" .. time .. "s) | " ..
            "Average frame time: " .. avgFrameTime .. "s (" .. Round(1 / avgFrameTime) .. "fps)")
end

return Test