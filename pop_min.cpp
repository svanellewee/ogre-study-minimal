 
#define OIS_DYNAMIC_LIB
 
#include "Ogre.h" 
#include "OgreFrameListener.h" 
#include <OIS/OIS.h>
 
using namespace Ogre; 

class SimpleFrameListener : public FrameListener 
{ 
public: 
  SimpleFrameListener(OIS::Keyboard* keyboard) 
  { 
    mKeyboard = keyboard;
  } 
  // This gets called before the next frame is beeing rendered.
  bool frameStarted(const FrameEvent& evt) 
  {
    //update the input devices
    mKeyboard->capture();
 
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
}; 
 
class SimpleKeyListener : public OIS::KeyListener 
{ 
public: 
  bool keyPressed(const OIS::KeyEvent& e){ return true; }
 
  bool keyReleased(const OIS::KeyEvent& e){ return true; }
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
          ResourceGroupManager::getSingleton().addResourceLocation(archName,
                                                                   typeName,
                                                                   secName);
        }
    }

}

void create_scene(SceneManager* mSceneMgr) {
  mSceneMgr->setAmbientLight(ColourValue(0.9,0.9,0.9));
  mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);
  Entity* entOgreHead = mSceneMgr->createEntity("OgreHeadAgain", "ogrehead.mesh");
  entOgreHead->setCastShadows(true);
  mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(entOgreHead);
}

Camera* setup_camera(SceneManager *sceneMgr) {
  Camera* camera = sceneMgr->createCamera("SimpleCamera"); 
  // Position it at 500 in Z direction
  camera->setPosition(Ogre::Vector3(0,0,80));
  // Look back along -Z
  camera->lookAt(Ogre::Vector3(0,0,-300));
  camera->setNearClipDistance(5);
  return camera;
}

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
  int main(int argc, char **argv)
#endif
{
  Root* root = new Root();
    
  load_resource_config();  // -- 

  if (!root->restoreConfig()) {
    root->showConfigDialog();
  }

  RenderWindow* window = NULL;
  window = root->initialise(true, "Simple Ogre App");
  if (!window) {
    delete root;
    return false;
  }

  SceneManager* sceneMgr = root->createSceneManager(ST_GENERIC); 
  Camera *camera = setup_camera(sceneMgr);
  Viewport* viewPort = window->addViewport(camera);

  ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

  create_scene(sceneMgr);
 
  // setup input  (using OIS)
  OIS::ParamList pl;
  size_t windowHnd = 0;
  std::ostringstream windowHndStr; 
  window->getCustomAttribute("WINDOW", &windowHnd);
  windowHndStr << windowHnd;
  pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str())); 
  OIS::InputManager* inputManager = OIS::InputManager::createInputSystem(pl);
  OIS::Keyboard* keyboard = static_cast<OIS::Keyboard*>(inputManager->createInputObject(OIS::OISKeyboard, true)); 
  SimpleKeyListener* keyListener = new SimpleKeyListener();
  keyboard->setEventCallback(keyListener);

  // update frame based on keyboard input
  SimpleFrameListener* frameListener = new SimpleFrameListener(keyboard);
  
  root->addFrameListener(frameListener);  
  root->startRendering(); // blocks until a frame listener returns false. eg from pressing escape in this example


  inputManager->destroyInputObject(keyboard); keyboard = 0;
  OIS::InputManager::destroyInputSystem(inputManager); inputManager = 0;

  delete frameListener; 
  delete keyListener;
  //Ogre
  delete root;
 
  return 0; 
}
