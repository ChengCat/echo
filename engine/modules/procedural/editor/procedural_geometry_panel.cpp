#include "procedural_geometry_panel.h"
#include "engine/core/editor/editor.h"
#include "engine/core/editor/qt/QWidgets.h"
#include "engine/core/base/class_method_bind.h"
#include "engine/core/util/PathUtil.h"
#include "engine/core/util/StringUtil.h"
#include "engine/core/io/MemoryReader.h"
#include "engine/core/util/Buffer.h"
#include "engine/core/io/IO.h"
#include "engine/core/render/base/image/Image.h"
#include "engine/core/render/base/atla/TextureAtlas.h"
#include "engine/core/log/Log.h"

namespace Echo
{
#ifdef ECHO_EDITOR_MODE
	ProceduralGeometryPanel::ProceduralGeometryPanel(Object* obj)
	{
		m_proceduralGeometry = ECHO_DOWN_CAST<ProceduralGeometry*>(obj);

		m_ui = qobject_cast<QDockWidget*>(EditorApi.qLoadUi("engine/modules/procedural/editor/procedural_geometry_panel.ui"));

		QSplitter* splitter = (QSplitter*)EditorApi.qFindChild(m_ui, "m_splitter");
		if (splitter)
		{
			splitter->setStretchFactor(0, 0);
			splitter->setStretchFactor(1, 1);
		}

		// Tool button icons
		EditorApi.qToolButtonSetIcon(EditorApi.qFindChild(m_ui, "m_play"), "engine/modules/procedural/editor/icon/play.png");

		// connect signal slots
		EditorApi.qConnectWidget(EditorApi.qFindChild(m_ui, "m_graphicsView"), QSIGNAL(customContextMenuRequested(const QPoint&)), this, createMethodBind(&ProceduralGeometryPanel::onRightClickGraphicsView));
		EditorApi.qConnectWidget(EditorApi.qFindChild(m_ui, "m_nodeTreeWidget"), QSIGNAL(itemClicked(QTreeWidgetItem*, int)), this, createMethodBind(&ProceduralGeometryPanel::onSelectItem));
		EditorApi.qConnectWidget(EditorApi.qFindChild(m_ui, "m_nodeTreeWidget"), QSIGNAL(itemChanged(QTreeWidgetItem*, int)), this, createMethodBind(&ProceduralGeometryPanel::onChangedAtlaName));
		EditorApi.qConnectWidget(EditorApi.qFindChild(m_ui, "m_play"), QSIGNAL(clicked()), this, createMethodBind(&ProceduralGeometryPanel::onPlay));

		// create QGraphicsScene
		m_graphicsView = m_ui->findChild<QGraphicsView*>("m_graphicsView");
		m_graphicsScene = EditorApi.qGraphicsSceneNew();
		m_graphicsView->setScene(m_graphicsScene);

		// background
		m_backgroundGridSmall.set(m_graphicsView, m_graphicsScene);
		m_backgroundGridBig.set(m_graphicsView, m_graphicsScene);
	}

	ProceduralGeometryPanel::~ProceduralGeometryPanel()
	{
		clearImageItemAndBorder();

		EditorApi.removeCenterPanel(m_ui);
		delete m_ui; m_ui = nullptr;
	}

	void ProceduralGeometryPanel::update()
	{
		refreshUiDisplay();
	}

	void ProceduralGeometryPanel::onRightClickGraphicsView()
	{
		if (!m_menuNew)
		{
			m_menuNew = EchoNew(QMenu(m_ui));

			Echo::StringArray pgNodeClasses;
			Echo::Class::getChildClasses(pgNodeClasses, "PGNode", true);
			for (String& className : pgNodeClasses)
			{
				QAction* newAction = new QAction;
				newAction->setText(Echo::StringUtil::Replace(className, "PG", "").c_str());
				newAction->setData(className.c_str());
				m_menuNew->addAction(newAction);

				EditorApi.qConnectAction(newAction, QSIGNAL(triggered()), this, createMethodBind(&ProceduralGeometryPanel::onNewPGNode));
			}
		}

		QPoint  localPos = m_graphicsView->mapFromGlobal(QCursor::pos());
		QPointF scenePos = m_graphicsView->mapToScene(localPos);
		m_newPGNodePosition = Echo::Vector2(scenePos.x(), scenePos.y());

		m_menuNew->exec(QCursor::pos());
	}

	void ProceduralGeometryPanel::onNewPGNode()
	{
		QAction* action = qobject_cast<QAction*>(EditorApi.qSender());
		if(action)
		{ 
			Echo::String className = action->data().toString().toStdString().c_str();
			if (m_proceduralGeometry)
			{
				Echo::PGNode* root = m_proceduralGeometry->getPGNode();
				if (root)
				{
					Echo::PGNode* pgNode = Echo::Class::create<PGNode*>(className);
					pgNode->setPosition(m_newPGNodePosition);

					root->addChild(pgNode);
				}
			}
		}
	}

	void ProceduralGeometryPanel::onPlay()
	{
		if (m_proceduralGeometry)
		{
			m_proceduralGeometry->play();
		}
	}

	void ProceduralGeometryPanel::refreshUiDisplay()
	{
		drawBackground();

		drawNodes();
	}

	void ProceduralGeometryPanel::drawBackground()
	{
		m_backgroundStyle.m_backgroundColor.setRGBA(77, 77, 77, 255);
		m_backgroundStyle.m_fineGridColor.setRGBA(84, 84, 84, 255);
		m_backgroundStyle.m_coarseGridColor.setRGBA(64, 64, 64, 255);

		EditorApi.qGraphicsViewSetBackgroundBrush(EditorApi.qFindChild(m_ui, "m_graphicsView"), m_backgroundStyle.m_backgroundColor);

		m_backgroundGridSmall.update(15, m_backgroundStyle.m_fineGridColor);
		m_backgroundGridBig.update(150, m_backgroundStyle.m_coarseGridColor);
	}

	void ProceduralGeometryPanel::drawNodes()
	{
		const vector<PGNode*>::type& pgNodes = m_proceduralGeometry->getPGNode()->children();
		while (m_pgNodePainters.size() > pgNodes.size())
		{
			EchoSafeDelete(m_pgNodePainters.back(), PGNodePainter);
			m_pgNodePainters.pop_back();
		}

		if (m_pgNodePainters.size() < pgNodes.size())
		{
			for (size_t i = m_pgNodePainters.size(); i < pgNodes.size(); ++i)
				m_pgNodePainters.emplace_back(EchoNew(Procedural::PGNodePainter(m_graphicsView, m_graphicsScene, pgNodes[i])));
		}

		for (size_t i = 0; i < pgNodes.size(); i++)
		{
			if (!m_pgNodePainters[i] || m_pgNodePainters[i]->m_pgNode != pgNodes[i])
			{
				EchoSafeDelete(m_pgNodePainters[i], PGNodePainter);
				m_pgNodePainters[i] = EchoNew(Procedural::PGNodePainter(m_graphicsView, m_graphicsScene, pgNodes[i]));
			}
		}

		for (size_t i = 0; i < pgNodes.size(); i++)
		{
			m_pgNodePainters[i]->update();
		}
	}

	void ProceduralGeometryPanel::clearImageItemAndBorder()
	{
	}

	void ProceduralGeometryPanel::refreshImageDisplay()
	{

	}

	void ProceduralGeometryPanel::onSelectItem()
	{

	}

	void ProceduralGeometryPanel::onChangedAtlaName()
	{

	}
#endif
}
