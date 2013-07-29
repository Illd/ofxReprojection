#include "guiRepro.h"

void guiRepro::setup()
{

    ofxGuiSetFont("Ubuntu-L.ttf",10,true,true);
    main.setup("Main controls"); // most of the time you don't need a name
    //main.setDefaultHeight(22);
    main.add(gUsetransform.setup("Use transform", false));
    main.add(gUsetexture.setup("Use texture", false));
	main.add(radius.setup( "radius", 140, 10, 300 ));
	main.add(center.setup("center",ofVec2f(ofGetWidth()*.5,ofGetHeight()*.5),ofVec2f(0,0),ofVec2f(ofGetWidth(),ofGetHeight())));
	main.add(circleResolution.setup("circle res", 5, 3, 90));
    main.add(twoCircles.setup("two circles"));
	main.add(ringButton.setup("ring"));
    main.add(drawMeasure.setup("Enable measurements", true));

    //measure.setDefaultHeight(22);
    measure.setup("Measure/solver");
    //measure.setDefaultHeight(25);
    measure.add(startMeasure.setup("Start measurements"));

    main.setPosition(20, 600);
    measure.setPosition(2 * main.getPosition().x + main.getWidth(), 600);

}

void guiRepro::draw() {
    main.draw();
    if(drawMeasure) {

    measure.draw();
    }

    if(startMeasure) {
        cout << "test" <<endl;
    }


}
