---@class SwitchCam : Script
local SwitchCam = {
    otherCamera = Entity.new()
}

local playerCamera
local otherCamera
local holding_switch

function SwitchCam:OnInit()
end

function SwitchCam:OnStart()
    playerCamera = self.owner:GetOrCreate(Camera)
    otherCamera = self.otherCamera:GetOrCreate(Camera)

    playerCamera.isActive = true
    otherCamera.isActive = false
end

function SwitchCam.IsPlayerCam()
    return playerCamera.isActive
end

local function UpdateKeyboardSwitch(self)
    local switchkey = Input.IsKeyDown(EKey.TAB)
    
    if not holding_switch and switchkey then
        playerCamera.isActive = not playerCamera.isActive
        otherCamera.isActive = not otherCamera.isActive
    end

    holding_switch = switchkey
end

function SwitchCam:OnUpdate(deltaTime)
    UpdateKeyboardSwitch(self)
end

function SwitchCam:OnStop()
end

return SwitchCam