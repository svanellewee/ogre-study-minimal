 
#define OIS_DYNAMIC_LIB
 
#include "Ogre.h" 
#include "OgreFrameListener.h" 
#include <OIS/OIS.h>
 
using namespace Ogre; 

class SimpleFrameListener : public FrameListener 
{ 
public: 
  SimpleFrameListener(OIS::Keyboard* keyboard, OIS::Mouse* mouse) 
  { 
    mKeyboard = keyboard; 
    mMouse = mouse; 
  } 
  // This gets called before the next frame is beeing rendered.
  bool frameStarted(const FrameEvent& evt) 
  {
    //update the input devices
    mKeyboard->capture();
    mMouse->capture();
 
    //exit if key KC_ESCAPE pressed 
    if(mKeyboard->isKeyDown(OIS::KC_ESCAPE)) 
      return false; 
 
    return true; 
  } 
  // This gets called at the end of a frame.
  bool frameEnded(const FrameEvent& evt) 
  { 
    return true; 
  } 
private: 
  OIS::Keyboard* mKeyboard; 
  OIS::Mouse* mMouse; 
}; 
 
class SimpleKeyListener : public OIS::KeyListener 
{ 
public: 
  bool keyPressed(const OIS::KeyEvent& e){ return true; }
 
  bool keyReleased(const OIS::KeyEvent& e){ return true; }
};
 
class SimpleMouseListener : public OIS::MouseListener
{
public: 
  bool mouseMoved(const OIS::MouseEvent& e){ return true; }
 
  bool mousePressed(const OIS::MouseEvent& e, OIS::MouseButtonID id){ return true; }
 
  bool mouseReleased(const OIS::MouseEvent& e, OIS::MouseButtonID id){ return true; }
};
 
void load_resource_config() {
  ConfigFile cf;
  cf.load("resources.cfg");
 
  ConfigFile::SectionIterator seci = cf.getSectionIterator();
 
  String secName, typeName, archName;
  while (seci.hasMoreElements())
    {
      secName = seci.peekNextKey();
      ConfigFile::SettingsMultiMap *settings = seci.getNext();
      ConfigFile::SettingsMultiMap::iterator i;
      for (i = settings->begin(); i != settings->end(); ++i)
        {
          typeName = i->first;
          archName = i->second;
          ResourceGroupManager::getSingleton().addResourceLocation(
                                                                   archName, typeName, secName);
        }
    }

}

void create_scene(SceneManager* mSceneMgr) {
  mSceneMgr->setAmbientLight(ColourValue(0,0,0));
  mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);
  Entity* entOgreHead = mSceneMgr->createEntity("OgreHeadAgain", "ogrehead.mesh");
  entOgreHead->setCastShadows(true);
  mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(entOgreHead);
  Plane plane(Vector3::UNIT_Y, 0);
  MeshManager::getSingleton().createPlane("ground", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                                          plane, 1500, 1500, 20,20, true, 1, 5,5,Vector3::UNIT_Z);
  Entity *entGround = mSceneMgr->createEntity("GroundEntity", "ground");
  mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(entGround); /// you forgot this line!!!

  entGround->setMaterialName("SurfaceX");
  entGround->setCastShadows(false);


  // ---
  Light* pointLight = mSceneMgr->createLight("pointLight");
  pointLight->setType(Light::LT_POINT);
  pointLight->setPosition(Vector3(0, 150, 250));
  
  pointLight->setDiffuseColour(1.0, 0.0, 0.0);
  pointLight->setSpecularColour(1.0, 0.0, 0.0);

  // --
  Light* directionalLight = mSceneMgr->createLight("directionalLight");
  directionalLight->setType(Light::LT_DIRECTIONAL);
  directionalLight->setDiffuseColour(ColourValue(.25, .25, 0));
  directionalLight->setSpecularColour(ColourValue(.25, .25, 0));

  directionalLight->setDirection(Vector3( 0, -1, 1 )); 
  
  // --
  Light* spotLight = mSceneMgr->createLight("spotLight");
  spotLight->setType(Light::LT_SPOTLIGHT);
  spotLight->setDiffuseColour(0, 0, 1.0);
  spotLight->setSpecularColour(0, 0, 1.0);

  spotLight->setDirection(-1, -1, 0);
  spotLight->setPosition(Vector3(300, 300, 0));

  spotLight->setSpotlightRange(Degree(35), Degree(50));
}

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
  int main(int argc, char **argv)
#endif
{
  Root* root = new Root("plugins.cfg");
    
  load_resource_config();  // -- 

  RenderWindow* window = NULL;
  if(root->showConfigDialog()) {
    window = root->initialise(true, "Simple Ogre App");
  } else {
    //Ogre
    delete root;
    return false; // Exit the application on cancel
  }
 
  SceneManager* sceneMgr = root->createSceneManager(ST_GENERIC); 
  Camera* camera = sceneMgr->createCamera("SimpleCamera"); 
  // Position it at 500 in Z direction
  camera->setPosition(Ogre::Vector3(0,0,80));
  // Look back along -Z
  camera->lookAt(Ogre::Vector3(0,0,-300));
  camera->setNearClipDistance(5);
  Viewport* viewPort = window->addViewport(camera);

  ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

  create_scene(sceneMgr);
 
  OIS::ParamList pl;
  size_t windowHnd = 0;
  std::ostringstream windowHndStr;
 
  window->getCustomAttribute("WINDOW", &windowHnd);
  windowHndStr << windowHnd;
  pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
 
  OIS::InputManager* inputManager = OIS::InputManager::createInputSystem(pl);
  OIS::Keyboard* keyboard = static_cast<OIS::Keyboard*>(inputManager->createInputObject(OIS::OISKeyboard, true));
  OIS::Mouse*    mouse = static_cast<OIS::Mouse*>(inputManager->createInputObject(OIS::OISMouse, true));
 
  unsigned int width, height, depth;
  int top, left;
  window->getMetrics(width, height, depth, left, top);
  const OIS::MouseState &ms = mouse->getMouseState();
  ms.width = width;
  ms.height = height;
 
  SimpleKeyListener* keyListener = new SimpleKeyListener();
  keyboard->setEventCallback(keyListener);
  SimpleMouseListener* mouseListener = new SimpleMouseListener();
  mouse->setEventCallback(mouseListener);
  SimpleFrameListener* frameListener = new SimpleFrameListener(keyboard, mouse);
  root->addFrameListener(frameListener); 
 
  root->startRendering(); // blocks until a frame listener returns false. eg from pressing escape in this example
 
  inputManager->destroyInputObject(mouse); mouse = 0;
  inputManager->destroyInputObject(keyboard); keyboard = 0;
  OIS::InputManager::destroyInputSystem(inputManager); inputManager = 0;

  delete frameListener; 
  delete mouseListener; 
  delete keyListener;
  //Ogre
  delete root;
 
  return 0; 
}
