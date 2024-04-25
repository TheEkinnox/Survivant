//PanelInputBase.h
#pragma once

#include "SurvivantEditor/Interfaces/IPanelable.h"
#include "SurvivantCore/Utility/TypeTraits.h"

#include <memory>
#include <functional>

namespace SvEditor::PanelItems
{
	template <class T, typename ...Params>
	class PanelInputBase : public Interfaces::IPanelable
	{
	public:
		using Value =				T;
		using GetRefFunc =			std::function<T&()>;	
		using GetCopyFunc =			std::function<T()>;	
		using CallbackParams =		std::conditional_t<SvCore::Utility::IsEmpty<Params...>,
			const T&, const std::tuple<Params...>&>;
		//using CopyValue = std::conditional_t<SvCore::Utility::IsEmpty<Params...>,
		//	T, std::tuple<Params...>>;
		using Callback =			std::function<void(CallbackParams)>;

		PanelInputBase(const GetRefFunc& p_getRef, const Callback& p_callback = nullptr);
		PanelInputBase(const GetCopyFunc& p_getCopy, const Callback& p_callback);
		~PanelInputBase() = default;

		virtual void	DisplayAndUpdatePanel() override = 0;

	protected:
		Value&			GetRef();

		Callback	m_callback;

	private:
		GetRefFunc	m_getRef;
	};

	template<class T, typename ...Params >
	inline PanelInputBase<T, Params...>::PanelInputBase(const GetRefFunc& p_getRef, const Callback& p_callback) :
		m_getRef(p_getRef), m_callback(p_callback)
	{}

	template<class T, typename ...Params>
	inline PanelInputBase<T, Params...>::PanelInputBase(const GetCopyFunc& p_getCopy, const Callback& p_callback) :
		m_callback(p_callback)
	{
		m_getRef = [p_getCopy]() -> T&
			{
				static T val;
				val = p_getCopy();
				return val;
			};
	}

	template<class T, typename ...Params >
	inline T& PanelInputBase<T, Params...>::GetRef()
	{
		return m_getRef();
	}
}