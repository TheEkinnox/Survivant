//PanelTextBox.h
#pragma once

#include "SurvivantEditor/Interfaces/IPanelable.h"
#include "SurvivantEditor/Interfaces/ITextable.h"

#include <functional>
#include <unordered_set>
#include <vector>
#include <map>
#include <memory>

namespace SvEditor::PanelItems
{
	using namespace Interfaces;

	class PanelTextBox : public IPanelable
	{
	public:
		using FilterSet = std::unordered_set<std::string>;

		PanelTextBox() = default;
		~PanelTextBox() = default;

		void DisplayAndUpdatePanel()override;

		/// <summary>
		/// Set filters for the text box, optimized for same lenght filters
		/// </summary>
		/// <param name="p_filters">display strings that start with at least a filter</param>
		void SetFilters(const FilterSet& p_filters);
		void AddFilter(const std::string& p_filter);
		void RemoveFilter(const std::string& p_filter);

		/// <summary>
		/// Adds filter if dosent contain, remove it if contains
		/// </summary>
		/// <param name="p_filter"></param>
		void AddOrRemoveFilter(const std::string& p_filter);

		void AddItem(const std::shared_ptr<ITextable>& item, bool p_willScrollToBottom = false);
		void Clear();
		void Copy();

	private:
		bool IsFilteredItem(const ITextable* p_item);
		void UpdateFilteredItems();

		std::vector<std::shared_ptr<ITextable>>		m_items;
		std::vector<std::shared_ptr<ITextable>>		m_filteredItems;
		std::map<size_t, FilterSet>					m_filters;
		bool										m_scrollToBottom = false;
		bool										m_copy = false;
	};

	class PanelTextDisplay : public IPanelable
	{
	public:
		class DefaultText : public ITextable
		{
		public:
			DefaultText(const std::string& p_string);
			~DefaultText() = default;

			// Inherited via ITextable
			void DisplayAndUpdatePanel() override;

			std::string			GetString(size_t p_len) const override;
			const std::string&	GetString() override;
			size_t				GetLength() const override;

		private:
			std::string m_string;
		};

		PanelTextDisplay(const std::shared_ptr<ITextable>& p_item);
		~PanelTextDisplay() = default;

		void DisplayAndUpdatePanel()override;

	private:
		std::shared_ptr<ITextable> m_item;
	};
}