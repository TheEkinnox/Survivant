---@class AudioTest : Script
local AudioTest = {
    sound = Resource.new("AudioClip", "")
}

local wasDown = false
local wasUp = false

function AudioTest:OnStart()
    Audio:Play(self.sound)

    wasDown = Input.IsKeyDown(EKey.SPACE)
    wasUp = Input.IsKeyUp(EKey.SPACE)
end

function AudioTest:OnUpdate()
    local isDown = Input.IsKeyDown(EKey.SPACE)
    local isUp = Input.IsKeyUp(EKey.SPACE)

    if isDown and not wasDown then
        print("Pressed")
        Audio:Play(self.sound)
    end

    wasDown = isDown
    wasUp = isUp
end

return AudioTest