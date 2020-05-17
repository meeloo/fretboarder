//
//  Tests.mm
//  Tests
//
//  Created by Sebastien Metrot on 17/05/2020.
//  Copyright © 2020 Autodesk. All rights reserved.
//

#import <XCTest/XCTest.h>

#include "Fretboard.hpp"
#include "String.hpp"
#include "Geometry.hpp"

using namespace fretboarder;

@interface Tests : XCTestCase

@end

@implementation Tests

- (void)setUp {
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
}

- (void)testRightHanded {
    Instrument instrument;
    Fretboard fretboard(instrument);
    
    XCTAssertEqual(fretboard.fret_slots().size(), 25);
    XCTAssertEqual(fretboard.strings().front().distance_from_bridge(0), instrument.scale_length[1]);
    XCTAssertEqual(fretboard.strings().back().distance_from_bridge(0), instrument.scale_length[0]);
}

- (void)testLeftHanded {
    Instrument instrument;
    instrument.right_handed = false;
    Fretboard fretboard(instrument);
    
    XCTAssertEqual(fretboard.fret_slots().size(), 25);
    XCTAssertEqual(fretboard.strings().front().distance_from_bridge(0), instrument.scale_length[0]);
    XCTAssertEqual(fretboard.strings().back().distance_from_bridge(0), instrument.scale_length[1]);
}

- (void)testPrependicularFretAtBridge {
    Instrument instrument;
    instrument.perpendicular_fret_index = 100;
    Fretboard fretboard(instrument);
    
    XCTAssertEqual(fretboard.fret_slots().size(), 25);
    XCTAssertEqual(fretboard.strings().front().distance_from_bridge(0), instrument.scale_length[1]);
    XCTAssertEqual(fretboard.strings().back().distance_from_bridge(0), instrument.scale_length[0]);
}

- (void)testPrependicularFretAtNut {
    Instrument instrument;
    instrument.perpendicular_fret_index = 0;
    Fretboard fretboard(instrument);
    
    XCTAssertEqual(fretboard.fret_slots().size(), 25);
    XCTAssertEqual(fretboard.strings().front().distance_from_bridge(0), instrument.scale_length[1]);
    XCTAssertEqual(fretboard.strings().back().distance_from_bridge(0), instrument.scale_length[0]);
}

- (void)testStringPerpendicularFretAtNut {
    double scale = 635;
    double nut_dist = 10;
    String string(0, scale, 0, 0, 0, true, nut_dist);
    printf("X at start: %lf\n", string.x_at_start());
    printf("X at nut: %lf\n", string.x_at_nut());
    printf("X at bridge: %lf\n", string.x_at_bridge());

    XCTAssertEqual(string.x_at_bridge() - string.x_at_start(), scale);
    XCTAssertEqual(string.x_at_nut(), string.x_at_start() - nut_dist);
}


- (void)testStringPerpendicularFretAtBridge {
    double scale = 635;
    double nut_dist = 10;
    String string(0, scale, 0, 0, 0, true, nut_dist);
    printf("X at start: %lf\n", string.x_at_start());
    printf("X at nut: %lf\n", string.x_at_nut());
    printf("X at bridge: %lf\n", string.x_at_bridge());

    XCTAssertEqual(string.x_at_bridge() - string.x_at_start(), scale);
    XCTAssertEqual(string.x_at_nut(), string.x_at_start() - nut_dist);
}

- (void)testStringPerpendicularFretBehindNut {
    double scale = 635;
    double nut_dist = 10;
    String string(0, scale, -3, 0, 0, true, nut_dist);
    printf("X at start: %lf\n", string.x_at_start());
    printf("X at nut: %lf\n", string.x_at_nut());
    printf("X at bridge: %lf\n", string.x_at_bridge());
    
    XCTAssertEqual(string.x_at_bridge() - string.x_at_start(), scale);
    XCTAssertEqual(string.x_at_nut(), string.x_at_start() - nut_dist);
}


//- (void)testPerformanceExample {
//    // This is an example of a performance test case.
//    [self measureBlock:^{
//        // Put the code you want to measure the time of here.
//    }];
//}

@end
