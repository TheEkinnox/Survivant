//LuaEditorBinder.h
#pragma once

namespace sol
{
	class state;
}

namespace SvEditor::Core
{
	class LuaEditorBinder
	{
	public:
		static void EditorUserTypeBindings(sol::state& p_luaState);

	private:
		static void EditorInputBinder(sol::state& p_luaState);
		static void EditorInputBindFunctions(sol::state& p_luaState);
	};
}
