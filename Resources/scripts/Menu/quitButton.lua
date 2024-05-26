---@class QuitButton : Script
local QuitButton = {}

local function OnButtonTrigger(self)
    print("Implement quit here")
end

local isHovered = false

function QuitButton:OnUpdate(deltaTime)
    if isHovered and (Input.IsKeyDown(EKey.ENTER) or Input.IsMouseButtonDown(EMouseButton.LEFT)) then
        OnButtonTrigger(self)
        isHovered = false
    end
end

---@param triggerInfo TriggerInfo
function QuitButton:OnTriggerEnter(triggerInfo)
    if triggerInfo.otherCollider.owner:GetScript("scripts.Menu.cursor").isValid then
        isHovered = true
    end
end

---@param triggerInfo TriggerInfo
function QuitButton:OnTriggerExit(triggerInfo)
    if triggerInfo.otherCollider.owner:GetScript("scripts.Menu.cursor").isValid then
        isHovered = false
    end
end


return QuitButton