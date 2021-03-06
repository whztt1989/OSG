#include <osg/GL>
#include <osg/Group>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/Viewport>
#include <osg/Texture2D>
#include <osg/MatrixTransform>
#include <osg/FrameBufferObject>
#include <osgViewer/Viewer>

#include <osgGA/CameraManipulator>
#include <osgGA/TrackballManipulator>
#include <osgDB/ReadFile>

#define SCREEN_WIDTH  1024
#define SCREEN_HEIGHT 768

osg::ref_ptr<osg::Texture2D> g_Tex;

//***********************************************************
//FUNCTION:
osg::ref_ptr<osg::Node> createQuadNode()
{
	osg::ref_ptr<osg::Geode> Geode = new osg::Geode();

	osg::ref_ptr<osg::Geometry> Geom = new osg::Geometry();

	//设置顶点
	osg::ref_ptr<osg::Vec3Array> vc= new osg::Vec3Array();
	vc->push_back(osg::Vec3(-1.0f,0.0f,-1.0f));
	vc->push_back(osg::Vec3(1.0f,0.0f,-1.0f));
	vc->push_back(osg::Vec3(1.0f,0.0f,1.0f));
	vc->push_back(osg::Vec3(-1.0f,0.0f,1.0f));
	Geom->setVertexArray(vc.get());


// 	osg::ref_ptr<osg::Vec4Array> VertexColor = new osg::Vec4Array();
// 	VertexColor->push_back(osg::Vec4(1.0, 0.0, 0.0, 1.0));
// 	VertexColor->push_back(osg::Vec4(0.0, 1.0, 0.0, 1.0));
// 	VertexColor->push_back(osg::Vec4(0.0, 0.0, 1.0, 1.0));
// 	VertexColor->push_back(osg::Vec4(1.0, 1.0, 0.0, 1.0));
// 	Geom->setColorArray(VertexColor.get());
// 	Geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

	//设置纹理坐标
	osg::ref_ptr<osg::Vec2Array> vt= new osg::Vec2Array();
	vt->push_back(osg::Vec2(0.0f,0.0f));
	vt->push_back(osg::Vec2(1.0f,0.0f));
	vt->push_back(osg::Vec2(1.0f,1.0f));
	vt->push_back(osg::Vec2(0.0f,1.0f));
	Geom->setTexCoordArray(0,vt.get());

	//添加图元
	Geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,4));
	//绘制
	Geode->addDrawable(Geom.get());

	return Geode.get();
}

//***********************************************************
//FUNCTION:
void build_world(osg::Group *root)
{
	g_Tex = new osg::Texture2D;
	g_Tex->setTextureSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	g_Tex->setInternalFormat(GL_RGBA);
	g_Tex->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
	g_Tex->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
	g_Tex->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
	g_Tex->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);

	osg::ref_ptr<osg::Node> SubGraph = osgDB::readNodeFile("../OSGData/cow.osg");
	if (!SubGraph) return;

	osg::ref_ptr<osg::Camera> RTTCamera = new osg::Camera;
	RTTCamera->setClearColor(osg::Vec4f(1.0, 1.0, 1.0, 1.0));
	RTTCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	RTTCamera->setViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	RTTCamera->setRenderOrder(osg::Camera::PRE_RENDER);

	const osg::BoundingSphere& BoundingSphere = SubGraph->getBound();
// 	float ZNear = 1.0f * BoundingSphere.radius();
// 	float ZFar  = 3.0f * BoundingSphere.radius();
// 
// 	// 2:1 aspect ratio as per flag geomtry below.
// 	float Proj_top   = 0.5f*ZNear;
// 	float Proj_right = 0.5f*ZNear;
// 	ZNear *= 0.9f;
// 	ZFar *= 1.1f;
// 
// 	// set up projection.
// 	RTTCamera->setProjectionMatrixAsFrustum(-Proj_right,Proj_right,-Proj_top,Proj_top,ZNear,ZFar);
	RTTCamera->setProjectionMatrixAsPerspective(60.0, SCREEN_WIDTH/SCREEN_HEIGHT, 0.1, 100);

	// set view
	RTTCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	RTTCamera->setViewMatrixAsLookAt(BoundingSphere.center()+osg::Vec3(0.0f,-2.0f,0.0f)*BoundingSphere.radius(),BoundingSphere.center(),osg::Vec3(0.0f,0.0f,1.0f));
//	RTTCamera->setViewMatrixAsLookAt(osg::Vec3(0.0, -15.0, 0.0), osg::Vec3(0.0, 0.0, 0.0), osg::Vec3(0.0, 1.0, 0.0));
	
	RTTCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
	// attach the texture and use it as the color buffer.
	RTTCamera->attach(osg::Camera::COLOR_BUFFER0, g_Tex.get());
	// attach the subgraph
	RTTCamera->addChild(SubGraph.get());
	// attach the camera to the main scene graph.    
	root->addChild(RTTCamera.get());
}

int main()
{
	osgViewer::Viewer Viewer;
	osg::GraphicsContext::Traits *pTraits = new osg::GraphicsContext::Traits;
	pTraits->x = 100;
	pTraits->y = 100;
	pTraits->width = 1024;
	pTraits->height = 768;
	pTraits->windowName = "Hive Application";
	pTraits->windowDecoration = true;
	pTraits->doubleBuffer = true;
	osg::GraphicsContext* pGraphicsContext = osg::GraphicsContext::createGraphicsContext(pTraits);

	osg::ref_ptr<osg::Camera> pCamera = Viewer.getCamera();
	pCamera->setGraphicsContext(pGraphicsContext);
	
	osgGA::CameraManipulator* pCameraManipulator = new osgGA::TrackballManipulator();
	Viewer.setCameraManipulator(pCameraManipulator);
	
	pCamera->setClearColor(osg::Vec4f(1.0, 1.0, 1.0, 1.0));
	pCamera->setViewport(0, 0, SCREEN_WIDTH, SCREEN_WIDTH);
	pCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	pCamera->setViewMatrix(osg::Matrix::identity());
	pCamera->setProjectionMatrix(osg::Matrix::identity());
	pCameraManipulator->updateCamera(*pCamera);

	osg::ref_ptr<osg::Group> Root = new osg::Group;
	build_world(Root.get());

	osg::ref_ptr<osg::Node> QuadNode = createQuadNode();
	QuadNode->getOrCreateStateSet()->setTextureAttributeAndModes(0, g_Tex.get(), osg::StateAttribute::ON);
	QuadNode->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
	Root->addChild(QuadNode);
	
	//osg::ref_ptr<osg::Node> SubGraph = osgDB::readNodeFile("../OSGData/cow.osg");
	//Root->addChild(SubGraph);
	Viewer.setSceneData(Root.get());
	Viewer.realize();

	Viewer.run();

	return 0;
}
