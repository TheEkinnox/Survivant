//PanelTextBox.h
#pragma once

#include "SurvivantUI/Interfaces/IPanelable.h"
#include "SurvivantUI/Interfaces/ITextable.h"

#include <functional>
#include <unordered_set>
#include <vector>
#include <map>
#include <memory>

namespace SvUI::PanelItems
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
}