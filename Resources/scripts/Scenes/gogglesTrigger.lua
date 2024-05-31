---@class GogglesTrigger : Script
local GogglesTrigger = {
    ---@type Entity
    player = Entity.new()
}

local shouldDestroy = false

---@param triggerInfo TriggerInfo
function GogglesTrigger:OnTriggerEnter(triggerInfo)
    if triggerInfo.otherCollider.owner == self.player then
        ---@type SwitchCam
        print(self.player)
        local switchCam = self.player:GetScriptInChildren("scripts.Player.switchCam")

        if switchCam.isValid then
            switchCam.UnlockSwitch()
        else
            warn("Player entity does not have script 'scripts.Player.switchCam'")
        end

        shouldDestroy = true
    end
end

function GogglesTrigger:OnUpdate(deltaTime)
    if shouldDestroy then
        self.owner:Destroy()
    end
end

return GogglesTrigger