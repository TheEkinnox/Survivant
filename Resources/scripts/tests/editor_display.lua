---@class EditorDisplay : Script
local EditorDisplay = {
    _hidden = "You can't see me",
    nil_obj = nil,
    string = "str",
    int = 123456,
    double = 789.012,
    bool = true,
    table = {
        a = 1,
        [1] = "a",
        sub_table = {
            b = 2,
            [2] = "b",
            a = "used name"
        },
        empty = {},
        single_element = {
            alone = "lonely"
        }
    },
    userdata = {
        vec2 = Vector2.new(),
        vec3 = Vector3.new(),
        vec4 = Vector4.new(),
        transform = Transform.new(),
        degree = Degree.new(45),
        radian = Radian.new(Degree.new(45)),
        quaternion = Quaternion.FromEuler(Degree.new(90), Degree.new(45), Degree.new(30), ERotationOrder.YXZ),
        emptyResourceRef = Resource.new(),
        resourceType = Resource.new(),
        sceneRef = Application.currentScene,
        entity = Application.currentScene.entities[1]
    },
    "no key"
}

return EditorDisplay