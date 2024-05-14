---@class UtilityTest : Script
local UtilityTest = {}

local start_frame = 0
local start_time = 0
local start_unscaled_time = 0

local frame_count = 0
local time_scale = 1
local time = 0
local unscaled_time = 0

local function SetTimeScale(timeScale)
    time_scale = timeScale
    Time.timeScale = time_scale
end

function UtilityTest:OnInit()
    Debug.Log("[LUA TEST] debug log")
    Debug.LogWarning("[LUA TEST] warning log")
    Debug.LogError("[LUA TEST] lua error log")
end

function UtilityTest:OnStart()
    start_time = Time.time
    start_unscaled_time = Time.unscaledTime
    start_frame = Time.frameCount

    SetTimeScale(1)
end

function UtilityTest:OnUpdate(deltaTime)
    local unscaledDeltaTime = deltaTime / time_scale
    frame_count = frame_count + 1

    time = time + deltaTime
    unscaled_time = unscaled_time + unscaledDeltaTime

    assert(math.isNear(Time.time, start_time + time), "Current frame: " .. frame_count .. " | Expected " .. start_time + time .. " | Received " .. Time.time)
    assert(math.isNear(Time.deltaTime, deltaTime), "Current frame: " .. frame_count .. " | Expected " .. deltaTime .. " | Received " .. Time.deltaTime)
    assert(math.isNear(Time.unscaledTime, start_unscaled_time + unscaled_time), "Current frame: " .. frame_count .. " | Expected " .. start_unscaled_time + unscaled_time .. " | Received " .. Time.unscaledTime)
    assert(math.isNear(Time.unscaledDeltaTime, unscaledDeltaTime), "Current frame: " .. frame_count .. " | Expected " .. unscaledDeltaTime .. " | Received " .. Time.unscaledDeltaTime)
    assert(math.isNear(Time.timeScale, time_scale), "Current frame: " .. frame_count .. " | Expected " .. time_scale .. " | Received " .. Time.timeScale)
    assert(math.isNear(Time.frameCount, start_frame + frame_count), "Current frame: " .. frame_count .. " | Expected " .. start_frame + frame_count .. " | Received " .. Time.frameCount)

    if frame_count == 3 then
        SetTimeScale(.5)
    elseif frame_count == 6 then
        SetTimeScale(1)
    elseif frame_count == 9 then
        self.owner:RemoveScript("scripts.utilityTest")
    end
end

return UtilityTest