---@class SceneButton : Script
local SceneButton = {
    scenePath = "",
    sceneKey = EKey.S
}

local function OnButtonTrigger(self)
    Application:ChangeScene(self.scenePath)
end

local isHovered = false

function SceneButton:OnUpdate(deltaTime)
    if Input.IsKeyDown(self.sceneKey) or (isHovered and Input.IsMouseButtonDown(EMouseButton.LEFT)) then
        OnButtonTrigger(self)
        isHovered = false
    end
end

---@param triggerInfo TriggerInfo
function SceneButton:OnTriggerEnter(triggerInfo)
    if triggerInfo.otherCollider.owner:GetScript("scripts.Menu.cursor").isValid then
        isHovered = true
    end
end

---@param triggerInfo TriggerInfo
function SceneButton:OnTriggerExit(triggerInfo)
    if triggerInfo.otherCollider.owner:GetScript("scripts.Menu.cursor").isValid then
        isHovered = false
    end
end


return SceneButton