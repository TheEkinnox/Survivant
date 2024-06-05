---@class Door : Script
local Door = {
    move_speed = 1.0,
    length_and_dir = 2.0,
    numButtonsToMove = 1,
    on_move_start_sound = Resource.new("AudioClip", "sounds/hydraulics-start.wav"),
    on_move_sound = Resource.new("AudioClip", "sounds/hydraulics-loop.wav"),
    on_move_end_sound = Resource.new("AudioClip", "sounds/hydraulics-end.wav")
}

local transform = nil
local distance_percent = 0
local start_pos
local end_pos
local abs_length
local move_loop_handle
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

local function StartAudioLoop(self)
    if self.on_move_start_sound.isValid then
        Audio:Play(self.on_move_start_sound)
    end

    if self.on_move_sound.isValid then
        move_loop_handle = Audio:Play(self.on_move_sound)
        move_loop_handle.isLooping = true
    else
        move_loop_handle = AudioHandle.new()
    end
end

local function EndAudioLoop(self)
    if move_loop_handle then
        move_loop_handle:Stop()
        move_loop_handle = nil
    end

    if self.on_move_end_sound.isValid then
        Audio:Play(self.on_move_end_sound)
    end
end

local function UpdatePosition(self, deltaTime)
    local canMove = NumActiveButtons() >= self.numButtonsToMove

    if canMove and distance_percent < 1 then
        distance_percent = distance_percent + GetAdditionalDistancePercent(self, deltaTime)

        if distance_percent >= 1 then
            EndAudioLoop(self)
            distance_percent = 1
        elseif not move_loop_handle then
            StartAudioLoop(self)
        end

        transform.position = math.lerp(start_pos, end_pos, distance_percent)

    elseif not canMove and 0 < distance_percent then
        distance_percent = distance_percent - GetAdditionalDistancePercent(self, deltaTime)

        if distance_percent <= 0 then
            EndAudioLoop(self)
            distance_percent = 0
        elseif not move_loop_handle then
            StartAudioLoop(self)
        end

        transform.position = math.lerp(start_pos, end_pos, distance_percent)
    end
end

function Door:OnUpdate(deltaTime)
    UpdatePosition(self, deltaTime)
end

function Door:OnStop()
end

return Door