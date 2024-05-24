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
		static void EditorImputBinder(sol::state& p_luaState);
		static void EditorImputBindFunctions(sol::state& p_luaState);
	};
}
