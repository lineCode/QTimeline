#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Utilities.h"
#include "cinder/ImageIo.h"
#include "Resources.h"
#include "QTimeline.h"

#include "BasicModule.h"

using namespace ci;
using namespace ci::app;
using namespace std;


class BasicSampleApp : public AppBasic {
public:
	void prepareSettings( Settings *settings );
    void shutdown();
	void setup();
	void keyDown( KeyEvent event );
	void update();
	void draw();
    
    void createModuleCallback( QTimeline::CreateModuleCallbackArgs args );
    void deleteModuleCallback( QTimeline::DeleteModuleCallbackArgs args );
    
    QTimelineRef                    mTimeline;
    vector<QTimelineModuleRef>      mModules;
};


void BasicSampleApp::prepareSettings( Settings *settings )
{
    settings->setWindowSize( 1200, 800 );
}


void BasicSampleApp::shutdown()
{
    mTimeline->shutdown();
    mTimeline.reset();
}


void BasicSampleApp::setup()
{
    mTimeline = QTimelineRef( new QTimeline() );
    
    mTimeline->init();
    
    mTimeline->initOsc();
    
    // register modules
    mTimeline->registerModule( "BasicModule", this, &BasicSampleApp::createModuleCallback, &BasicSampleApp::deleteModuleCallback );

    QTimelineModuleRef mod;
    mod = QTimelineModuleRef( new BasicModule( "Sample module one" ) );
    mTimeline->addModule( mod, 2.0f, 12.0f );
    mod->init();
    mModules.push_back( mod );
    
    mTimeline->addCue( "Cue 1", 0.0f, 3.0f );
    mTimeline->addCue( "Another cue", 4.0f, 5.0f );
    
    mTimeline->loadTheme( getAssetPath("default_theme.xml").generic_string() );
}

void BasicSampleApp::keyDown( KeyEvent event )
{
    char    c       = event.getChar();
    int     code    = event.getChar();
    
    if ( event.isAltDown() )
    {
        if ( c == 'f' )
            setFullScreen( !isFullScreen() );
        
        else if ( c == 's' )
            mTimeline->save( "/Users/Q/Code/Cinder/blocks/QTimeline/samples/BasicSample/assets/timeline.xml" );
        
        else if ( c == 'l' )
            mTimeline->load( "/Users/Q/Code/Cinder/blocks/QTimeline/samples/BasicSample/assets/timeline.xml" );
    }
    
    
    if ( code == KeyEvent::KEY_SPACE )
        mTimeline->play( !mTimeline->isPlaying(), QTimeline::FREE_RUN );
    
    else if ( code == KeyEvent::KEY_RETURN )
        mTimeline->playCue();
    
    else if ( code == KeyEvent::KEY_DELETE )
        mTimeline->setTime(0.0f);
    
    else if ( code == KeyEvent::KEY_ESCAPE )
        mTimeline->closeMenu();
    
    else if ( code == 63 )
        mTimeline->toggleHelp();
    
    else if ( c == 'd' )
        mTimeline->toggleDebugInfo();
    
    else if ( c == 'c' )
        mTimeline->collapse();
    
    else if ( c == 'r' )
    {
//        for( size_t k=0; k < mModules.size(); k++ )
//        delete mModules[0];
        
        mTimeline->clear();
        
//        mModules.clear();
    }
    
    else if ( c == 't' )
        mTimeline->addOscMessage( "/test", "f0.3" );

    else if ( (int)c >= 48 && (int)c <= 57 )
        mTimeline->playCue( (int)c - 49 );
    
    else if ( c == 'h' )
        mTimeline->toggleRender();
    
    else if ( code == 2 )
        mTimeline->step(-1);

    else if ( code == 3 )
        mTimeline->step(1);
}


void BasicSampleApp::update()
{
    mTimeline->update();
}


void BasicSampleApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
    gl::enableAlphaBlending();
    
    for( size_t k=0; k < mModules.size(); k ++ )
        if ( mModules[k]->isPlaying() )
            mModules[k]->render();
    
    gl::drawString( "modules: " + toString( mModules.size() ), Vec2i( 300, 15 ) );
    
    mTimeline->render();

    gl::color( Color::white() );
}


void BasicSampleApp::createModuleCallback( QTimeline::CreateModuleCallbackArgs args )
{
    QTimelineModuleRef mod;
    
    if( args.type == "BasicModule" )
        mod = QTimelineModuleRef( new BasicModule( args.name ) );
    
    // ...
    
    if ( !mod )
        return;
    
    mTimeline->addModule( mod, args.startTime, args.duration, args.trackRef );
    mod->init();
    
    mModules.push_back( mod );
}


void BasicSampleApp::deleteModuleCallback( QTimeline::DeleteModuleCallbackArgs args )
{
    QTimelineModuleItem* itemPtr = (QTimelineModuleItem*)args.itemRef.get();
    
    string name             = itemPtr->getName();
    string targetModuleType = itemPtr->getTargetType();
    
    for( size_t k=0; k < mModules.size(); k++ )
        if ( mModules[k]->getName() == name && mModules[k]->getType() == targetModuleType )
        {
            mModules.erase( mModules.begin() + k );
            return;
        }
}


CINDER_APP_BASIC( BasicSampleApp, RendererGl )

