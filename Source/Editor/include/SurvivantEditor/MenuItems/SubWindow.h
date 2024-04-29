////Panel.h
//#pragma once
//
//#include "IMenuable.h"
//#include "IPanelable.h"
//#include "Panel.h"
//#include "Menu.h"
//
//#include "SurvivantApp/Inputs/InputManager.h"
//#include "SurvivantCore/Utility/UnusedIdGenerator.h"
//#include "SurvivantEditor/UI.h"
//
//#include "Vector/Vector3.h"
//#include "Quaternion.h"
//
//#include <ctime>
//#include <functional>
//#include <map>
//#include <string>
//#include <unordered_set>
//#include <filesystem>
//
//
////foward declaration
//struct ImGuiInputTextCallbackData;
//struct ImVec4;
//struct ImFont;
//
//namespace UI
//{
//
//
//	class PanelButton : public IPanelable
//	{
//	public:
//		PanelButton(const std::string& p_name, const std::function<void()>& p_callback = std::function<void()>()) :
//			m_name(p_name), m_callback(p_callback) 
//		{}
//		~PanelButton() = default;
//
//		void DisplayAndUpdatePanel()override;
//
//		void AddCallback(const std::function<void()>& p_callback)
//		{
//			m_callback = p_callback;
//		}
//
//	private:
//		std::string	m_name;
//		std::function<void()> m_callback;
//	};
//
//	class PanelButtonList : IPanelable
//	{
//	public:
//		PanelButtonList() = default;
//		~PanelButtonList() = default;
//
//		void DisplayAndUpdatePanel()override;
//
//		std::vector<PanelButton> m_buttons;
//	};
//
//	class PanelTextInput : public IPanelable
//	{
//	public:
//		PanelTextInput(std::string p_name, std::function<void(PanelTextInput&)> p_callback);
//		~PanelTextInput() = default;
//
//		void DisplayAndUpdatePanel()override;
//		void Clear();
//		const std::string& GetText()const;
//
//		static void ClearPanelText(PanelTextInput& p_panel);
//
//	private:
//		//static int TextPanelCallBack(ImGuiInputTextCallbackData* data);
//
//		std::string								m_name;
//		std::string								m_buffer;
//		std::function<void(PanelTextInput&)>	m_callback;
//	};
//
//
//	class PanelTextBox : public IPanelable
//	{
//	public:
//		using FilterSet = std::unordered_set<std::string>;
//
//		PanelTextBox() = default;
//		~PanelTextBox() = default;
//
//		void DisplayAndUpdatePanel()override;
//
//		/// <summary>
//		/// Set filters for the text box, optimized for same lenght filters
//		/// </summary>
//		/// <param name="p_filters">display strings that start with at least a filter</param>
//		void SetFilters(const FilterSet& p_filters);
//		void AddFilter(const std::string& p_filter);
//		void RemoveFilter(const std::string& p_filter);
//
//		/// <summary>
//		/// Adds filter if dosent contain, remove it if contains
//		/// </summary>
//		/// <param name="p_filter"></param>
//		void AddOrRemoveFilter(const std::string& p_filter);
//
//		void AddItem(const std::shared_ptr<ITextable>& item, bool p_willScrollToBottom = false);
//		void Clear();
//		void Copy();
//
//	private:
//		bool IsFilteredItem(const ITextable* p_item);
//		void UpdateFilteredItems();
//
//		std::vector<std::shared_ptr<ITextable>>		m_items;
//		std::vector<std::shared_ptr<ITextable>>		m_filteredItems;
//		std::map<size_t, FilterSet>					m_filters;
//		bool										m_scrollToBottom = false;
//		bool										m_copy = false;
//	};
//
//	class PanelUniqueSelection : public IPanelable
//	{
//	public:
//		using TransformCallback = std::function<void(int)>;
//
//		/// <summary>
//		/// 
//		/// </summary>
//		/// <param name="p_selectable">Displayed options</param>
//		/// <param name="p_callback">callback with all selected flags as a parameter</param>
//		PanelUniqueSelection(
//			const std::string& p_name,
//			const std::vector<std::string>& p_selectable, 
//			const TransformCallback& p_callback);
//		~PanelUniqueSelection() = default;
//
//		virtual void DisplayAndUpdatePanel() override;
//
//	private:
//		std::string		m_name;
//		std::string		m_items;
//		int				m_count;
//		int				m_curentSelection;
//		TransformCallback		m_callback;
//	};
//
//	class PanelMultipleSelection : public IPanelable
//	{
//	public:
//		using TransformCallback = std::function<void(int)>;
//
//		/// <summary>
//		/// 
//		/// </summary>
//		/// <param name="p_selectable">Displayed options</param>
//		/// <param name="p_callback">callback with the new selected index as parameter</param>
//		PanelMultipleSelection(
//			const std::string& p_name,
//			const std::vector<std::string>& p_selectable, 
//			const TransformCallback& p_callback);
//		~PanelMultipleSelection() = default;
//
//		virtual void DisplayAndUpdatePanel() override;
//
//	private:
//		std::string GetDisplayString();
//
//		std::string					m_name;
//		std::vector<std::string>	m_items;
//		int							m_count;
//		int							m_curentSelection;
//		TransformCallback					m_callback;
//	};
//
//	class PanelFloatInput : public IPanelable
//	{
//	public:
//		using TransformCallback = std::function<void(float)>;
//
//		PanelFloatInput(
//			const std::string& p_name,
//			const float& p_value,
//			const TransformCallback& p_callback);
//		~PanelFloatInput() = default;
//
//		virtual void DisplayAndUpdatePanel() override;
//
//	private:
//		std::string		m_name;
//		TransformCallback		m_callback;
//		float			m_value;
//	};
//
//	class PanelIntInput : public IPanelable
//	{
//	public:
//		using TransformCallback = std::function<void(int)>;
//
//		PanelIntInput(
//			const std::string& p_name,
//			int p_value,
//			const TransformCallback& p_callback);
//		~PanelIntInput() = default;
//
//		virtual void DisplayAndUpdatePanel() override;
//
//	private:
//		std::string		m_name;
//		TransformCallback		m_callback;
//		int				m_value;
//	};
//
//	class PanelVec3Input : public IPanelable
//	{
//	public:
//		using TransformCallback = std::function<void(LibMath::Vector3)>;
//
//		PanelVec3Input(
//			const std::string& p_name,
//			const LibMath::Vector3& p_value,
//			const TransformCallback& p_callback);
//		~PanelVec3Input() = default;
//
//		virtual void DisplayAndUpdatePanel() override;
//
//	private:
//		std::string			m_name;
//		TransformCallback			m_callback;
//		LibMath::Vector3	m_value;
//
//	};
//
//	class PanelTransformInput : public IPanelable
//	{
//	public:
//		using TransformCallback = std::function<void(const LibMath::Vector3 * const, const LibMath::Quaternion * const, const LibMath::Vector3 * const)>;
//
//		PanelTransformInput(
//			const LibMath::Vector3&		p_position = LibMath::Vector3(),
//			const LibMath::Quaternion&	p_rotation = LibMath::Quaternion(),
//			const LibMath::Vector3&		p_scale = LibMath::Vector3(),
//			const TransformCallback& p_callback = TransformCallback());
//		~PanelTransformInput() = default;
//
//		virtual void DisplayAndUpdatePanel() override;
//
//	private:
//		LibMath::Vector3 					ToVector3Degree(const LibMath::TVector3<LibMath::Radian>& p_radians);
//		LibMath::TVector3<LibMath::Radian>	ToVector3Radian(const LibMath::Vector3& p_degrees);
//
//		TransformCallback							m_callback;
//		LibMath::Vector3					m_position;
//		LibMath::Quaternion					m_rotation;
//		LibMath::Vector3					m_scale;
//		LibMath::TVector3<LibMath::Radian>	m_yawPitchRoll;
//	};
//
//	class ISelectionBoxable
//	{
//	public:
//		virtual ~ISelectionBoxable() = default;
//
//		/// <summary>
//		/// 
//		/// </summary>
//		/// <param name="p_width"></param>
//		/// <param name="p_height"></param>
//		/// <param name="p_doubleClicked"></param>
//		/// <returns>true if need to stop displaying other SelectionBoxable</returns>
//		virtual bool DisplayAndUpdateSelection(float& p_width, float& p_height, bool p_doubleClicked) = 0;
//		virtual const std::string& GetName() = 0;
//	};
//
//	class PanelSelectionBox : public IPanelable
//	{
//	public:
//		PanelSelectionBox() = default;
//		~PanelSelectionBox() = default;
//
//		void DisplayAndUpdatePanel() override;
//
//		void SetSelectionSize(float p_width, float p_height);
//		void SetSelectionBoxable(const std::vector<std::shared_ptr<ISelectionBoxable>>& p_selectionBoxable);
//
//		static void DisplayCenteredText(const std::string& p_text, float p_maxWidth);
//	private:
//		std::vector<std::shared_ptr<ISelectionBoxable>>		m_elements;
//		std::set<ISelectionBoxable*>						m_currentSelection;
//		float												m_width = 80;
//		float												m_height = 120;
//	};
//	
//	class PanelTreeBranch : public IPanelable, public ISelectionBoxable
//	{
//	public:
//		using OnOpenCallback = std::function<bool(PanelTreeBranch&)>;
//		using Childreen = std::unordered_map<std::string, std::shared_ptr<PanelTreeBranch>>;
//
//		PanelTreeBranch(const std::string& p_name, bool p_hideLeafs = true);
//		PanelTreeBranch(const std::string& p_name, const Childreen& p_branches, bool p_hideLeafs = true);
//		~PanelTreeBranch() = default;
//
//		//IPanelable
//		void DisplayAndUpdatePanel() override;
//
//		//ISelectionBoxable
//		bool				DisplayAndUpdateSelection(float& p_width, float& p_height, bool p_doubleClicked) override;
//		const std::string&	GetName() override;
//
//		void DisplayTreePanel();
//
//		bool				IsBranch()const;
//		const Childreen&	GetChildreen()const;
//		std::string			GetPathName()const;
//
//		Childreen&	SetBranches(const Childreen& p_branches);
//		Childreen&	SetBranches(const std::set<std::shared_ptr<PanelTreeBranch>>& p_branches);
//		void		AddBranch(const std::shared_ptr<PanelTreeBranch>& p_branch);
//		void		RemoveBranch(const std::string& p_name);
//		void		ForceOpenParents(bool p_openSelf = false);
//		void		ForceCloseChildreen(bool p_closeSelf = false);
//		void		ForceOpenAll();
//
//		void		SetOnClickCallback(const std::shared_ptr<OnOpenCallback>& p_callback);
//		void		SetAllOnClickCallback(const std::shared_ptr<OnOpenCallback>& p_callback);
//		void		SetAllBranchesOnClickCallback(const std::shared_ptr<OnOpenCallback>& p_callback);
//		void		SetAllLeavesOnClickCallback(const std::shared_ptr<OnOpenCallback>& p_callback);
//
//	private:
//		enum class EForceState
//		{
//			NOTHING,
//			FORCE_OPEN,
//			FORCE_CLOSE,
//		};
//
//		bool								m_hideLeafs;
//		std::string							m_name;
//		PanelTreeBranch*					m_parent;
//		Childreen							m_childreen;
//		EForceState							m_forceState;
//		std::shared_ptr<OnOpenCallback>		m_callback;
//	};
//
//
//	class ImagePanel : public Panel
//	{ 
//	public:
//		ImagePanel(const std::string& p_name, intptr_t p_textureId) : Panel(p_name), m_textureId(p_textureId){}
//		~ImagePanel() = default;
//
//		ERenderFlags Render() override;
//
//	private:
//		intptr_t m_textureId;
//	};
//
//	class TestPanel : public Panel
//	{
//	public:
//		TestPanel(const std::string& p_name);
//		~TestPanel() = default;
//
//		ERenderFlags Render() override;
//
//		PanelUniqueSelection								m_unique;
//		PanelMultipleSelection								m_multiple;
//	};
//
//	class MainPanel : public Panel
//	{ 
//	public:
//		using ChangeLayout = std::function<void(int)>;
//
//		MainPanel();
//		MainPanel(MenuBar&& p_menuBar);
//		~MainPanel() = default;
//
//		ERenderFlags Render() override;
//
//		void SetMenuBar(MenuBar&& p_menuBar);
//		void ChangePanelLayout(const ChangeLayout& p_layout);
//
//	private:
//		void SetupLayout(int p_dockspaceId);
//
//		static constexpr char NAME[] = "Main";
//
//		ChangeLayout		m_layout;
//		MenuBar				m_menuBar;
//		Panel::ERenderFlags m_panelFlags;
//	};
//
//	class ConsolePanel : public Panel
//	{
//	public:
//		enum class ELogType
//		{
//			DEFAULT_LOG	=	1 << 0,
//			DEBUG_LOG =		1 << 1,
//			WARNING_LOG =	1 << 2,
//			ERROR_LOG =		1 << 3,
//			//COMMAND_LOG	=	1 << 4
//		};
//
//		struct LogInfo
//		{
//			ELogType m_type;
//			std::string m_message;
//		};
//
//		ConsolePanel();
//		~ConsolePanel();
//
//		ERenderFlags Render() override;
//
//		void AddConsoleLog(const LogInfo& p_log);
//
//	private:
//		class LogText : public ITextable
//		{
//		public:
//			LogText(const LogInfo& p_logInfo);
//			~LogText() = default;
//
//			void DisplayAndUpdatePanel()override;
//			std::string GetString(size_t p_len = 0)const override;
//			size_t GetLength()const override;
//
//		private:
//			static constexpr char SPACER[] = "";
//
//			LogInfo m_logInfo;
//			size_t m_length;
//		};
//
//		static std::string LogTypeToString(ELogType p_type);
//
//		/// <summary>
//		/// returns a color associated with the LogType, or nullptr if there are none
//		/// </summary>
//		/// <param name="p_type"></param>
//		/// <param name="p_color">[out]</param>
//		static void LogTypeColor(ELogType p_type, ImVec4* p_color);
//
//		void TextInputCallback(PanelTextInput& p_textInput);
//
//		static constexpr char NAME[] = "DebugLog";
//		static inline int s_panelCount = 0;
//
//		PanelTextInput							m_input;
//		PanelTextBox							m_textBox;
//		PanelButtonList							m_buttons;
//		Menu									m_filterMenu;
//		//cant use vector<bool> bcs not container
//		std::vector<unsigned char>				m_currentFilters;
//		EditorUI::DebugEvent::ListenerId		m_eventHandle;
//	};
//
//	class SavePanel : public Panel
//	{
//	public:
//		SavePanel();
//		~SavePanel(); 
//
//		ERenderFlags Render() override;
//
//		static int GetPanelCount() { return s_panelCount; };
//
//	private:
//		static constexpr char NAME[] = "Save";
//		
//		static inline int s_panelCount = 0;
//
//		PanelButtonList m_options;
//		bool			m_open = true;
//	};
//
//	class ContentDrawerPanel : public Panel
//	{
//	public:
//		ContentDrawerPanel();
//		~ContentDrawerPanel();
//
//		//Panel
//		ERenderFlags Render() override;
//
//		bool SetGridDisplay(PanelTreeBranch& p_branch);
//		bool TryOpenFile(PanelTreeBranch& p_branch);
//
//		static size_t GetPanelCount() { return s_idGenerator.GetNumUsedIds(); };
//
//	private:
//		void SetupTree();
//		void SetupBranches(std::shared_ptr<PanelTreeBranch> p_parent, const std::filesystem::path& p_filePath);
//
//		static constexpr char NAME[] =				"ContentDrawer";
//		static constexpr char DIRECTORY_PATH[] =	"Source";
//		static constexpr char SLASH[] =			"/";
//
//		static inline SvCore::Utility::UnusedIdGenerator s_idGenerator;
//
//		std::shared_ptr<PanelTreeBranch>					m_tree;
//		PanelSelectionBox									m_grid;
//	};
//
//	class ExamplResource :public IPanelable
//	{
//	public:
//		ExamplResource() = default;
//		~ExamplResource() = default;
//
//		void DisplayAndUpdatePanel() override;
//
//	protected:
//		std::string m_name = "None";
//		std::string m_resourceLogo = "Tmp";
//		std::string s_resourceName = "Default";
//	//private:
//	//	static void OpenCallback();
//	};
//
//	class ExamplGameObj : public ExamplResource
//	{
//	public:
//		ExamplGameObj();
//		~ExamplGameObj() = default;
//
//		void DisplayAndUpdatePanel() override;
//		//void AddComponent();
//
//	private:
//		static void AddComponentCallback(ExamplGameObj* p_this);
//
//		struct ExampleComponent
//		{
//			std::string					m_name;
//			std::shared_ptr<IPanelable> m_panelable;
//		};
//
//		//components
//		std::vector<ExampleComponent> m_defaultComponents;
//		std::vector<ExampleComponent> m_aditionalComponents;
//
//		PanelButton m_addComponentButton;
//	};
//
//	class InspectorPanel : public Panel
//	{
//	public:
//		InspectorPanel();
//		~InspectorPanel();
//
//		//TODO : change to resource
//		void SetInpectorInfo(const std::shared_ptr<IPanelable>& p_info);
//		void ClearInfo();
//
//		virtual ERenderFlags Render()override;
//
//	private:
//		static constexpr char NAME[] = "Inspector";
//		static inline SvCore::Utility::UnusedIdGenerator s_idGenerator;
//
//		std::shared_ptr<IPanelable> m_items;
//
//	};
//}
