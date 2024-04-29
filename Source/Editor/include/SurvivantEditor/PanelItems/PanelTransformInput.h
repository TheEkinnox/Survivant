#pragma once

#include "SurvivantEditor/Interfaces/IPanelable.h"
#include "SurvivantEditor/PanelItems/PanelInputBase.h"

#include "Transform.h"
#include "Vector/Vector3.h"
#include "Quaternion.h"

#include <functional>

namespace SvEditor::PanelItems
{
	using namespace LibMath;


	using BasePanelTransformInput = PanelInputBase<Transform,
		const LibMath::Vector3* const,
		const LibMath::Quaternion* const,
		const LibMath::Vector3* const>;

	class PanelTransformInput : public BasePanelTransformInput
	{
	public:
		using TransformCallback = BasePanelTransformInput::Callback;

		PanelTransformInput(
			const GetRefFunc& p_getRef,
			const TransformCallback& p_callback = nullptr);
		PanelTransformInput(
			const GetCopyFunc& p_getCopy,
			const TransformCallback& p_callback);
		~PanelTransformInput() = default;

		virtual void DisplayAndUpdatePanel() override;

	private:
		Vector3 							ToVector3Degree(const LibMath::TVector3<LibMath::Radian>& p_radians);
		LibMath::TVector3<LibMath::Radian>	ToVector3Radian(const LibMath::Vector3& p_degrees);
		//void								ResetVector3ValIfNan(LibMath::Vector3& p_value);

		TransformCallback					m_callback;
		TVector3<LibMath::Radian>			m_yawPitchRoll;

		/*LibMath::Vector3					m_position;
		LibMath::Quaternion					m_rotation;
		LibMath::Vector3					m_scale;*/
	};
}