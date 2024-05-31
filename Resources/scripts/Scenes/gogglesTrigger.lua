---@class GogglesTrigger : Script
local GogglesTrigger = {
    ---@type Entity
    player = Entity.new()
}

local shoudlDestroy = false

---@param triggerInfo TriggerInfo
function GogglesTrigger:OnTriggerEnter(triggerInfo)    
    if triggerInfo.otherCollider.owner == self.player then
        ---@type SwitchCam
        print(self.player)
        local switchCam = self.player:GetScript("scripts.Player.switchCam")


        if switchCam.isValid then
            switchCam.UnlockSwitch()
        else
            warn("Player entity does not have script 'scripts.Player.switchCam'")
        end
        
        shoudlDestroy = true
    end
end

function GogglesTrigger:OnUpdate(deltaTime) 
    if shoudlDestroy then
        self.owner:Destroy()
    end
end   



return GogglesTrigger