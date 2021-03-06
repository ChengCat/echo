#include "RenderPipelinePanel.h"
#include "engine/core/editor/editor.h"
#include "engine/core/editor/qt/QWidgets.h"
#include "engine/core/base/class_method_bind.h"
#include "engine/core/util/PathUtil.h"
#include "engine/core/util/StringUtil.h"

namespace Echo
{
#ifdef ECHO_EDITOR_MODE
	RenderpipelinePanel::RenderpipelinePanel(Object* obj)
	{
		m_pipeline = ECHO_DOWN_CAST<RenderPipeline*>(obj);

		m_ui = EditorApi.qLoadUi("engine/core/render/base/editor/pipeline/RenderPipelinePanel.ui");

		QSplitter* splitter = (QSplitter*)EditorApi.qFindChild(m_ui, "m_splitter");
		if (splitter)
		{
			splitter->setStretchFactor(0, 0);
			splitter->setStretchFactor(1, 1);
		}

		// Tool button icons
		EditorApi.qToolButtonSetIcon(EditorApi.qFindChild(m_ui, "m_new"), "engine/core/render/base/editor/icon/import.png");

		// connect signal slots
		EditorApi.qConnectWidget(EditorApi.qFindChild(m_ui, "m_new"), QSIGNAL(clicked()), this, createMethodBind(&RenderpipelinePanel::onNew));
	}

	void RenderpipelinePanel::update()
	{
	}

	void RenderpipelinePanel::onNew()
	{
		if (!m_importMenu)
		{
			m_importMenu = EchoNew(QMenu(m_ui));

			m_importMenu->addAction( EditorApi.qFindChildAction(m_ui, "m_actionNewCamera"));
			m_importMenu->addAction( EditorApi.qFindChildAction(m_ui, "m_actionNewImageFilter"));

			EditorApi.qConnectAction(EditorApi.qFindChildAction(m_ui, "m_actionNewCamera"), QSIGNAL(triggered()), this, createMethodBind(&RenderpipelinePanel::onNewCamera));
			EditorApi.qConnectAction(EditorApi.qFindChildAction(m_ui, "m_actionNewImageFilter"), QSIGNAL(triggered()), this, createMethodBind(&RenderpipelinePanel::onNewImageFilter));
		}

		m_importMenu->exec(QCursor::pos());
	}

	void RenderpipelinePanel::onNewCamera()
	{

	}

	void RenderpipelinePanel::onNewImageFilter()
	{

	}
#endif
}
