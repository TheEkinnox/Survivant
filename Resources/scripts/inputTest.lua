local InputTest = {}

local key_up_time = 0
local key_up_frames = 0

local mouse_up_time = 0
local mouse_up_frames = 0

function InputTest:OnInit()
end

function InputTest:OnStart()
    print("mousePos: " .. Input.mousePos)
end

function InputTest:OnUpdate(deltaTime)
    if Input.IsKeyUp(EKey.SPACE) then
        key_up_time = key_up_time + deltaTime
        key_up_frames = key_up_frames + 1
    elseif Input.IsKeyDown(EKey.SPACE) then
        if key_up_frames > 0 then
            print("Space pressed after " .. key_up_time .. "s (" .. key_up_frames .. " frames)")

            key_up_time = 0
            key_up_frames = 0
        end
    else
        assert(false, "Key is considered neither up nor down")
    end

    if Input.IsMouseButtonUp(EMouseButton.LEFT) then
        mouse_up_time = mouse_up_time + deltaTime
        mouse_up_frames = mouse_up_frames + 1
    elseif Input.IsMouseButtonDown(EMouseButton.LEFT) then
        if mouse_up_frames > 0 then
            print("Mouse clicked at " .. Input.mousePos .. " after " .. mouse_up_time .. "s (" .. mouse_up_frames .. " frames)")

            mouse_up_time = 0
            mouse_up_frames = 0
        end
    else
        assert(false, "Mouse button is considered neither up nor down")
    end
end

return InputTest