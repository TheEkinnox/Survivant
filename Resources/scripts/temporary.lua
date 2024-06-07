local Temporary = {
    lifetime_sec = 5
}

local remaining_time

function Temporary:OnStart()
    remaining_time = self.lifetime_sec
end

function Temporary:OnUpdate(deltaTime)
    remaining_time = remaining_time - deltaTime

    if remaining_time <= 0 then
        self.owner:Destroy()
    end
end

return Temporary