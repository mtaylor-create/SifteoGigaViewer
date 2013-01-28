/*
 * Sifteo SDK Example.
 */

#include <sifteo.h>
#include "assets.gen.h"
using namespace Sifteo;

static AssetSlot MainSlot = AssetSlot::allocate().bootstrap(gigaGroup);

static Metadata M = Metadata()
    .title("Sensors SDK Example")
    .package("com.sifteo.sdk.sensors", "1.1")
    .icon(Icon)
    .cubeRange(0, CUBE_ALLOCATION);

static VideoBuffer vid[CUBE_ALLOCATION];
static TiltShakeRecognizer motion[CUBE_ALLOCATION];

class SensorListener {
public:
    struct Counter {
        unsigned touch;
        unsigned neighborAdd;
        unsigned neighborRemove;
        unsigned orientation;
        signed x;
        signed y;
    } counters[CUBE_ALLOCATION];

    unsigned newest;
    
    void install()
    {
        Events::neighborAdd.set(&SensorListener::onNeighborAdd, this);
        Events::neighborRemove.set(&SensorListener::onNeighborRemove, this);
        Events::cubeAccelChange.set(&SensorListener::onAccelChange, this);
        Events::cubeTouch.set(&SensorListener::onTouch, this);
        Events::cubeBatteryLevelChange.set(&SensorListener::onBatteryChange, this);
        Events::cubeConnect.set(&SensorListener::onConnect, this);

        // Handle already-connected cubes
        for (CubeID cube : CubeSet::connected())
            onConnect(cube);
    }

private:
    void onConnect(unsigned id)
    {
        CubeID cube(id);
        uint64_t hwid = cube.hwID();

        bzero(counters[id]);
        LOG("Cube %d connected\n", id);

        vid[id].initMode(BG0_ROM);
        vid[id].attach(id);
        motion[id].attach(id);

        // Draw the cube's identity
        String<128> str;
        str << "I am cube #" << cube << "\n";
        //str << "hwid " << Hex(hwid >> 32) << "\n     " << Hex(hwid) << "\n\n";
        vid[cube].bg0rom.text(vec(1,2), str);

        vid[cube].initMode(BG0_SPR_BG1);
        vid[cube].attach(cube);
        vid[cube].bg0.image(vec(0,0), giga, 22);
        
        // Draw initial state for all sensors
        onAccelChange(cube);
        onBatteryChange(cube);
        onTouch(cube);
        drawNeighbors(cube);
    }

    void onBatteryChange(unsigned id)
    {
    }

    void onTouch(unsigned id)
    {
        newest=id;
    }

    void onAccelChange(unsigned id)
    {
    }

    void onNeighborRemove(unsigned firstID, unsigned firstSide, unsigned secondID, unsigned secondSide)
    {
        LOG("Neighbor Remove: %02x:%d - %02x:%d\n", firstID, firstSide, secondID, secondSide);

        if (firstID < arraysize(counters)) {
            counters[firstID].neighborRemove++;
            drawNeighbors(firstID);
        }
        if (secondID < arraysize(counters)) {
            counters[secondID].neighborRemove++;
            drawNeighbors(secondID);
        }
    }

    void onNeighborAdd(unsigned firstID, unsigned firstSide, unsigned secondID, unsigned secondSide)
    {
        LOG("Neighbor Add: %02x:%d:%d - %02x:%d:%d\n",
            firstID, firstSide, counters[firstID].orientation,
            secondID, secondSide, counters[secondID].orientation);
        
        
        
        
        
        
        if(newest==firstID) {
            counters[firstID].orientation=umod(secondSide
                                                      +counters[secondID].orientation
                                                      -firstSide
                                                      +2,4);
            
            switch (umod(firstSide + counters[firstID].orientation, 4)) {
            case 0:
                counters[firstID].y=counters[secondID].y-1;
                counters[firstID].x=counters[secondID].x;
                break;
            case 1:
                counters[firstID].x=counters[secondID].x+1;
                counters[firstID].y=counters[secondID].y;
                break;
            case 2:
                counters[firstID].y=counters[secondID].y+1;
                counters[firstID].x=counters[secondID].x;
                break;
            case 3:
                counters[firstID].x=counters[secondID].x-1;
                counters[firstID].y=counters[secondID].y;
                break;
                
            default:
                break;
            }
            
            
        }
        if (newest==secondID) {
            counters[secondID].orientation=umod(firstSide
                                                      +counters[firstID].orientation
                                                      -secondSide
                                                      +2,4);
            
            switch (umod(secondSide + counters[secondID].orientation, 4)) {
            case 0:
                counters[secondID].y=counters[firstID].y-1;
                    counters[secondID].x=counters[firstID].x;
                break;
            case 1:
                counters[secondID].x=counters[firstID].x+1;
                    counters[secondID].y=counters[firstID].y;
                break;
            case 2:
                counters[secondID].y=counters[firstID].y+1;
                    counters[secondID].x=counters[firstID].x;
                break;
            case 3:
                counters[secondID].x=counters[firstID].x-1;
                    counters[secondID].y=counters[firstID].y;
                break;
                
            default:
                break;
            }
        }


        if (firstID < arraysize(counters)) {
            counters[firstID].neighborAdd++;
            drawNeighbors(firstID);
        }
        if (secondID < arraysize(counters)) {
            counters[secondID].neighborAdd++;
            drawNeighbors(secondID);
        }
    }

    bool isLonely(CubeID cube)
    {
        Neighborhood nb(cube);
        return !((nb.hasCubeAt(TOP)
                ||nb.hasCubeAt(BOTTOM))
                ||(nb.hasCubeAt(LEFT)
                ||nb.hasCubeAt(RIGHT)));
        
    }
    
    void drawNeighbors(CubeID cube)
    {
        Neighborhood nb(cube);
        
        //Rotation r;
        
        unsigned index;
        

        String<64> str;
        str << "nb "
            << Hex(nb.neighborAt(TOP), 2) << " "
            << Hex(nb.neighborAt(LEFT), 2) << " "
            << Hex(nb.neighborAt(BOTTOM), 2) << " "
            << Hex(nb.neighborAt(RIGHT), 2) << "\n"
            << counters[cube].orientation << " "
            << counters[cube].x << " "
            << counters[cube].y << "   \n";
        
        //unsigned r = 1;
        
        index = (counters[cube].x+3)-(5*(counters[cube].y-2));
        //vid[cube].setRotation(Rotation(counters[cube].orientation));
        vid[cube].bg0.image(vec(0,0), giga, index);


        //BG0ROMDrawable &draw = vid[cube].bg0rom;
        //draw.text(vec(1,6), str);

        //drawSideIndicator(draw, nb, vec( 1,  0), vec(14,  1), TOP);
        //drawSideIndicator(draw, nb, vec( 0,  1), vec( 1, 14), LEFT);
        //drawSideIndicator(draw, nb, vec( 1, 15), vec(14,  1), BOTTOM);
        //drawSideIndicator(draw, nb, vec(15,  1), vec( 1, 14), RIGHT);
    }

    static void drawSideIndicator(BG0ROMDrawable &draw, Neighborhood &nb,
        Int2 topLeft, Int2 size, Side s)
    {
        unsigned nbColor = draw.ORANGE;
        draw.fill(topLeft, size,
            nbColor | (nb.hasNeighborAt(s) ? draw.SOLID_FG : draw.SOLID_BG));
    }
};


void main()
{
    static SensorListener sensors;

    sensors.install();

    // We're entirely event-driven. Everything is
    // updated by SensorListener's event callbacks.
    while (1)
        System::paint();
}
