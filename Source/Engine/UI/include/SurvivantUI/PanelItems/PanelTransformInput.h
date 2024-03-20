#pragma once

#include "SurvivantUI/Interfaces/IPanelable.h"

#include "Vector/Vector3.h"
#include "Quaternion.h"

#include <functional>

namespace SvUI::PanelItems
{
	class PanelTransformInput : public Interfaces::IPanelable
	{
	public:
		using Callback = std::function<void(const LibMath::Vector3* const, const LibMath::Quaternion* const, const LibMath::Vector3* const)>;

		PanelTransformInput(
			const LibMath::Vector3& p_position = LibMath::Vector3(),
			const LibMath::Quaternion& p_rotation = LibMath::Quaternion(),
			const LibMath::Vector3& p_scale = LibMath::Vector3(),
			const Callback& p_callback = Callback());
		~PanelTransformInput() = default;

		virtual void DisplayAndUpdatePanel() override;

	private:
		LibMath::Vector3 					ToVector3Degree(const LibMath::TVector3<LibMath::Radian>& p_radians);
		LibMath::TVector3<LibMath::Radian>	ToVector3Radian(const LibMath::Vector3& p_degrees);
		//void								ResetVector3ValIfNan(LibMath::Vector3& p_value);

		Callback							m_callback;
		/*LibMath::Vector3					m_position;
		LibMath::Quaternion					m_rotation;
		LibMath::Vector3					m_scale;*/
		LibMath::TVector3<LibMath::Radian>	m_yawPitchRoll;
	};
}