#pragma once

#include "SurvivantEditor/UI/Interfaces/IPanelable.h"

#include "Transform.h"
#include "Vector/Vector3.h"
#include "Quaternion.h"

#include <functional>

namespace SvEditor::UI::PanelItems
{
	using namespace LibMath;

	class PanelTransformInput : public Interfaces::IPanelable
	{
	public:
		using Callback = std::function<void(const LibMath::Vector3* const, const LibMath::Quaternion* const, const LibMath::Vector3* const)>;

		PanelTransformInput(
			Transform& p_transform,
			const Callback& p_callback = Callback());
		~PanelTransformInput() = default;

		virtual void DisplayAndUpdatePanel() override;

	private:
		Vector3 							ToVector3Degree(const LibMath::TVector3<LibMath::Radian>& p_radians);
		LibMath::TVector3<LibMath::Radian>	ToVector3Radian(const LibMath::Vector3& p_degrees);
		//void								ResetVector3ValIfNan(LibMath::Vector3& p_value);

		Callback					m_callback;
		Transform&					m_transform;
		/*LibMath::Vector3					m_position;
		LibMath::Quaternion					m_rotation;
		LibMath::Vector3					m_scale;*/
		TVector3<LibMath::Radian>	m_yawPitchRoll;
	};
}