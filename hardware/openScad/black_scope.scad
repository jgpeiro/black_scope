//-----------------------------------------------------------------------
// Yet Another Parameterized Projectbox generator
//
//  This is a box for <template>
//
//  Version 1.6 (06-10-2022)
//
// This design is parameterized based on the size of a PCB.
//
//  for many or complex cutoutGrills you might need to adjust
//  the number of elements:
//
//      Preferences->Advanced->Turn of rendering at 100000 elements
//                                                  ^^^^^^
//
//-----------------------------------------------------------------------

// Subir 0.66mm para el conector JTAG
// Ampliar el conector para el USB
translate([-100+4.4+2,-71+0.3+2,4.6-0.4+1+1+5.87+1.5-2-0.5-5.35])
{
    scale(10*2.54)
    {
        rotate([0,180,180])
        {
            //import( "C:/git/black_scope/black_scope/hardware/black_scope/black_scope.stl" );
        }
    }
}

translate([46.9+1,35.0-1,11.1-5.35])
{
    rotate([0,0,90])
    {
        scale(10*2.54){
          // # import( "C:/Users/Jose/Downloads/ER-TFT035-6.stl" );
        }
    }
}

polygonShape = [  [0,0],[20,15],[30,0],[40,15],[70,15]
                 ,[50,30],[60,45], [40,45],[30,70]
                 ,[20,45], [0,45]
               ];


include <../../../YAPP_Box/library/YAPPgenerator_v16.scad>

// Note: length/lengte refers to X axis, 
//       width/breedte to Y, 
//       height/hoogte to Z

/*
            padding-back>|<---- pcb length ---->|<padding-front
                                 RIGHT
                   0    X-ax ---> 
               +----------------------------------------+   ---
               |                                        |    ^
               |                                        |   padding-right 
             ^ |                                        |    v
             | |    -5,y +----------------------+       |   ---              
        B    Y |         | 0,y              x,y |       |     ^              F
        A    - |         |                      |       |     |              R
        C    a |         |                      |       |     | pcb width    O
        K    x |         |                      |       |     |              N
               |         | 0,0              x,0 |       |     v              T
               |   -5,0  +----------------------+       |   ---
               |                                        |    padding-left
             0 +----------------------------------------+   ---
               0    X-ax --->
                                 LEFT
*/

printBaseShell      = true;
printLidShell       = true;

// Edit these parameters for your own board dimensions
wallThickness       = 4.0;
basePlaneThickness  = 2.0;
lidPlaneThickness   = 1.0;

baseWallHeight      = 4;
lidWallHeight       = 3.1;

// ridge where base and lid off box can overlap
// Make sure this isn't less than lidWallHeight
ridgeHeight         = 3;
ridgeSlack          = 0.2;
roundRadius         = 4.0;

// How much the PCB needs to be raised from the base
// to leave room for solderings and whatnot
standoffHeight      = 2;
pinDiameter         = 0;
pinHoleSlack        = 0;
standoffDiameter    = 4;

// Total height of box = basePlaneThickness + lidPlaneThickness 
//                     + baseWallHeight + lidWallHeight
pcbLength           = 89;//87;
pcbWidth            = 60.5-0.4;//59;
pcbThickness        = 1.7;
                            
// padding between pcb and inside wall
paddingFront        = 0.4;
paddingBack         = 0.4;
paddingRight        = 0.4;
paddingLeft         = 0.4;


//-- D E B U G -------------------
showSideBySide      = true;
//showSideBySide      = false;
hideLidWalls        = false;
onLidGap            = 0;
shiftLid            = -8;
colorLid            = "yellow";
hideBaseWalls       = false;
colorBase           = "white";
showPCB             = false;
//showPCB             = true;
showMarkers         = false;
inspectX            = 0;  // 0=none, >0 from front, <0 from back
inspectY            = 0;  // 0=none, >0 from left, <0 from right


//-- pcb_standoffs  -- origin is pcb[0,0,0]
// (0) = posx
// (1) = posy
// (2) = { yappBoth | yappLidOnly | yappBaseOnly }
// (3) = { yappHole, YappPin }
_pcbStands = [
                [35,  6, yappBaseOnly, yappHole],
           //     [72,  7, yappBaseOnly, yappHole], 
           //     [72,  52, yappBaseOnly, yappHole],
               [16,  25, yappBaseOnly, yappHole],
              [16,  45, yappBaseOnly, yappHole], 
              
              [63,  4, yappBaseOnly, yappHole], 
              [83,  4, yappBaseOnly, yappHole],
              [83,  59-4, yappBaseOnly, yappHole],
              [51,  11, yappBaseOnly, yappHole], 
              [51,  46, yappBaseOnly, yappHole], 
              

              [0,  0, yappBaseOnly, yappHole], 
              [0,  pcbWidth, yappBaseOnly, yappHole],
             [pcbLength,  0, yappBaseOnly, yappHole],
            [ pcbLength, pcbWidth, yappBaseOnly, yappHole],
            
             ];     

//-- Lid plane    -- origin is pcb[0,0,0]
// (0) = posx
// (1) = posy
// (2) = width
// (3) = length
// (4) = angle
// (5) = { yappRectangle | yappCircle }
// (6) = { yappCenter }
cutoutsLid =  [
                    //[2, 2, 55, 83, 0, yappRectangle]  
                    [2+1.22+1, 2+1.8+0.75, 48.96+2, 73.44+2, 0, yappRectangle]  
               //   , [20, 50, 10, 20, 0, yappRectangle, yappCenter]
               //   , [50, 50, 10, 2, 0, yappCircle]
               //   , [pcbLength-10, 20, 15, 0, 0, yappCircle] 
               //   , [50, pcbWidth, 5, 7, 0, yappRectangle, yappCenter]
              ];

//-- base plane    -- origin is pcb[0,0,0]
// (0) = posx
// (1) = posy
// (2) = width
// (3) = length
// (4) = angle
// (5) = { yappRectangle | yappCircle }
// (6) = { yappCenter }
cutoutsBase =   [
                    [-3, 1.8-0.2+0.75, 16+0.4, 11.5, 0, yappRectangle],
                    //[2, 50.8, 6.4, 4.9, 0, yappRectangle],
                    //[-3, 24.7, 9.9, 10.3, 0, yappRectangle],
                    [39, 4+0.75, 2, 8, 0, yappRectangle],
                    [24.765, 42.235+0.75, 3.53, 6.47, 0, yappRectangle],
                    [22.5-0.2, 42.000+0.75, 4, 11+0.4, 0, yappRectangle],
                    [22.5+4, 42.000+0.75, 5, 3, 0, yappRectangle],
                    [5.36, 40+0.75, 2, 0, 0, yappCircle],
                    [5.36, 44+0.75, 2, 0, 0, yappCircle],
                    //[14, 51+0.75, 1.5, 0, 0, yappCircle],
                    [3, 54.75, 1.5, 0, 0, yappCircle],
                    
                    [-5.5, 24.7+0.75-(12.7-9.9)/2, 12.7, 3.5, 0, yappRectangle],
                    [-5.5, 24.7+0.75, 9.9, 9.9+3, 0, yappRectangle],
                 // , [30, 10, 15, 10, 45, yappRectangle, yappCenter]
                 // , [20, pcbWidth-20, 15, 0, 0, yappCircle]
                 // , [pcbLength-15, 5, 10, 2, 0, yappCircle]
                ];

//-- front plane  -- origin is pcb[0,0,0]
// (0) = posy
// (1) = posz
// (2) = width
// (3) = height
// (4) = angle
// (5) = { yappRectangle | yappCircle }
// (6) = { yappCenter }
cutoutsFront =  [
               //     [0, 5, 10, 15, 0, yappRectangle]               // org
               //  ,  [25, 3, 10, 10, 0, yappRectangle, yappCenter]  // center
               //  ,  [60, 10, 15, 6, 0, yappCircle]                 // circle
                ];

//-- back plane  -- origin is pcb[0,0,0]
// (0) = posy
// (1) = posz
// (2) = width
// (3) = height
// (4) = angle
// (5) = { yappRectangle | yappCircle }
// (6) = { yappCenter }
cutoutsBack =   [
                [1.8-0.2+0.75, -5.5, 16+0.4, 3.4, 0, yappRectangle],
                [24.7+0.75+1.4-(12.7-9.9), -5.5+2.5, 12.7, 3.4, 0, yappRectangle],
                //[ 24.7+0.75-(12.7-9.9)/2, -1.6-3.5, 12.7, 3.5, 0, yappRectangle],
                //  , [25, 18, 10, 6, 0, yappRectangle, yappCenter]  // center
                //  , [50, 0, 8, 8, 0, yappCircle]                   // circle
                ];

//-- left plane   -- origin is pcb[0,0,0]
// (0) = posx
// (1) = posz
// (2) = width
// (3) = height
// (4) = angle
// (5) = { yappRectangle | yappCircle }
// (6) = { yappCenter }
cutoutsLeft =   [
                 //   [25, 0, 6, 20, 0, yappRectangle]                       // org
                 // , [pcbLength-35, 0, 20, 6, 0, yappRectangle, yappCenter] // center
                 // , [pcbLength/2, 10, 20, 6, 0, yappCircle]                // circle
                ];

//-- right plane   -- origin is pcb[0,0,0]
// (0) = posx
// (1) = posz
// (2) = width
// (3) = height
// (4) = angle
// (5) = { yappRectangle | yappCircle }
// (6) = { yappCenter }
cutoutsRight =  [
                 //   [10, 0, 9, 5, 0, yappRectangle]                // org
                 // , [40, 0, 9, 5, 0, yappRectangle, yappCenter]    // center
                 // , [60, 0, 9, 5, 0, yappCircle]                   // circle
                ];

//-- cutoutGrills    -- origin is pcb[x0,y0, zx]
// (0) = xPos
// (1) = yPos
// (2) = grillWidth
// (3) = grillLength
// (4) = gWidth
// (5) = gSpace
// (6) = gAngle
// (7) = plane {"base" | "lid" }
// (8) = {polygon points}}

cutoutsGrill = [
                // [35,  8, 70, 70, 2, 3, 50, "base", polygonShape ]
                //,[ 0, 20, 10, 40, 2, 3, 50, "lid"]
                //,[45,  0, 50, 10, 2, 3, 45, "lid"]
                //,[15, 85, 50, 10, 2, 3,  20, "base"]
                //,[85, 15, 10, 50, 2, 3,  45, "lid"]
               ];

//-- connectors -- origen = box[0,0,0]
// (0) = posx
// (1) = posy
// (2) = screwDiameter
// (3) = insertDiameter
// (4) = outsideDiameter
// (5) = { yappAllCorners }
connectors   =  [
              //      [8, 8, 2.5, 3.8, 5, yappAllCorners]
               //   , [30, 8, 5, 5, 5]
                ];
                
//-- connectorsPCB -- origin = pcb[0,0,0]
//-- a connector that allows to screw base and lid together through holes in the PCB
// (0) = posx
// (1) = posy
// (2) = screwDiameter
// (3) = insertDiameter
// (4) = outsideDiameter
// (5) = { yappAllCorners }
connectorsPCB   =  [
                //    [pcbLength/2, 10, 2.5, 3.8, 5]
                //   ,[pcbLength/2, pcbWidth-10, 2.5, 3.8, 5]
                ];

//-- base mounts -- origen = box[x0,y0]
// (0) = posx | posy
// (1) = screwDiameter
// (2) = width
// (3) = height
// (4..7) = yappLeft / yappRight / yappFront / yappBack (one or more)
// (5) = { yappCenter }
baseMounts   = [
            //        [-5, 3.5, 10, 3, yappRight, yappCenter]
            //      , [0, 3.5, shellLength, 3, yappLeft, yappCenter]
            //      , [0, 3.5, 33, 3, yappLeft]
           //       , [shellLength, 3.5, 33, 3, yappLeft]
           //       , [shellLength/2, 3.5, 30, 3, yappLeft, yappCenter]
            //      , [10, 3.5, 15, 3, yappBack, yappFront]
            //      , [shellWidth-10, 3.5, 15, 3, yappBack, yappFront]
               ];
               
//-- snap Joins -- origen = box[x0,y0]
// (0) = posx | posy
// (1) = width
// (2..5) = yappLeft / yappRight / yappFront / yappBack (one or more)
// (n) = { yappSymmetric }
snapJoins   =     [
                    [20, 10, yappLeft, yappRight, yappSymmetric]
              //    [5, 10, yappLeft]
              //  , [shellLength-2, 10, yappLeft]
               //   , [30,  10, yappFront, yappBack]
              //  , [2.5, 3, 5, yappBack, yappFront, yappSymmetric]
                ];
               
//-- origin of labels is box [0,0,0]
// (0) = posx
// (1) = posy/z
// (2) = orientation
// (3) = plane {lid | base | left | right | front | back }
// (4) = font
// (5) = size
// (6) = "label text"
labelsPlane =  [
                 //   [10,  10,   0, "lid",   "Liberation Mono:style=bold", 7, "YAPP" ]
                //  , [100, 90, 180, "base",  "Liberation Mono:style=bold", 7, "Base" ]
               //   , [8,    8,   0, "left",  "Liberation Mono:style=bold", 7, "Left" ]
              //    , [10,   5,   0, "right", "Liberation Mono:style=bold", 7, "Right" ]
              //    , [40,  23,   0, "front", "Liberation Mono:style=bold", 7, "Front" ]
              //    , [5,    5,   0, "back",  "Liberation Mono:style=bold", 7, "Back" ]
               ];


//========= MAIN CALL's ===========================================================
  
//===========================================================
//module lidHookInside()
{
  //echo("lidHookInside(original) ..");
  
} // lidHookInside(dummy)
  
//===========================================================
//module lidHookOutside()
{
  //echo("lidHookOutside(original) ..");
  
} // lidHookOutside(dummy)

//===========================================================
//module baseHookInside()
{
  //echo("baseHookInside(original) ..");
  
} // baseHookInside(dummy)

//===========================================================
//module baseHookOutside()
{
  //echo("baseHookOutside(original) ..");
  
} // baseHookOutside(dummy);




//---- This is where the magic happens ----

$fn=50;
union()
{
    difference() 
    {
        YAPPgenerate();    
        translate( [pcbLength-5, pcbWidth/2+4, 4-0.4-1.2 ] )
        {
            cube([17,41.7, 3.2], center=true );
        };
    };

    translate([4.2,4.2,2]){
         cylinder(r=2,h=2);
    };
    translate([4+pcbLength+0.4,4.2,2]){
         cylinder(r=2,h=2);
    };

    translate([4.2,4.7+pcbWidth,2]){
         cylinder(r=2,h=2);
    };
    translate([4+pcbLength+0.4,4.7+pcbWidth,2]){
         cylinder(r=2,h=2);
    };

    translate([4+pcbLength/2+0.4,4.2,2]){
         cylinder(r=2,h=2);
    };
    translate([4+pcbLength/2+0.4,4.7+pcbWidth,2]){
         cylinder(r=2,h=2);
    };
    
    translate([4.2,4.2+21.5+0.75,2]){
         cylinder(r=2,h=2);
    };
}
