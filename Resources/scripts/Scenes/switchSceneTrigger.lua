---@class SwitchSceneTrigger : Script
local SwitchSceneTrigger = {
    scenePath = ""
}

local is_triggerred = false

---@param triggerInfo TriggerInfo
function SwitchSceneTrigger:OnTriggerEnter(triggerInfo)
    local other = triggerInfo.otherCollider.owner:Get(Tag);
    
    print(other.value)
    if other.isValid and other.value == "player" then
        is_triggerred = true
    end
end

function SwitchSceneTrigger:OnUpdate()
    if is_triggerred then
        Application:ChangeScene(self.scenePath)
        is_triggerred = false
    end
end

return SwitchSceneTrigger