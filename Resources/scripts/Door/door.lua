---@class Door : Script
local Door = {
    move_speed = 1.0,
    length_and_dir = 2.0,
    numButtonsToMove = 1
}

local transform = nil
local distance_percent = 0
local start_pos
local end_pos
local abs_length
---@type table<number, Button>
local buttons

function Door:OnInit()
end

function Door:OnStart()
    buttons = {}
    abs_length = math.abs(self.length_and_dir)

    for _, child in ipairs(self.owner.children) do
        local script = child:GetScript("scripts.Door.button")

        if script.isValid then
            buttons[#buttons + 1] = script
        elseif not transform and child:Get(Tag).value == "wall" then
            transform = child:GetOrCreate(Transform)
        end
    end

    if not transform then
        warn("Missing child named 'wall' in Door")
    else
        start_pos = transform.position
        end_pos = start_pos + (transform.up * self.length_and_dir)
    end
end

local function NumActiveButtons()
    local numActive = 0
    for _, script in ipairs(buttons) do
        if script.IsActive() then
            numActive = numActive + 1
        end
    end

    return numActive
end

local function GetAdditionalDistancePercent(self, deltaTime)
    return self.move_speed * deltaTime / abs_length
end

local function UpdatePosition(self, deltaTime)
    if NumActiveButtons() >= self.numButtonsToMove and distance_percent < 1 then
        distance_percent = distance_percent + GetAdditionalDistancePercent(self, deltaTime)
        
        distance_percent = distance_percent > 1 and 1 or distance_percent
        transform.position = math.lerp(start_pos, end_pos, distance_percent)

    elseif 0 < distance_percent then
        distance_percent = distance_percent - GetAdditionalDistancePercent(self, deltaTime)
        
        distance_percent = distance_percent < 0 and 0 or distance_percent
        transform.position = math.lerp(start_pos, end_pos, distance_percent)
    end
end

function Door:OnUpdate(deltaTime)
    UpdatePosition(self, deltaTime)
end

function Door:OnStop()
end

return Door